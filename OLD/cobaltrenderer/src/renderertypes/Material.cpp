#include "Material.h"

int GetMatSize(std::type_index materialID)
{
    return MaterialSizes[materialID];
}