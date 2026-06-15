#pragma once
#include "BasicTypeAliases.h"
#include <string>

namespace KE {

	struct TypeInfo
	{
		u64 typeID = 0;
		std::string typeName;
	};
}