#include <string>
#include <unordered_map>

#include "BasicTypeAliases.h"
#include "TypeInfo.h"

#include "BaseDLL.h"

namespace KE
{
	//Purpose: Make sure the unordered map doesn't do extra work. We want to create entities both by string and ID, so it is beneficial to have control over the hashing.
struct IdentityHash
{
    size_t operator()(uint64_t v) const noexcept
    {
        return v;
    }
};


class BASE_DLL_API TypeRegister
{

  private:
    static std::unordered_map<u64, KE::TypeInfo, KE::IdentityHash> types;

    static u32 incrementingID;

  public:
    static KE::TypeInfo* RegisterType(const char* typeName);
};
} // namespace KE
