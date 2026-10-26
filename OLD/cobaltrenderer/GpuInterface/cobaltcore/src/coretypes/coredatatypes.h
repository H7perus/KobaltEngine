#pragma once
#include "glm/glm.hpp"

#ifdef DOUBLE_PRECISION  //use these types whenever the type would change between double and single precision floating point. Else use glm::vec3 and glm::dvec3
using cvec3 = glm::dvec3;
#else
using cvec3 = glm::vec3;
#endif