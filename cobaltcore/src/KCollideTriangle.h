#pragma once
#include "glm/glm.hpp"


class KCollideTriangle
{
public:
	glm::vec3 a, b, c;
	glm::vec3 AABBA, AABBB;
	glm::vec3 normal;
};

