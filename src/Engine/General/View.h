#pragma once

#include "Engine/General/Angles.h"
#include "Engine/General/Perspective.h"
#include "glm/glm.hpp"



namespace KE
{

class View
{
  public:
    glm::vec<3, glm::f64> origin = glm::vec3(0);
    glm::vec3             forward = glm::vec3(0, 1, 0);
    glm::vec3             up = glm::vec3(0, 0, 1);

    // default is CS HDEG4:3 90deg
    float hFov = 106.260205;
    float vFov = 73.7397953;

    bool isOrthographic = false;

    inline void SetAnglesEuler(float yaw, float pitch)
    {
        forward = glm::vec3(
            cos(ToRad(yaw)) * cos(ToRad(pitch)), 
            sin(ToRad(yaw)) * cos(ToRad(pitch)), 
            sin(ToRad(pitch)));

        up = glm::vec3(
            -cos(ToRad(yaw)) * sin(ToRad(pitch)),
            -sin(ToRad(yaw)) * sin(ToRad(pitch)),
            cos(ToRad(pitch))
        );


    }

    inline glm::mat4 GetProjectionRotationMatrix(float zNear)
    {

        glm::mat4 swap(0);

        swap[0][0] = 1;
        swap[1][2] = 1;
        swap[2][1] = 1;

        swap[3][3] = 1;

        glm::mat4 perspective = infiniteReverseZPerspective(hFov, vFov, 0.01);

        glm::mat4 viewMat = glm::mat4(glm::vec4(glm::cross(forward, up), 0), glm::vec4(forward, 0), glm::vec4(up, 0),
                                      glm::vec4(0, 0, 0, 1));

        return perspective * swap * glm::transpose(viewMat);
    }
};

} // namespace KE