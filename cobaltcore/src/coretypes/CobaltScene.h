#pragma once
#include <iostream>
#include <set>
#include <vector>

#include "renderertypes/DrawInfo.h"
#include "BaseEntity.h"

class CobaltScene
{
	std::vector<ObjectDrawInfo> StaticMeshes;
	std::vector<BaseEntity*> Entities;
};

