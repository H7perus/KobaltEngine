#pragma once

#include <filesystem>
#include <iostream>
#include "SDL2/SDL.h"
#undef main
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "renderertypes/Shader.h"
#include "renderertypes/GLBuffer.h"
#include "renderertypes/RenderGroup.h"
#include "renderertypes/Material.h"
#include "renderertypes/VertexBufferGroup.h"



#include "rendererdll.h"
#include <deque>

//#include "stb_image.h"
#include "stb_image.h"
#include "tiny_gltf.h"

//are these even used wtf
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 820;

std::vector < std::pair<std::string, uint32_t> > DefaultVBOElements{ { "POSITION", 3 }, { "NORMAL", 3 }, {"TANGENT", 3}, {"TEXCOORD_0", 2}}; //, {"TEXCOORD_1", 2}
std::vector < std::pair<std::string, uint32_t> > DefaultVBOElements123{ { "POSITION", 3 }, { "NORMAL", 3 }, { "TEXCOORD_0", 2 } };
std::vector < std::pair<std::string, uint32_t> > AnimatedVBOElements{ { "POSITION", 3 }, { "NORMAL", 3 }, { "TEXCOORD_0", 2 }, { "TEXCOORD_1", 2 } };


struct DrawCommand
{
	uint64_t group[2];
	unsigned int  count;
	unsigned int  instanceCount;
	unsigned int  firstIndex;
	int  baseVertex;
	unsigned int  baseInstance;
	unsigned int objectID;
	unsigned int modelmatrixID;
};


//Eventual TODO might be: split render groups into vertex + indices and data buffers. That way we can reuse vertex data for other materials.

class RENDERER_DLL_API  CobaltRenderer //only OpenGL for now, maybe never anything else.
{
public:

	SDL_Window* Window; //The Core will also hold this, though for different purposes like handling inputs(?)

    SDL_GLContext Context;

	//This will replace RenderGroups
	// 
	std::vector<VertexBufferGroup> VBGroups; //VBG contains VAO, VBO and info about contained 
	std::vector<GLBuffer> MaterialGroups; //each material type gets its own buffer


	std::vector<DrawElementsIndirectCommand> CPUCommandBuffer;
	GLBuffer CommandBuffer = GLBuffer(GL_DRAW_INDIRECT_BUFFER, GL_DYNAMIC_DRAW);

	//std::vector<Shader> Shaders; //each Shader and each Shader variant(like ifdefs) will be represented here.

	//GOAL: Have all incoming Draw commands sorted by VBG, Material and Shader.
	//		All calls where all 3 match will be grouped into one final draw call.
	//		Only got to figure out now how to integrate external data like matrices and bone data...thats for later, first only do model mats in one buffer.
	GLBuffer ModelMatrixBuffer = GLBuffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW);
	std::vector<glm::mat4> CPUModelMatrixBuffer; //need to find a way to check when shit gets deallocated.


	//std::vector< std::vector<std::shared_ptr<RenderGroup>> > RenderGroups; //this is fucking stupid. We are forcing materials to be part of the groups instead of separate. This is not flexible and not performant and requires possibly more drawcalls and bufferbinds
	std::vector< std::shared_ptr<GLTexture> > Textures;


	//we can have multiple render groups with the same material. Different material == new column!
	std::vector<std::type_index> group_data_types;

	std::vector<std::shared_ptr<DrawInfoGroup>> DrawInfoGroups; //name speaks for itself

	//std::vector<std::shared_ptr<RenderObject>> RenderObjects;


	Shader shader;

	GLBuffer UBO = GLBuffer(GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW);


	//all VBOs used for normal 3D draw must! follow the order, even if not all element are used.



	CobaltRenderer();

	void Init(SDL_Window* SDL_Window);

	void Setup();

	void AddRenderGroup(uint64_t block_size, std::type_index type);
	//check if exists, if it does, return that objects type back. RenderObject is supposed to be lean such that its creation/destruction is basically irrelevant.

	void ShareModelMatrices();

	//std::shared_ptr<RenderObject> GetRenderObject(RenderObjectRequest& Request);

	VertLocator SubmitPrimitive(PrimitiveDataBlock &primitive);

	std::shared_ptr<DrawInfoGroup> SubmitPrimitiveList(PrimitiveDataList primitive);

	int SubmitMaterial(PBRMat& material);


	//std::shared_ptr<RenderObject> GetRenderObject(RenderObjectRequest& Request);

	void SubmitDrawCommand(DrawCommand &cmd);

	void Draw();

	PBRMat GetMaterial(tinygltf::Model& model, int matindex);
};







