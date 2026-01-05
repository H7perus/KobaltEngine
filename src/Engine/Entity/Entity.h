#pragma once
#include "BasicTypeAliases.h"
#include "Base/TypeSystem/ReflectedType.h"
//#include "Engine/Entity/EntityFactory.h"

//------------------------------------------------------------
// Entity type. Needs to register itself by name into the factory!
//------------------------------------------------------------
namespace KE
{
	class Entity : public IReflectedType
	{
		u8 loadState;
	};
}



//#define KE_ENTITY(TypeName) \
//	KE_REFLECT(TypeName)
//	
//#define KE_REGISTER_ENTITY(TypeName ) \
//KE::EntityFactory::RegisterEntity<TypeName>(#TypeName); \
	