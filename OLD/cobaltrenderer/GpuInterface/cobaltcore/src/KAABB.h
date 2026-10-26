#pragma once
#include "glm/glm.hpp"
#include "renderertypes/LineVert.h"

#include <vector>

struct KAABB
{
	//a = lower bound, b = upper bound
	glm::vec3 a = glm::vec3(0), b = glm::vec3(0);

	std::vector<LineSegment> GetWireframeLines(glm::vec3 color)
	{
		std::vector<LineSegment> ret;
		for (int i = 0; i < 12; i++)
		{
			glm::vec3 start, end;

			start = a;


			if (i < 4)
			{

				bool xYes = (i % 2);
				bool yYes = (i > 1);

				start.x = xYes ? b.x : a.x;
				start.y = yYes ? b.y : a.y;

				end = glm::vec3((!yYes) ? b.x : a.x, (xYes) ? b.y : a.y, a.z);
			}
			if (i > 3 && i < 8)
			{

				bool xYes = (i % 2);
				bool yYes = (i > 5);

				start.x = xYes ? b.x : a.x;
				start.y = yYes ? b.y : a.y;

				end.x = xYes ? b.x : a.x;
				end.y = yYes ? b.y : a.y;
				end.z = b.z;
			}
			if (i > 7)
			{

				bool xYes = (i % 2);
				bool yYes = (i > 9);

				start.x = xYes ? b.x : a.x;
				start.y = yYes ? b.y : a.y;
				start.z = b.z;

				end = glm::vec3((!yYes) ? b.x : a.x, (xYes) ? b.y : a.y, b.z);
			}

			LineSegment seg;
			seg.a = start;
			seg.b = end;
			seg.SetColor(color);
			ret.push_back(seg);
		}
		return ret;
	}
};