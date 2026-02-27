#pragma once

#include "../ShaderCompile/SlangCompiledUnit.h"

#include "vulkan/vulkan.hpp"


namespace KE::VK
{
	class Pipeline
	{
	protected:
		vk::Pipeline pipeline_;

	public:
		virtual void Recreate(SlangCompiledUnit shader) {};
	};
}


