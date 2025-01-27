#pragma once 
#include <string>
#include <filesystem>

#include "SDL2/SDL.h"
#include "cobaltrenderer.h"

#include "tiny_gltf.h"

#include "coredll.h"
#include "renderertypes/RenderObject.h"



class CORE_DLL_API CobaltEngineCore
{
public:
	tinygltf::TinyGLTF loader;

	SDL_Window* Window;
	CobaltRenderer Renderer;
	std::vector<DrawInfoGroup> RObjects;

	CobaltEngineCore();

	int Init(std::string WindowTitle);

	void LoadModel(PBRMat* Material);

	void LoadGLTFasScene(std::string path);
};