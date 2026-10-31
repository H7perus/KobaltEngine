#pragma once
#include <type_traits>
#include <concepts>
#include <meta>

#include "EntityFactory.h"

namespace KE
{
template <std::meta::info NS>
consteval auto members_of_namespace()
{
    return std::define_static_array(members_of(NS, std::meta::access_context::unchecked()));
}

template <std::meta::info NS>
void RegisterEntitiesIn(EntityFactory& factory)
{
    template for (constexpr auto m : members_of_namespace<NS>())
    {
        if constexpr (is_type(m) && !is_type_alias(m) && is_class_type(m) && is_complete_type(m))
        {
            using T = [:m:];
            if constexpr (std::derived_from<T, Entity::Entity> && !std::same_as<T, Entity::Entity> && !std::is_abstract_v<T>)
            {
                factory.Register<T>();
            }
        }
    }
}
}