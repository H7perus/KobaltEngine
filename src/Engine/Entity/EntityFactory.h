#pragma once

#include <string>
#include <functional>
#include "Engine/Entity/Entity.h"

#include "EngineDLL.h"

namespace KE
{
	class ENGINE_DLL_API EntityFactory
	{
	public:
		using Creator = std::function<std::shared_ptr<Entity>()>;

		template <typename T>
		static void RegisterEntity(std::string EntityTypeName)
		{
			static_assert(std::is_base_of<Entity, T>::value,
				"T must derive from Entity");
			creators_[EntityTypeName] = [] {return std::make_shared<T>(); };
		}

		static std::shared_ptr<Entity> Create(std::string& EntityTypeName);

		static std::shared_ptr<Entity> Create(std::string EntityTypeName);

	private:
		static std::unordered_map<std::string, Creator> creators_;
	};
}