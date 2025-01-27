#pragma once

#include <iostream>
#include "GL/glew.h"

#include "../rendererdll.h"

class GLBuffer
{
public:
	GLuint ID = 0;
	int main_use = 0;
	int usage_hint = 0; //GL_DYNAMIC_DRAW or GL_STATIC_DRAW
	uint64_t allocated_size = 0; //if zero, unallocated! resizing will require resubmitting all data!
	uint64_t used_size = 0;
	GLBuffer& operator=(GLBuffer&& other) noexcept
	{
		if (this != &other)
		{
			// Release any resources currently held by this object
			if (ID)
			{
				glDeleteBuffers(1, &ID);
			}

			// Transfer ownership of the resources of the new object
			ID = other.ID;
			main_use = other.main_use;

			// Invalidate the moved-from object
			other.ID = 0;
		}
		return *this;
	}

	GLBuffer(GLBuffer&& other) noexcept {
		ID = other.ID;
		main_use = other.main_use;
		usage_hint = other.main_use;
		allocated_size = other.allocated_size;
		used_size = other.used_size;
	}

	~GLBuffer()
	{
		if(ID) glDeleteBuffers(1, &ID);
	}
	//SET GL_STATIC_DRAW IF IT RARELY CHANGES!
	GLBuffer(int main_use, int usage_hint = GL_DYNAMIC_DRAW) : main_use(main_use), usage_hint(usage_hint)
	{
	}

	GLBuffer(int main_use, uint64_t size, void* data, int usage_hint = GL_DYNAMIC_DRAW) : main_use(main_use), usage_hint(usage_hint)
	{
		glCreateBuffers(1, &ID);
		//Bind();
		glNamedBufferStorage(ID, size, data, GL_DYNAMIC_STORAGE_BIT);
	}
	//Careful! If it is already allocated, it will wipe the buffer completely and create a new one of the specified size/data. Supply NULL if you only want to allocate.
	void Allocate(uint64_t size, void* data)
	{
		Delete();
		glCreateBuffers(1, &ID);
		glNamedBufferStorage(ID, size, data, GL_DYNAMIC_STORAGE_BIT);

		if (data == NULL)
			used_size = 0;
		else
			used_size = size;

		allocated_size = size;
	}

	void Data(uint64_t size, void* data)
	{
		if (allocated_size < size)
		{
			Allocate(size, data);
			used_size = size;
		}
		else
		{
			glNamedBufferSubData(ID, 0, size, data);
			used_size = size;
		}
	}

	bool DataAtEnd(uint64_t size, void* data)
	{
		if (allocated_size < used_size + size)
			return false;
		glNamedBufferSubData(ID, used_size, size, data);
		std::cout << "ERROR ON DATAATEND" << glGetError() << std::endl;
		used_size += size;
	}
	//bind with main use.
	void Bind()
	{
		glBindBuffer(main_use, ID);
	}
	//bind to base as SSBO.
	void BindToBaseSSBO(int slot)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, ID);
	}
	//bind to base as UBO.
	void BindToBaseUBO(int slot)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, slot, ID);
	}

	//mainly used inside this class, used when you don't necessarily want to get rid of the entire object.
	void Delete()
	{
		if (ID) glDeleteBuffers(1, &ID);
	}
};

