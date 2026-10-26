#pragma once
#include <vector>
#include "GLBuffer.h"

//for all data that should be shared between GPU and CPU unidirectionally: CPU -> GPU
class SharedBuffer
{
	GLBuffer GPUBuffer;
	std::vector<unsigned char> CPUBuffer;
	void TransferData();
	void TransferAndBind();
};

