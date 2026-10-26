#pragma once
#include "GL/glew.h"
#include "glm/glm.hpp"

struct UniformCommons
{
	glm::vec4 CameraPos;
	glm::mat4 Transform;
	GLfloat gamma;
};