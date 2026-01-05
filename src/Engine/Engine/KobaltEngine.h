#pragma once
#include <vector>
#include <thread>

namespace KE
{
	class Engine
	{
		


		void Run();

		void Exit();

		std::vector<std::thread> workers_;
	};
}

