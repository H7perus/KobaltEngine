#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <chrono>
#include "coretypes/Time/KTimestamp.h"
#include "coretypes/Input/KInputSet.h"

#include "glm/glm.hpp"
#include "KPlayerAABB.h"
#include "KBvhNode.h"

class KGameState
{
public:
	KDuration nextTickTime;

	//hack for testing
	KInputSet inputState;
	float yaw = 0, pitch = 0;
	float savedyaw = 0, savedpitch = 0;

	glm::vec3 savedPos = glm::vec3(0);

	glm::vec3 velocity = glm::vec3(0);
	glm::vec3 pos = glm::vec3(0);
	glm::vec3 oldpos = glm::vec3(0); 
	bool noclip = true;
	bool isonground = false;

	float tickrate = 32;

	std::vector<glm::vec3> verts; 
	std::vector<uint32_t> vertindices;

	std::vector<KCollideTriangle> tris;

	uint32_t topNodeCount;
	std::vector<std::pair<uint32_t, uint32_t>> BvhLayers;
	std::deque<KBvhNode> BvhNodes;


	void StepAhead(float frametime)
	{
		float gravity = 20;
		float jumpimpulse = sqrt(2 * gravity * (57 * 0.025));
		float airaccel = 100;
		float airmaxspeed = 0.75;
		float accel = 5.5;
		float friction = 5.2;
		float maxspeed = 6.25;
		float minfrictionspeed = 2.0;
		float minspeed = 0.01;

		glm::vec3 viewDir = glm::vec3(sin(glm::radians(yaw)) * cos(glm::radians(pitch)), cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)));

		viewDir = glm::normalize(viewDir);

		oldpos = pos;

		glm::vec3 movedelta = glm::vec3(0, 0, -0.05);

		KPlayerAABB aabb;

		//CATEGORIZE POSITION;
		float cat_t = 1.0;
		glm::vec3 irrelevant_hitnormal;

		isonground = false;

		if (!noclip)
		{
			isonground = aabb.traceAgainstWorld(tris, pos, irrelevant_hitnormal, movedelta, cat_t);
		}
		
		//TEST FOR BHOP NERF
		if (isonground)
		{
			
		}

		float lengthfract = (cat_t * glm::length(movedelta) - 0.01f > 0) ? (cat_t * glm::length(movedelta) - 0.01f) / glm::length(movedelta) / cat_t : 0.0f;
		pos += movedelta * cat_t * lengthfract * float(!noclip) * float(isonground);


		movedelta = glm::vec3(0);

		if (noclip)
		{
			velocity = glm::vec3(0);
			if (inputState.buttonstates[SDL_SCANCODE_W]) { velocity += viewDir * 10.f; }
			if (inputState.buttonstates[SDL_SCANCODE_A]) { velocity -= glm::normalize(glm::cross(viewDir, glm::vec3(0, 0, 1))) * 10.f; }
			if (inputState.buttonstates[SDL_SCANCODE_S]) { velocity -= viewDir * 10.f; }
			if (inputState.buttonstates[SDL_SCANCODE_D]) { velocity += glm::normalize(glm::cross(viewDir, glm::vec3(0, 0, 1))) * 10.f; }
			movedelta = velocity / tickrate;
		}
		else
		{
			

			float forwardback = inputState.buttonstates[SDL_SCANCODE_W] - inputState.buttonstates[SDL_SCANCODE_S];
			float rightleft = inputState.buttonstates[SDL_SCANCODE_D] - inputState.buttonstates[SDL_SCANCODE_A];

			glm::vec3 wishdir = glm::vec3(sin(glm::radians(yaw)), cos(glm::radians(yaw)), 0) * forwardback + glm::vec3(cos(glm::radians(yaw)), -sin(glm::radians(yaw)), 0) * rightleft;

			if (isonground)
			{
				//JUMP
				bool jumping = inputState.buttonstates[SDL_SCANCODE_SPACE];
				velocity.z += jumpimpulse * jumping;
				isonground = !jumping;
			}
			if (isonground)
			{
				float hor_vel = glm::length(glm::vec2(velocity));

				if (hor_vel < minfrictionspeed * friction / tickrate)
				{
					velocity.x = 0; velocity.y = 0;
				}
				else
				{
					velocity -= glm::vec3(normalize(glm::vec2(velocity)), 0) * friction / tickrate * glm::max(minfrictionspeed, hor_vel);
				}

				velocity.x += wishdir.x * maxspeed * accel / tickrate;
				velocity.y += wishdir.y * maxspeed * accel / tickrate;
				float horizontal_vel = glm::length(glm::vec2(velocity));
				if (horizontal_vel > maxspeed)
				{
					velocity *= maxspeed / horizontal_vel;
				}
			}
			else
			{
				float currentspeed = dot(wishdir, glm::vec3(velocity.x, velocity.y, 0));

				//std::cout << "currentspeed: " << currentspeed << std::endl;
				//std::cout << "wishdir: " << wishdir.x << " " << wishdir.y << " " << wishdir.z << std::endl;
				//std::cout << "vel: " << velocity.x << " " << velocity.y << " " << velocity.z << std::endl;

				float addspeed = airmaxspeed - currentspeed;

				if (addspeed > 0)
				{
					float accelspeed = airaccel * airmaxspeed / tickrate;

					accelspeed = glm::min(accelspeed, addspeed);



					velocity.x += wishdir.x * accelspeed;
					velocity.y += wishdir.y * accelspeed;
				}
			}

			//APPLY HALF GRAVITY
			velocity.z -= 0.5 / tickrate * gravity;
			movedelta = velocity / tickrate;
			velocity.z -= 0.5 / tickrate * gravity;
		}
		
		
		
		glm::vec3 hitnormal = glm::vec3(0);

		float fraction_left = 1.0;

		std::chrono::microseconds cumulativecost = std::chrono::microseconds(0);

		for (int bumpcount = 0; bumpcount < 4; bumpcount++)
		{
			float t = 1.0;
			if(!noclip)
			{
				auto time = std::chrono::steady_clock::now();
				//std::vector<KBvhNode> tris = GetAllBottomNodes()
				aabb.traceAgainstWorld(tris, pos, hitnormal, movedelta* fraction_left, t);
				cumulativecost += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - time);
			}

			float lengthfract = (t * glm::length(movedelta) - 0.01f > 0) ? (t * glm::length(movedelta) - 0.01f) / glm::length(movedelta) / t : 0.0f;

			pos += movedelta * (t * lengthfract * fraction_left);

			fraction_left -= t * fraction_left;

			movedelta -= hitnormal * dot(movedelta, hitnormal);

			velocity -= hitnormal * dot(velocity, hitnormal) * float(t < 1.0);
			
			if (isnan(velocity.z))
			{
				velocity.z = 0;
			}

			movedelta *= glm::bvec3(glm::max(glm::abs(glm::vec3(movedelta)) - glm::vec3(1e-5), glm::vec3(0)));

			if (fraction_left == 0) 
				break;
		}

		std::cout << "traces took " << cumulativecost.count() / 1000.f << "ms" << std::endl;
	}

	void BuildCollideBvh()
	{
		//so, we have to create a binary BVH-tree...
		//we need to figure out how to efficiently group a set of AABBs into half as many BBs while keeping volume minimal
		//if this was for raytracing, we would probably rather optimize for surface area to minimize intersections, i.e. filter out more efficiently
		//but in this case we are basically checking whether they intersect with another AABB, so minimizing volume it is

		std::deque<KBvhNode> bottomNodes;

		for (KCollideTriangle& tri : tris)
		{
			KBvhNode bNode;
			bNode.bb.a = tri.AABBA;
			bNode.bb.b = tri.AABBB;
			bNode.numTris = 1;
			bNode.tris[0] = &tri;

			bottomNodes.push_back(bNode);
		}
		std::deque<KBvhNode> nodes = bottomNodes;
		int prev_size = bottomNodes.size();
		BvhLayers.push_back(std::pair(0, nodes.size()));
		for (int layer = 0; layer < 4; layer++)
		{
			int offset = nodes.size() - prev_size;
			int next_size = 0;
			for (int i = 0; i < prev_size; i+=2)
			{
				KBvhNode tNode;
				tNode.bb.a = glm::min(nodes[offset + i].bb.a, nodes[offset + i + 1].bb.a);
				tNode.bb.b = glm::max(nodes[offset + i].bb.b, nodes[offset + i + 1].bb.b);
				tNode.children[0] = &nodes[offset + i];
				if (i + 1 < bottomNodes.size())
					tNode.children[1] = &nodes[offset + i + 1];
				nodes.push_back(tNode);
				next_size++;
			}
			prev_size = next_size;
			BvhLayers.push_back(std::pair(nodes.size() - next_size, next_size));
		}
		BvhNodes = nodes;

	}

	std::vector<KBvhNode*> GetAllBottomNodes(KAABB bb)
	{
		std::vector<KBvhNode*> ret;
		for (int i = topNodeCount;i; --i)
		{
			if (glm::all(glm::greaterThan(BvhNodes[i].bb.b, bb.a) && glm::lessThan(BvhNodes[i].bb.a, bb.b)))
			{
				for (int j = 0; j < 2; j++)
				{
					if (glm::all(glm::greaterThan(BvhNodes[i].children[j]->bb.b, bb.a) && glm::lessThan(BvhNodes[i].children[j]->bb.a, bb.b)))
					{
						ret.push_back(BvhNodes[i].children[j]);
					}
				}
			}

		}
		return ret;
	}
};

