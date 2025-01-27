#pragma once

#include <typeindex>
#include <malloc.h>
#include <vector>
#include <mutex>


#include "SDL2/sdl.h"

#include "GL/glew.h"
#include "../renderertypes/Shader.h"
#include "DrawInfo.h"

#include "GLTexture.h"
#include "GLBuffer.h"
#include "RenderObject.h"



#include "../rendererdll.h"













class RENDERER_DLL_API RenderGroup
{
public:
    //Now......Do we want to split our EBOs + VBOs + IndirectCmdBuffer away from the DataBuffer? That way we can split materials and models, else we have to double allocate. TODO: Think about this

    std::pair<uint64_t, uint64_t> location{ 0,0 };
    uint64_t size_per_vert; // example: some might use bones, some might not. diff is drastic.

    GLBuffer EBO                =    GLBuffer(GL_ELEMENT_ARRAY_BUFFER,  GL_STATIC_DRAW  );
    GLBuffer VBO                =    GLBuffer(GL_ARRAY_BUFFER,          GL_STATIC_DRAW  );
    GLBuffer IndirectCmdBuffer  =    GLBuffer(GL_DRAW_INDIRECT_BUFFER,  GL_DYNAMIC_DRAW );
    GLBuffer DataBuffer         =    GLBuffer(GL_SHADER_STORAGE_BUFFER, GL_STATIC_DRAW  );

    std::vector< std::shared_ptr<RenderObject> > RenderObjects;


    GLuint VAO = 0; //basics
    Shader shader;

    //int group_type = GL_STATIC_DRAW; //Reusing usage hints, not sure this is halal. Idea was to change command data on GPU only if its dynamic.
    //bool data_changed = false;

    std::mutex CommandEntryLock;
    std::vector<DrawElementsIndirectCommand> commands;

    std::type_index datatype;
    uint64_t block_size;
    std::vector<uint8_t> draw_data; // usually materials. 

    RenderGroup(RenderGroup&& other) noexcept;


    RenderGroup(uint64_t block_size, std::type_index type, std::pair<uint64_t, uint64_t> location);

    void Init();

    void Draw();

    void SubmitCommand(DrawElementsIndirectCommand& cmd);

    std::shared_ptr<RenderObject> AddObject(RenderObjectRequest Request);

    int AddDataEntry(void* data, std::type_index type);
};

