#pragma once
#include "Pipeline.h"

#include "RendererDLL.h"

namespace KE::VK
{
	GPUI_DLL_API class PipelineGraphics : public Pipeline
	{
	public:
		PipelineGraphics() {};
		PipelineGraphics(const vk::Device& device, SlangCompiledUnit shader);
	};
}


