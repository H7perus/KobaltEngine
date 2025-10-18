#pragma once
#include "glm/glm.hpp"

#include "KCollideTriangle.h"
#include "KRay.h"
#include "KBvhNode.h"
#include <vector>
#include <algorithm>

class KPlayerAABB
{
public:
	float height = 1.8, width = 0.8;

	glm::vec3 tA, tB;

	bool traceAgainstWorld(std::vector<KBvhNode*> &tris, glm::vec3 position, glm::vec3 & normal, glm::vec3 delta, float& t)
	{
		glm::vec3 pA = position - glm::vec3(width / 2, width / 2, 1.6f);
		glm::vec3 pB = pA + glm::vec3(width, width, height);

		tA = pA, tB = pB;

		//this gets us a trace AABB, which is essentially the AABB encompassing start and end point player AABBs
		tA = glm::min(tA + delta, tA);
		tB = glm::max(tB + delta, tB);

		//time for some prefiltering. Any tri from the original list is checked whether it is inside the trace AABB and we then try finding the trace fraction that the tri AABB first enters the trace AABB.
		//Then we sort by this fraction and if we reach a triangle whose fraction is > our current collision fraction we exit.
		std::vector<std::pair<float, KCollideTriangle*>> collidableTris;



		for (KBvhNode* node : tris)
		{

			glm::bvec3 separation = glm::bvec3(glm::max( (node->bb.a - tB), glm::vec3(0))) || glm::bvec3(glm::max((tA - node->bb.b), glm::vec3(0)));

			bool isSeparate = glm::max(glm::max(separation.x, separation.y), separation.z);
			if (isSeparate)
				continue;

			glm::bvec3 baseSeparation = glm::bvec3(glm::max((node->bb.a - pB), glm::vec3(0))) || glm::bvec3(glm::max((pA - node->bb.b), glm::vec3(0)));
			bool isBaseSeparate = glm::max(glm::max(baseSeparation.x, baseSeparation.y), baseSeparation.z);
			if (!isBaseSeparate)
			{
				collidableTris.push_back(std::pair(0.0f, node->tris[0]));
				continue;
			}

			//on the low side
			glm::vec3 spatialseparation = glm::max(glm::vec3(node->bb.a - pB), glm::vec3(pA - node->bb.b));
			float fraction = glm::max(glm::max(spatialseparation.x / abs(delta.x), spatialseparation.y / abs(delta.y)), spatialseparation.z / abs(delta.z));

			if (fraction <= 1.0)
			{
				collidableTris.push_back(std::pair(fraction, node->tris[0]));
			}

		}

		std::sort(collidableTris.begin(), collidableTris.end(), [](auto& left, auto& right) {
			return left.first < right.first;
			});

		for (std::pair<float, KCollideTriangle*>& tri : collidableTris)
		{
			if (tri.first >= t)
			{
				break;
			}
			traceAgainstTri(*tri.second, position, normal, delta, t);
			traceAgainstEdge(tri.second->a, tri.second->b, position, normal, delta, t);
			traceAgainstEdge(tri.second->b, tri.second->c, position, normal, delta, t);
			traceAgainstEdge(tri.second->c, tri.second->a, position, normal, delta, t);
		}
		if (t < 1.0)
			return true;
		else return false;
	}

	bool traceAgainstWorld(std::vector<KCollideTriangle> &tris, glm::vec3 position, glm::vec3& normal, glm::vec3 delta, float& t)
	{
		glm::vec3 pA = position - glm::vec3(width / 2, width / 2, 1.6f);
		glm::vec3 pB = pA + glm::vec3(width, width, height);

		tA = pA, tB = pB;

		//this gets us a trace AABB, which is essentially the AABB encompassing start and end point player AABBs
		tA = glm::min(tA + delta, tA);
		tB = glm::max(tB + delta, tB);

		//time for some prefiltering. Any tri from the original list is checked whether it is inside the trace AABB and we then try finding the trace fraction that the tri AABB first enters the trace AABB.
		//Then we sort by this fraction and if we reach a triangle whose fraction is > our current collision fraction we exit.
		std::vector<std::pair<float, KCollideTriangle*>> collidableTris;



		for (KCollideTriangle& tri : tris)
		{

			glm::bvec3 separation = glm::bvec3(glm::max((tri.AABBA - tB), glm::vec3(0))) || glm::bvec3(glm::max((tA - tri.AABBB), glm::vec3(0)));

			bool isSeparate = glm::max(glm::max(separation.x, separation.y), separation.z);
			if (isSeparate)
				continue;

			glm::bvec3 baseSeparation = glm::bvec3(glm::max((tri.AABBA - pB), glm::vec3(0))) || glm::bvec3(glm::max((pA - tri.AABBB), glm::vec3(0)));
			bool isBaseSeparate = glm::max(glm::max(baseSeparation.x, baseSeparation.y), baseSeparation.z);
			if (!isBaseSeparate)
			{
				collidableTris.push_back(std::pair(0.0f, &tri));
				continue;
			}

			//on the low side
			glm::vec3 spatialseparation = glm::max(glm::vec3(tri.AABBA - pB), glm::vec3(pA - tri.AABBB));
			float fraction = glm::max(glm::max(spatialseparation.x / abs(delta.x), spatialseparation.y / abs(delta.y)), spatialseparation.z / abs(delta.z));

			if (fraction <= 1.0)
			{
				collidableTris.push_back(std::pair(fraction, &tri));
			}

		}

		std::sort(collidableTris.begin(), collidableTris.end(), [](auto& left, auto& right) {
			return left.first < right.first;
			});

		for (std::pair<float, KCollideTriangle*>& tri : collidableTris)
		{
			if (tri.first >= t)
			{
				break;
			}
			traceAgainstTri(*tri.second, position, normal, delta, t);
			traceAgainstEdge(tri.second->a, tri.second->b, position, normal, delta, t);
			traceAgainstEdge(tri.second->b, tri.second->c, position, normal, delta, t);
			traceAgainstEdge(tri.second->c, tri.second->a, position, normal, delta, t);
		}
		if (t < 1.0)
			return true;
		else return false;
	}
	bool traceAgainstTri(KCollideTriangle &tri, glm::vec3 &position, glm::vec3 &normal, glm::vec3 &delta, float &t)
	{
		KRay ray;

		float loct = 1.0;

		ray.direction = delta;

		for (int i = 0; i < 8; i++)
		{
			ray.origin = position - glm::vec3(width / 2, width / 2, 1.6f);

			glm::vec3 corneroffset = glm::vec3(width * bool(i & 1), width * bool(i & 2), height * bool(i & 4));

			ray.origin += corneroffset;

			glm::vec3 temp;
			glm::vec3 tempnormal;

			ray.intersectTri(tri, temp, tempnormal, loct);
			if (loct < t)
			{
				t = loct;
				normal = tempnormal;
			}
		}
		if (t < 1.0) return true;
		else return false;
	}
	bool traceAgainstEdge(glm::vec3 &eA, glm::vec3 &eB, glm::vec3 &position, glm::vec3& normal, glm::vec3 &delta, float& t)
	{
		float loct = 1.0;
		glm::vec3 locnormal = glm::vec3(0);

		updateTraceAABB(position, delta * t);

		if(!AABBintersect(glm::min(eA, eB), glm::max(eA, eB)))
			return false;

		for (int i = 0; i < 12; i++)
		{
			glm::vec3 pA, pB;

			pA = position - glm::vec3(width / 2, width / 2, 0);

			if (i < 4)
			{
				pA -= glm::vec3(0, 0, 1.6f);

				bool xYes = (i % 2);
				bool yYes = (i > 1);

				pA.x += xYes * width;
				pA.y += yYes * width;

				pB = pA + glm::vec3((!xYes * !yYes - xYes * yYes) * width, (xYes * !yYes - !xYes * yYes) * width, 0);
			}
			if (i > 3 && i < 8)
			{
				pA -= glm::vec3(0, 0, 1.6f);

				bool xYes = (i % 2);
				bool yYes = (i > 5);

				pA.x += xYes * width;
				pA.y += yYes * width;

				pB = pA + glm::vec3(0, 0, height);
			}
			if (i > 7) 
			{
				pA -= glm::vec3(0, 0, -0.2f);

				bool xYes = (i % 2);
				bool yYes = (i > 9);

				pA.x += xYes * width;
				pA.y += yYes * width;

				pB = pA + glm::vec3((!xYes * !yYes - xYes * yYes) * width, (xYes * !yYes - !xYes * yYes) * width, 0);
			}

			glm::vec3 planenormal_e = glm::cross(delta, eB - eA);
			glm::vec3 planenormal_p = glm::cross(delta, pB - pA);

			float doteA = glm::dot(eA - pA, planenormal_p);
			float doteB = glm::dot(eB - pA, planenormal_p);

			float eFract = -doteA / (doteB - doteA);
			

			if (eFract < 0 || eFract > 1.0)
			{
				continue; //This must miss!
			}

			float dotpA = glm::dot(pA - eA, planenormal_e);
			float dotpB = glm::dot(pB - eA, planenormal_e);

			float pFract = -dotpA / (dotpB - dotpA);

			if (pFract < 0 || pFract > 1.0)
			{
				continue; //This must miss!
			}

			//float eLength = 1.0; // glm::length(planenormal_e);
			//float pLength = 1.0; // glm::length(planenormal_p);

			glm::vec3 pointA = eA + (eB - eA) * eFract;
			glm::vec3 pointB = pA + (pB - pA) * pFract;
			glm::vec3 pointdelta = pointA - pointB;

			float tempt = glm::dot(pointdelta, delta) / glm::dot(delta, delta);


			//float tempt = glm::length(pointdelta) / glm::length(delta);
			if (tempt < loct && tempt > 0.0f)
			{
				loct = tempt;
				locnormal = glm::normalize(glm::cross(eB - eA,pB - pA));
			}
		}
		if (loct < t)
		{
			normal = locnormal;
			t = loct;
			return true;
		}
		else
			return false;
	}
	bool traceAgainstVert(glm::vec3& vert, glm::vec3& position, glm::vec3& normal, glm::vec3& delta, float& t)
	{
		glm::vec3 pA = position - glm::vec3(width / 2, width / 2, 1.6f);
		glm::vec3 pB = position + glm::vec3(width / 2, width / 2, 0.2f);

		glm::bvec3 isinside = glm::bvec3(glm::sign( (vert - pA))) && glm::bvec3(glm::sign((pB - vert)));

		bool outsideTrace = glm::all(glm::bvec3(glm::sign((vert - tA))) && glm::bvec3(glm::sign((tB - vert))));
		if(outsideTrace)
			return false;


		std::numeric_limits<float>::infinity();
		/*glm::vec3 relApproachRate =  glm::vec3(
			isinside.x ? std::numeric_limits<float>::infinity() : ;
*/

		return true;
	}
	void updateTraceAABB(glm::vec3 &position, glm::vec3 delta)
	{
		glm::vec3 pA = position - glm::vec3(width / 2, width / 2, 1.6f);
		glm::vec3 pB = pA + glm::vec3(width, width, height);

		tA = pA, tB = pB;

		//this gets us a trace AABB, which is essentially the AABB encompassing start and end point player AABBs
		tA = glm::min(tA + delta, tA);
		tB = glm::max(tB + delta, tB);
	}

	bool AABBintersect(glm::vec3 a1, glm::vec3 a2)
	{
		glm::bvec3 separation = glm::bvec3(glm::max((a1 - tB), glm::vec3(0))) || glm::bvec3(glm::max((tA - a2), glm::vec3(0)));

		return !glm::max(glm::max(separation.x, separation.y), separation.z);
	}
};

