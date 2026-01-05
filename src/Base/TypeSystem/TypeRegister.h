#include <unordered_map>
#include <string>

#include "BasicTypeAliases.h"
#include "TypeInfo.h"

#include "BaseDLL.h"

namespace KE
{
	class BASE_DLL_API TypeRegister
	{

	private:

		static std::unordered_map<std::string, KE::TypeInfo> types;

		static u32 incrementingID;

	public:

		static KE::TypeInfo* RegisterType(std::string typeName);

	};
}
