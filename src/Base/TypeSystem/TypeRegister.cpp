#include "TypeRegister.h"

#include "Util/FNV1a.h"
#include <cstring>
#include <format>
#include <stdexcept>

u32 KE::TypeRegister::incrementingID = 0;


std::unordered_map<u64, KE::TypeInfo, KE::IdentityHash> KE::TypeRegister::types;


KE::TypeInfo* KE::TypeRegister::RegisterType(const char* typeName)
{
    u64 hashedName = FNV1aTo64(typeName);

    if (!KE::TypeRegister::types.contains(hashedName))
    {
        KE::TypeInfo info{hashedName, typeName};
        KE::TypeRegister::types.insert(std::make_pair(hashedName, info));
    }
    else
    {
        if (strcmp(typeName, KE::TypeRegister::types.at(hashedName).typeName.c_str()) != 0)
            throw std::logic_error(
                std::format("Type already exists or colliding hash! \n Existing name: {}  \n Trying to add: {} ",
                            types.at(hashedName).typeName, typeName));
    }

    return &KE::TypeRegister::types.at(hashedName);
}