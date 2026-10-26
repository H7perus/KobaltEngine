#pragma once

#include "Engine/General/Angles.h"

#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/reciprocal.hpp"
#include "glm/gtc/constants.hpp"

inline glm::mat4 infiniteReverseZPerspective(float hfov, float vfov, float zNear) {
    const float h = glm::cot(ToRad(0.5f * vfov));
    const float w = glm::cot(ToRad(0.5f * hfov));
    glm::mat4 result = glm::zero<glm::mat4>();
    result[0][0] = w;
    result[1][1] = -h;
    result[2][2] = 0.0f;
    result[2][3] = 1.0f;
    result[3][2] = zNear;
    return result;
};
