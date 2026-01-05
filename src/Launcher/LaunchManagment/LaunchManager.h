#pragma once

#include <vector>
#include <thread>


#include "Client/Client.h"

//--------------------------------------------------
// The launch manager is supposed to handle 1. Thread/Worker creation and 2. Passing along launch arguments as commands and convars.
// For Threads/Workers, this means launching the main loop, allocating threads for Prediction, Client ticks and Server ticks (depending on 
//--------------------------------------------------




namespace KE
{
	class LaunchManager
	{
		void Run();

	private:
		std::vector<std::thread> workers_;
	};
}