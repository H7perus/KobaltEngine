#pragma once
#include <string>
#include <typeindex>
#include <mutex>

#include "DrawInfo.h"
#include "../rendererdll.h"

inline float vertices[18]{
-1.0, 1.0, 0.0,
-1.0, -0.2, 0.0,
0.0, -1.0, 0.0,
1.0, 1.0, 0.0,
0.0, -1.0, 0.0,
1.0, -0.2, 0.0
};

inline unsigned int indices[6]
{
    0, 1, 2, 0, 1, 2
};

//we use this to request an object to be created. It sends a unique identifier that the render group will keep
struct RenderObjectRequest
{
	std::string name;
	void* vertex_data;
	uint64_t vertex_size;
	void* index_data;
	uint64_t num_indices;

	std::type_index data_type{ typeid(void) };
	void* data_pointer;

	RenderObjectRequest() {}
	RenderObjectRequest(std::string name, uint64_t vertex_size, void* vertex_data, uint64_t num_indices, void* index_data, std::type_index data_type, uint64_t data_size, void* data_pointer) :
		name(name),
		vertex_size(vertex_size),
		vertex_data(vertex_data),
		num_indices(num_indices),
		index_data(index_data),
		data_type(data_type),
		data_pointer(data_pointer)
	{}
};


//baseobject for renderobject. Should hold state information about a renderable object. Is it loaded? How do we load it?
class RENDERER_DLL_API RenderObject
{

public:
	std::string name;

	SingleDraw DrawInfo;

	std::mutex ObjectAccess; //block during move for example, when group target has to update.

	bool loaded = false;
	//up to date info of what group this objects ressources are allocated in. Gets updated whenever RGs change

	//offset for indices, first index in index buffer, number of indices to draw.
	//usually material, used to group items of identical material together to save on drawcalls
	std::type_index data_type;
	uint64_t data_index;

	RenderObject(std::type_index data_type);

	void UpdateLocation(std::pair<uint64_t, uint64_t> new_location);

	virtual void Load(uint64_t& num_vertices, void* vertices_data, void* indices_data);
};

class GLTFRenderObject : RenderObject
{
	std::string gltfPath;
	std::string model;


	void Load(uint64_t& num_vertices, void* vertices_data, void* indices_data);
};

