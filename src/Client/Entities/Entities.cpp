#include "Engine/Entity/EntityFactory.h"
#include "Renderer/Types/EntityComponents/StaticMeshComponent.h"


namespace KE
{
    KE_ENTITYARCHETYPE(
    TestDecorationEntity,
{
    GetKey<KE::R::StaticMeshComponent>()
}
)

}
