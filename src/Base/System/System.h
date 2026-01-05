#pragma once
#include "Base/TypeSystem/ReflectedType.h"


namespace KE
{
	class ISystem : public IReflectedType
	{
	public:
		//virtual void Run() = 0;

	};
}


#define KE_SYSTEM(TypeName) \
	KE_REFLECT(TypeName) \
	//we will get to this


#define KE_FRAME_SYSTEM(TypeName)

#define KE_TICK_SYSTEM(TypeName)
