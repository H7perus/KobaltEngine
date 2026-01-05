#pragma once
#include <string>
#include <memory>
#include "BasicTypeAliases.h"


#include "TypeRegister.h"
#include "TypeInfo.h"


namespace KE {

	class IReflectedType
	{
	public:
		inline static KE::TypeInfo const* m_TypeInfo = nullptr;

	public:

		IReflectedType() = default;
		virtual ~IReflectedType() = default;

		virtual u32 GetTypeID() = 0;
		virtual std::string GetTypeName() = 0;
	};

}

#define KE_REFLECT(TypeName) \
	inline  static KE::TypeInfo const* m_TypeInfo = KE::TypeRegister::RegisterType(#TypeName); \
	public: \
	virtual u32 GetTypeID() override {return m_TypeInfo->typeID;} \
	virtual std::string GetTypeName() override { return #TypeName; } \
	private: \
