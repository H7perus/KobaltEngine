#pragma once
#include "Client.h"
#include "Frame/Frame.h"

#include "Base/Time/LoopRateLimiter.h"

namespace KE
{
	int Client::Initialise()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			printf("SDL initialization failed: %s\n", SDL_GetError());
			return 1;
		}

		window = SDL_CreateWindow(
			"My SDL Window",              // Window title
			800,                          // Width
			600,                          // Height
			SDL_WINDOW_VULKAN              // Flags
		);

		if (!window) {
			printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
			SDL_Quit();
			return 1;
		}

		//SDL_SetWindowFullscreen(window, true);

		GpuInterface->Init();

	}


	void Client::Main()
	{
		LoopRateLimiter fps_limiter(10.f);
		bool isRunning = true;
		while (isRunning)
		{
			fps_limiter.Sleep();
			Frame::RunClientFrame();
		}
	}
}