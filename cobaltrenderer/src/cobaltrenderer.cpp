#pragma once


#include "cobaltrenderer.h"

#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

CobaltRenderer::CobaltRenderer()
{
}
void CobaltRenderer::Init(SDL_Window* SDLWindow)
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
void CobaltRenderer::Setup()
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

void CobaltRenderer::AddRenderGroup(uint64_t block_size, std::type_index type) {
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


VertLocator CobaltRenderer::SubmitPrimitive(PrimitiveDataBlock &primitive)
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

std::shared_ptr<DrawInfoGroup> CobaltRenderer::SubmitPrimitiveList(PrimitiveDataList PrimitiveList)
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

int CobaltRenderer::SubmitMaterial(PBRMat& material)
{
	int ret = MaterialGroups[0].used_size / sizeof(PBRMat);
	MaterialGroups[0].DataAtEnd(sizeof(PBRMat), &material);
	return ret;
}

void CobaltRenderer::SubmitDrawCommand(DrawCommand& cmd)
{
	void* inters = (char*)&cmd + sizeof(cmd.group);

	DrawElementsIndirectCommand *final_cmd = (DrawElementsIndirectCommand*)inters;

	//RenderGroups[cmd.group[0]][cmd.group[1]]->commands.push_back(*final_cmd);
}

void CobaltRenderer::Draw()
{
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

	VBGroups[0].Bind();

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

PBRMat CobaltRenderer::GetMaterial(tinygltf::Model& model, int matindex)
{
	PBRMat returnmat;

	tinygltf::Material gltfmat = model.materials[matindex];

	for (int ind = 0; ind < 3; ind++)
		returnmat.emissive_factor[ind] = gltfmat.emissiveFactor[ind];

	if (gltfmat.pbrMetallicRoughness.baseColorTexture.index > -1)
	{
		int img_index = model.textures[gltfmat.pbrMetallicRoughness.baseColorTexture.index].source;
		std::cout << "CREATING TEXTURE ALBEDO" << img_index << std::endl;
		bool alreadyexists = false;

		for (std::shared_ptr<GLTexture> Tex : Textures)
			if (Tex->sourcepath == model.images[model.textures[gltfmat.pbrMetallicRoughness.baseColorTexture.index].source].uri)
			{
				alreadyexists = true;
				returnmat.color_factor = glm::vec4(1.0);
				returnmat.albedo = Tex->handle;
				//Tex->MakeResident();
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
		std::cout << "CREATING TEXTURE NORMAL" << img_index << std::endl;
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