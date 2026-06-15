#pragma once
#include "BasicTypeAliases.h"
#include "Engine/Entity/EntityTransform.h"
#include "Engine/Entity/EntityComponent.h"



#include <memory>

//------------------------------------------------------------
// Entity type. Needs to register itself by name into the factory!
//------------------------------------------------------------
namespace KE
{
	class Entity
	{
		friend class EntityWorld;
		friend class EntityFactory;
		// 0 = entity not loading yet, 1 = entity loading, 2 = entity loaded, 3 = entity destroyed
		u8 state_ = 0;

		//type info
		u64 typeID_;
		std::string entityTypeName_;

		//unique entity ID
		u32 entityID_ = 0;

		Transform transform_;

		std::vector<std::shared_ptr<IEntityComponent>> components_;

	public:
		Entity(std::string entityTypeName, u64 typeID) : entityTypeName_(entityTypeName), typeID_(typeID) {};

		template<std::derived_from<IEntityComponent> T>
		std::shared_ptr<T> GetComponentPtr()
		{
			for(std::shared_ptr<IEntityComponent> &component : components_)
			{
				if(T::GetStaticTypeID() == component->GetTypeID())
				{
					return std::static_pointer_cast<T>(component);
				}
			}
			//TODO: This should straight up except. No tolerance for silent failure
			return nullptr;
		}

		bool IsValid() { return state_ == 2;};
	};

	struct EntityKey
	{
		u64                                                entityTypeID;

		EntityKey(u64 entityTypeID) : entityTypeID(entityTypeID)
		{
		}
	};

}
	