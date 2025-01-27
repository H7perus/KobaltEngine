#pragma once
#include "RenderGroup.h"

#include "stb_image.h"
#include "tiny_gltf.h"

RenderGroup::RenderGroup(RenderGroup&& other) noexcept
: EBO(std::move(other.EBO)),
VBO(std::move(other.VBO)),
IndirectCmdBuffer(std::move(other.IndirectCmdBuffer)),
DataBuffer(std::move(other.DataBuffer)),
shader(std::move(other.shader)),
VAO(other.VAO),
commands(std::move(other.commands)),
datatype(other.datatype),
block_size(other.block_size),
draw_data(std::move(other.draw_data)) {
    other.VAO = 0; // Reset the moved-from VAO if needed
}

RenderGroup::RenderGroup(uint64_t block_size, std::type_index type, std::pair<uint64_t, uint64_t> location) : block_size(block_size), datatype(type), location(location)
{
}

void RenderGroup::Init()
{

    //EBO.Data(sizeof(indices), indices);
    EBO.Allocate(1000000, NULL);
    //EBO.DataAtEnd(sizeof(indices), indices);
    EBO.Bind();

    //VBO.Data(sizeof(vertices), NULL);
    //VBO.Data(sizeof(vertices), vertices);
    VBO.Allocate(1000000, NULL);
    //VBO.DataAtEnd(sizeof(vertices) / 2, vertices);
    VBO.Bind();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    IndirectCmdBuffer.Allocate(10 * sizeof(DrawElementsIndirectCommand), NULL);

}

void RenderGroup::Draw()
{
    //these should only change if they are actually changing and only be subbed if data changed(always true for databuffer, will rarely change).
    //I need a system for dynamic/static drawing to reuse commands as much as possible. Problem for future me.

    IndirectCmdBuffer.Data(commands.size() * sizeof(DrawElementsIndirectCommand), commands.data());

    IndirectCmdBuffer.BindToBaseSSBO(1);
    IndirectCmdBuffer.Bind();



    DataBuffer.Data(draw_data.size(), draw_data.data());
    DataBuffer.BindToBaseSSBO(2);

    EBO.Bind();
    VBO.Bind();

    glBindVertexArray(VAO);

    glMultiDrawElementsIndirect(
        GL_TRIANGLES,
        GL_UNSIGNED_INT, // Type of data in indicesBuffer
        (const void*)0, // No offset into draw command buffer
        commands.size(),
        sizeof(DrawElementsIndirectCommand)
    );
    //std::cout << "CHECK ERROR: " << glGetError() << std::endl;
}

void RenderGroup::SubmitCommand(DrawElementsIndirectCommand &cmd)
{
    std::unique_lock<std::mutex> CommandEntryLock;
    commands.push_back(cmd);
}

std::shared_ptr<RenderObject> RenderGroup::AddObject(RenderObjectRequest Request)
{
    std::shared_ptr<RenderObject> returnObj(new RenderObject(Request.data_type));

    //returnObj->location = location;
    ////HARDCODED, PLEASE FIX:
    //returnObj->VBOfirst = (VBO.used_size) / (8 * 4); //divide by size of vert(3 floats times 4 bytes per float);
    //returnObj->IndexFirst = EBO.used_size / 4;
    //returnObj->IndexCount = Request.num_indices;
    //returnObj->data_index = draw_data.size() / block_size;

    draw_data.resize(draw_data.size() + block_size);
    uint8_t* destptr = draw_data.data() + draw_data.size() - block_size; //move this up two lines, better.
    memcpy(destptr, Request.data_pointer, block_size);

    VBO.DataAtEnd(Request.vertex_size, Request.vertex_data);
    EBO.DataAtEnd(Request.num_indices * sizeof(uint32_t), Request.index_data);
    

    RenderObjects.push_back(returnObj);
    return returnObj;
}

int RenderGroup::AddDataEntry(void* data, std::type_index type)
{
    assert(datatype == type);
    draw_data.resize(draw_data.size() + block_size);


    uint8_t* destptr = draw_data.data() + draw_data.size() - block_size;

    memcpy(destptr, data, block_size);

    return (draw_data.size() - block_size) / block_size;
}