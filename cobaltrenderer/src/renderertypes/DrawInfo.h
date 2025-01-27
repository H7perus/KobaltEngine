#pragma once
#include <utility>
#include <vector>

struct DrawElementsIndirectCommand {
	unsigned int  count;
	unsigned int  instanceCount;
	unsigned int  firstIndex;
	int  baseVertex;
	unsigned int  baseInstance;
	unsigned int materialID;
	unsigned int modelmatID;
};


struct VertLocator {

	//offset into the EBO and number of indices to draw, should be divisible by 3
	uint64_t first_index;
	uint64_t count;

	//offset for the indices and actual number of vertices stored. Due to reuse this number is different than the indices.
	uint64_t first_vert; //specifies offset, very important for indirect draw
	uint64_t num_verts;
};

struct PrimitiveDataBlock 
{
	uint64_t indexcount;
	uint64_t vertcount;
	void* indexdata;
	void* vertdata;

	std::vector < std::pair<std::string, uint32_t> > vertformat;

	uint64_t materialindex;


};

struct PrimitiveDataList
{
	std::vector<PrimitiveDataBlock> datalist;

};



//just for clarity, *MaterialLocator has more ring to it than *uint32_t
struct MaterialLocator {
	uint32_t index;
};

struct SingleDraw
{
	//std::pair<uint64_t, uint64_t> target_group; //tf outta here
	VertLocator VertLocator;
	uint32_t materialindex; //in the material buffer
	uint32_t modelmatindex;

	DrawElementsIndirectCommand GetCommand()
	{
		DrawElementsIndirectCommand cmd;
		cmd.count = VertLocator.count;
		cmd.instanceCount = 1;
		cmd.firstIndex = VertLocator.first_index;
		cmd.baseVertex = VertLocator.first_vert;
		cmd.baseInstance = 1;
		cmd.materialID = materialindex;
		cmd.modelmatID = modelmatindex;
		return cmd;
	}
};

struct DrawInfoGroup
{
	std::vector<SingleDraw> DrawActions;
};