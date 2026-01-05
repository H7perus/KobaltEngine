#pragma once
#include <thread>
#include "GpuInterface/GpuInterface.h"
#include "GpuInterface/VkGpuInterface.h"

#include "SDL3/SDL.h"
#include <SDL3/SDL_vulkan.h>

#include "ClientDLL.h"

namespace KE
{
	class CLIENT_DLL_API Client
	{
	public:

		Client() {};

		int Initialise();

		void Main();

		SDL_Window* window = nullptr;

		IGpuInterface* GpuInterface = new KE::VkGpuInterface();

		//Frames run on the main thread, so while we have three paths, only the "external threads need mentioning
		// 
		//This is where client prediction runs. Moving it to a thread drastically boosts frametime consistency.
		std::thread Prediction;
		//This is where world state calculations run. It runs async to frames and prediction, like in Valve's CS2, except there all calculations (world state and pred) run on the main thread.
		std::thread WorldState;
	};
}