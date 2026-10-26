#pragma once
#include "glm/glm.hpp"
#include <limits>
#include "KCollideTriangle.h"

class KRay
{
public:
	glm::vec3 origin;
	glm::vec3 direction;


	//shamelessly copied from wikipedia.org/wiki/Möller-Trumbore_intersection_algorithm
	bool intersectTri(KCollideTriangle triangle, glm::vec3 &intersect_point, glm::vec3 &intersectnormal, float &t)
	{
		constexpr float epsilon = std::numeric_limits<float>::epsilon();
		
		glm::vec3 edge1 = triangle.b - triangle.a;
		glm::vec3 edge2 = triangle.c - triangle.a;
		glm::vec3 ray_cross_e2 = glm::cross(direction, edge2);
		float det = dot(edge1, ray_cross_e2);

		if (det > -epsilon && det < epsilon)
		{
			return false;
		}

		glm::vec3 normal = triangle.normal;
		if (dot(normal, direction) > 0)
		{
			return false;
		}
		

		float inv_det = 1.0 / det;

		glm::vec3 s = origin - triangle.a;

		float u = inv_det * glm::dot(s, ray_cross_e2);

		if ((u < 0 && abs(u) > epsilon) || (u > 1 && abs(u - 1) > epsilon))
			return false;

		glm::vec3 s_cross_e1 = cross(s, edge1);
		float v = inv_det * dot(direction, s_cross_e1);

		if ((v < 0 && abs(v) > epsilon) || (u + v > 1 && abs(u + v - 1) > epsilon))
			return false;

		float lt = 1.0;
		// At this stage we can compute t to find out where the intersection point is on the line.
		lt = inv_det * dot(edge2, s_cross_e1);

		if (lt > epsilon) // ray intersection
		{
			t = lt;
			intersectnormal = normal;
			intersect_point = glm::vec3(origin + direction * t);
			return true;
		}
		else // This means that there is a line intersection but not a ray intersection.
			return false;
	}
};

