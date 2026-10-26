#pragma once
#include <cmath>


inline float ToRad(float angle)
{
    return angle * acos(-1) / 180;
}

inline float ToDeg(float angle)
{
    return angle * 180 / acos(-1);
}