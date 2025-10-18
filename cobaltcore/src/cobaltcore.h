#pragma once 
#include <string>
#include <filesystem>

#include "SDL3/SDL.h"
#include "cobaltrenderer.h"

#include "utils/KInputManager.h"
#include "utils/cobaltclock.h"
#include "KGameState.h"
#include "KRay.h"
#include "tiny_gltf.h"

#include "coredll.h"
#include "renderertypes/RenderObject.h"



class CORE_DLL_API KEngineCore
{
public:
	tinygltf::TinyGLTF loader;

	SDL_Window* Window;

	KGameState gamestate;

	

	KInputManager input_manager;
	RealtimeCobaltClock clock;
	KDuration lastFrame;

	bool test = true;
	float normalmix = 0.3;
	int texindex = 0;

	GLCobaltRenderer Renderer;
	std::vector<DrawInfoGroup> RObjects;

	KEngineCore();

	int Init(std::string WindowTitle);

	void RunFrame();

	void LoadModel(PBRMat* Material);

	void LoadGLTFasPhysics(std::string path);
	void LoadGLTFasScene(std::string path);
};