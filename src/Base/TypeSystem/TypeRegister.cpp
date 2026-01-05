#include "TypeRegister.h"


u32 KE::TypeRegister::incrementingID = 0;
std::unordered_map<std::string, KE::TypeInfo> KE::TypeRegister::types;

KE::TypeInfo* KE::TypeRegister::RegisterType(std::string typeName)
{
	if (!KE::TypeRegister::types.contains(typeName))
	{
		KE::TypeInfo info{ KE::TypeRegister::incrementingID++, typeName };
		KE::TypeRegister::types.insert(std::make_pair(typeName, info));
	}

	return &KE::TypeRegister::types.at(typeName);
}