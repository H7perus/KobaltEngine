#pragma once
#include "glm/glm.hpp"
#include <memory>
#include <vector>


template <typename VertexType>
struct VertexData
{
	std::vector<VertexType> vdata;
	VertexData(){}

	virtual void* GetData()
	{
		return vdata.data();
	}
	virtual uint64_t GetBytelength()
	{
		return vdata.size() * sizeof(vdata[0]);
	}

	unsigned int GetVAO()
	{
		//TODO:
	}
};

struct VertexSimple
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 UV;
};