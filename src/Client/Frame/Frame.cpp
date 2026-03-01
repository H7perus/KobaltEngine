#pragma once
#include "Frame.h"

#include <iostream>
#include <print>

#include "Base/Time/Clocks.h"

void KE::Frame::RunClientFrame()
{
	//Simulate, noclip
	std::cout << f32(KE::GlobalClock::QueryGlobalTime()) << std::endl;
	//Draw,



}