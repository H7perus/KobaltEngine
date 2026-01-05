#pragma once


#include "Base/TypeSystem/ReflectedType.h"

#include "Base/TypeSystem/TypeInfo.h"

#include "BasicTypeAliases.h"

#include "Engine/Entity/Entity.h"

class TestClass : public KE::IReflectedType
{
	KE_REFLECT(TestClass)
};


class TestEntity : public KE::Entity
{
	KE_REFLECT(TestEntity)
};

//KE_REGISTER_ENTITY(TestEntity)