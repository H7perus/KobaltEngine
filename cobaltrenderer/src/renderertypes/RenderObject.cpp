#include "RenderObject.h"


RenderObject::RenderObject(std::type_index data_type) : data_type(data_type)
{

}

void RenderObject::UpdateLocation(std::pair<uint64_t, uint64_t> new_location)
{
	//DrawInfo.target_group = new_location;
}

void RenderObject::Load(uint64_t& num_vertices, void* vertices_data, void* indices_data)
{
	DrawInfo.VertLocator.num_verts = num_vertices;

	vertices_data = &vertices;
	indices_data = &indices;

}