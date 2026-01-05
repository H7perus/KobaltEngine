#include "EntityFactory.h"

namespace KE
{
		using Creator = std::function<std::shared_ptr<Entity>()>;
		std::unordered_map<std::string, Creator> EntityFactory::creators_;

		std::shared_ptr<Entity> EntityFactory::Create(std::string& EntityTypeName)
		{
			if (creators_.contains(EntityTypeName))
			{
				return creators_[EntityTypeName]();
			}

			return nullptr;
		}

		std::shared_ptr<Entity> EntityFactory::Create(std::string EntityTypeName)
		{
			if (creators_.contains(EntityTypeName))
			{
				return creators_[EntityTypeName]();
			}
			
			return nullptr;
		}

}