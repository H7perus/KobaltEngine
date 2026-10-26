#pragma once


#include "glm/ext/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "BasicTypeAliases.h"

namespace KE
{
    class Transform
    {
        //TODO: Review privacy of these members
        public:
        glm::vec<3, glm::f64> position_ = {0, 0, 0};
        glm::mat3 localTransform_ = glm::identity<glm::mat3>();



        Transform() = default;
        Transform(glm::vec<3, glm::f64> position, glm::mat3 localTransform) : position_(position), localTransform_(localTransform) {}
        Transform(glm::mat4 transform)
        {
            localTransform_ = glm::mat3(transform);
            position_ = transform[3];
        }
        Transform(glm::mat<4, 4, glm::f64> transform)
        {
            localTransform_ = glm::mat3(transform);
            position_ = transform[3];
        }

        inline glm::mat3x4 GetRelativeTransform(glm::vec<3, glm::f64> center) const
        {
            auto retMat = glm::mat4x3(localTransform_);
            retMat[3] = glm::vec3(position_ - center);
            return glm::transpose(retMat);
        }

    };
}