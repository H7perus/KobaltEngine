#pragma once
#include "glm/glm.hpp"

struct LineSegment
{
	glm::vec3 a;
	glm::vec3 colora;
	glm::vec3 b;
	glm::vec3 colorb;
	void SetColor(glm::vec3 color)
	{
		colora = color;
		colorb = color;
	}

};