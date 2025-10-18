#pragma once


#include "cobaltrenderer.h"

#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

GLCobaltRenderer::GLCobaltRenderer()
{
}
void GLCobaltRenderer::Init(SDL_Window* SDLWindow)
{
	Window = SDLWindow;
	Context = SDL_GL_CreateContext(Window);
	if (Context == NULL) {
		fprintf(stderr, "Failed to create GL context\n");
		SDL_DestroyWindow(Window);
		SDL_Quit();
	}

	SDL_GL_SetSwapInterval(1); // TODO: Make this game exposed for settings etc...

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK,
		SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glewInit();
}
void GLCobaltRenderer::Setup()
{
	shader = Shader(shader_path + "shaders/MDI_test.vert", shader_path + "shaders/MDI_test.frag");
	shader.use();

	MaterialGroups.push_back(GLBuffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW));

	MaterialGroups[0].Allocate(100000, NULL);

	VBGroups.push_back(VertexBufferGroup(DefaultVBOElements));
	VBGroups[0].VBO.Allocate(1000000000, NULL);
	VBGroups[0].EBO.Allocate(1000000000, NULL);
	VBGroups[0].CreateVAO();

	glm::mat4 modelmat = glm::mat4(1.0);

	UBO.Allocate(sizeof(modelmat), &modelmat);

	VBGroups[0].SetVAOAttributes();


}

void GLCobaltRenderer::AddRenderGroup(uint64_t block_size, std::type_index type) {
	//RenderGroups[0].emplace_back(block_size, type);
}

//std::shared_ptr<RenderObject> CobaltRenderer::GetRenderObject(RenderObjectRequest &Request)
//{
//	int i = 0;
//	std::shared_ptr<RenderObject> returnObj;
//	//CHECK IF TYPE ALREADY EXISTS IN GROUPS
//	for (std::type_index& group_type : group_data_types ) //check if we already have a column for that type. 
//	{
//		if (Request.data_type == group_type)
//			break;
//		i++;
//	}
//
//	//CHECK IF OBJECT ALREADY EXISTS
//	if (i != group_data_types.size())
//	{
//		//IF(ALREADY EXISTS)
//		// RETURN THAT OBJECT
//		// TODO
//		//int j = 0;
//		//while()
//		returnObj = RenderGroups[i].back()->AddObject(Request);
//	}
//	else
//	{
//		group_data_types.push_back(Request.data_type);
//		RenderGroups.resize(i + 1); 
//
//		std::shared_ptr<RenderGroup> RG(new RenderGroup(sizeof(PBRMat), Request.data_type, std::pair<uint64_t, uint64_t>(i, RenderGroups[i].size())));
//		RG->Init();
//		RenderGroups[i].push_back( RG );
//		returnObj = RenderGroups[i].back()->AddObject(Request);
//	}
//	RenderObjects.push_back(returnObj);
//	return returnObj;
//}


VertLocator GLCobaltRenderer::SubmitPrimitive(PrimitiveDataBlock &primitive)
{
	VertLocator VertLocator;

	int size_per_vert = 0;

	for (std::pair<std::string, uint32_t>& parameter : primitive.vertformat)
		size_per_vert += parameter.second * 4;


	VertLocator.first_vert = VBGroups[0].VBO.used_size / size_per_vert;
	VertLocator.first_index = VBGroups[0].EBO.used_size / sizeof(uint32_t);
	VertLocator.count = primitive.indexcount;
	VertLocator.num_verts = primitive.vertcount;

	VBGroups[0].VBO.DataAtEnd(primitive.vertcount * size_per_vert, primitive.vertdata);
	VBGroups[0].EBO.DataAtEnd(primitive.indexcount * sizeof(uint32_t), primitive.indexdata);


	//InfoGroup->DrawActions.push_back(Primitive);
	return VertLocator;
}

std::shared_ptr<DrawInfoGroup> GLCobaltRenderer::SubmitPrimitiveList(PrimitiveDataList PrimitiveList)
{
	std::shared_ptr<DrawInfoGroup> InfoGroup = DrawInfoGroups.emplace_back(std::make_shared<DrawInfoGroup>());
	//auto& InfoGroup = DrawInfoGroups.emplace_back(std::make_shared<DrawInfoGroup>());


	//std::shared_ptr<DrawInfoGroup> InfoGroup;

	for (PrimitiveDataBlock& primitive : PrimitiveList.datalist)
	{
		SingleDraw Primitive;

		int size_per_vert = 0;

		for (std::pair<std::string, uint32_t>& parameter : primitive.vertformat)
			size_per_vert += parameter.second * 4;


		Primitive.VertLocator.first_vert	=	VBGroups[0].VBO.used_size / size_per_vert;
		Primitive.VertLocator.first_index	=	VBGroups[0].EBO.used_size / sizeof(uint32_t);
		Primitive.VertLocator.count			=	primitive.indexcount;
		Primitive.materialindex				=	primitive.materialindex;

		VBGroups[0].VBO.DataAtEnd(primitive.vertcount * size_per_vert, primitive.vertdata);
		VBGroups[0].EBO.DataAtEnd(primitive.indexcount * sizeof(uint32_t), primitive.indexdata);

				
		InfoGroup->DrawActions.push_back(Primitive);

	}

	return InfoGroup;


}

int GLCobaltRenderer::SubmitMaterial(PBRMat& material)
{
	int ret = MaterialGroups[0].used_size / sizeof(PBRMat);
	MaterialGroups[0].DataAtEnd(sizeof(PBRMat), &material);
	return ret;
}

void GLCobaltRenderer::SubmitDrawCommand(DrawCommand& cmd)
{
	void* inters = (char*)&cmd + sizeof(cmd.group);

	DrawElementsIndirectCommand *final_cmd = (DrawElementsIndirectCommand*)inters;

	//RenderGroups[cmd.group[0]][cmd.group[1]]->commands.push_back(*final_cmd);
}

void GLCobaltRenderer::Draw()
{
	VBGroups[0].Bind();

	glClearColor(0.15f, 0.0f, 0.15f, 1.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CPUCommandBuffer.clear();
	for (std::shared_ptr<DrawInfoGroup>& DrawInfo : DrawInfoGroups)
	{
		for (SingleDraw& SD : DrawInfo->DrawActions)
		{
			CPUCommandBuffer.push_back(SD.GetCommand());
		}
	}
	//CPUCommandBuffer[0] = CPUCommandBuffer[1];
	//CPUCommandBuffer.resize(5);
	CommandBuffer.Data(CPUCommandBuffer.size() * sizeof(CPUCommandBuffer[0]), CPUCommandBuffer.data());

	ModelMatrixBuffer.Data(sizeof(glm::mat4) * CPUModelMatrixBuffer.size(), CPUModelMatrixBuffer.data());

	CommandBuffer.Bind();
	CommandBuffer.BindToBaseSSBO(1);
	MaterialGroups[0].BindToBaseSSBO(2);
	UBO.BindToBaseUBO(0);

	

	//std::cout << "error before: " << glGetError() << std::endl;

	glMultiDrawElementsIndirect(
		GL_TRIANGLES,
		GL_UNSIGNED_INT, // Type of data in indicesBuffer
		(const void*)0, // No offset into draw command buffer
		CPUCommandBuffer.size(),
		sizeof(DrawElementsIndirectCommand)
	);
	//std::cout << "error after: " << glGetError() << std::endl;

	//SDL_GL_SwapWindow(Window);
}

/*void GLCobaltRenderer::SetupLines(std::vector<glm::vec3> verts, std::vector<uint32_t> indices)

void GLCobaltRenderer::DrawWireframes(std::vector<glm::vec3> verts, std::vector<uint32_t> indices)
{
	GLuint lVAO;
	GLBuffer lVBO = GLBuffer(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3), verts.data());
	GLBuffer lEBO = GLBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data());

	Shader lineshader(shader_path + "shaders/line.vert", shader_path + "shaders/line.frag");

	lVBO.Bind();
	glGenVertexArrays(1, &lVAO);
	glBindVertexArray(lVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	lineshader.use();
	lEBO.Bind();

	glLineWidth(1.5);

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	shader.use();
}*/

void GLCobaltRenderer::SetupLines(std::vector<glm::vec3>& verts, std::vector<uint32_t>& indices)
{
	/*std::vector<uint32_t> lineindices;

	for (int i = 0; i < indices.size() / 3; i++)
	{
		lineindices.push_back(indices[i * 3]);
		lineindices.push_back(indices[i * 3 +1]);
		lineindices.push_back(indices[i * 3 +1]);
		lineindices.push_back(indices[i * 3 +2]);
		lineindices.push_back(indices[i * 3 +2]);
		lineindices.push_back(indices[i * 3]);
	}*/

	lVBO = GLBuffer(GL_ARRAY_BUFFER, lines.size() * sizeof(LineSegment), lines.data());
	//lEBO = GLBuffer(GL_ELEMENT_ARRAY_BUFFER, lines.size() * sizeof(LineSegment), lines.data());


	lVBO.Bind();

	glGenVertexArrays(1, &lVAO);
	glBindVertexArray(lVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


}

int GLCobaltRenderer::AddLineSet(std::vector<LineSegment> &segments)
{
	//returns the index to the lineset pair;
	BBGroups.push_back(std::pair(lines.size() * 2, segments.size() * 2));

	lines.insert(lines.end(), segments.begin(), segments.end());

	return BBGroups.size() - 1;

}

void GLCobaltRenderer::DrawWireframes(int first, int count)
{
	Shader lineshader(shader_path + "shaders/line.vert", shader_path + "shaders/line.frag");
	lineshader.use();
	
	glBindVertexArray(lVAO);
	//lEBO.Bind();
	glLineWidth(1.5);

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDrawElements(GL_LINES, length, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINES, first, count);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);


	//VBGroups[0].Bind();
	shader.use();
}


PBRMat GLCobaltRenderer::GetMaterial(tinygltf::Model& model, int matindex)
{
	PBRMat returnmat;

	tinygltf::Material gltfmat = model.materials[matindex];

	for (int ind = 0; ind < 3; ind++)
		returnmat.emissive_factor[ind] = gltfmat.emissiveFactor[ind];

	if (gltfmat.pbrMetallicRoughness.baseColorTexture.index > -1)
	{
		int img_index = model.textures[gltfmat.pbrMetallicRoughness.baseColorTexture.index].source;
		//std::cout << "CREATING TEXTURE ALBEDO" << img_index << std::endl;
		bool alreadyexists = false;

		for (std::shared_ptr<GLTexture> Tex : Textures)
			if (Tex->sourcepath == model.images[model.textures[gltfmat.pbrMetallicRoughness.baseColorTexture.index].source].uri)
			{
				alreadyexists = true;
				returnmat.color_factor = glm::vec4(1.0);
				returnmat.albedo = Tex->handle;
				Tex->MakeResident();
			}
		
		if (!alreadyexists)
		{
			std::shared_ptr<GLTexture> Texture = std::make_shared<GLTexture>(GL_TEXTURE_2D, GL_RGB, model.images[img_index].width, model.images[img_index].height, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, model.images[img_index].image.data());
			Texture->sourcepath = model.images[model.textures[gltfmat.pbrMetallicRoughness.baseColorTexture.index].source].uri;
			Textures.push_back(Texture);

			returnmat.color_factor = glm::vec4(1.0);
			returnmat.albedo = Texture->handle;
			Texture->MakeResident();
		}

	}
	if (gltfmat.normalTexture.index > -1)
	{
		int img_index = model.textures[gltfmat.normalTexture.index].source;
		//std::cout << "CREATING TEXTURE NORMAL" << img_index << std::endl;
		bool alreadyexists = false;
		for (std::shared_ptr<GLTexture> Tex : Textures)
			if (Tex->sourcepath == model.images[model.textures[gltfmat.normalTexture.index].source].uri)
			{
				alreadyexists = true;
				returnmat.color_factor = glm::vec4(1.0);
				returnmat.normal = Tex->handle;
				//Tex->MakeResident();
			}

		if (!alreadyexists)
		{
			std::shared_ptr<GLTexture> Texture = std::make_shared<GLTexture>(GL_TEXTURE_2D, GL_RGB, model.images[img_index].width, model.images[img_index].height, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, model.images[img_index].image.data());
			Texture->sourcepath = model.images[model.textures[gltfmat.normalTexture.index].source].uri;
			Textures.push_back(Texture);

			returnmat.color_factor = glm::vec4(1.0);
			returnmat.normal = Texture->handle;
			Texture->MakeResident();
		}
	}
	return returnmat;
}