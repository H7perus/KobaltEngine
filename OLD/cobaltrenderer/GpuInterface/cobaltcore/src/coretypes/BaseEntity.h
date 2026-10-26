#pragma once
#include "glm/glm.hpp"
#include "coredatatypes.h"
#include "renderertypes/DrawInfo.h"
#include <vector>





class BaseEntity
{
public:
	cvec3 pos;
	ObjectDrawInfo DrawInfo;
};

