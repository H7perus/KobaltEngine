#pragma once
#include <iostream>
#include "GL/glew.h"
#include "glm/glm.hpp"

#include "../rendererdll.h"

class RENDERER_DLL_API GLTexture
{
public:
	unsigned int ID = 0;
	GLuint64 handle; //all our textures are bindless

	int target;  //do we give a shit about this? ever? add: yes, yes we do. Already for MS buffers
	int width, height;
	int msaa_samples;  //1 = not multisampled
	int internalformat;  //sized or not sized, that is the question.

	std::string sourcepath;

	GLTexture& operator=(GLTexture&& other) noexcept;

	GLTexture() {}

	~GLTexture();

	//dear god please keep the parameter names for the constructor when intellisensing that shit(Addendum: this code is copied from another project where this is a module)
	GLTexture(int target, int internalformat, int width, int height, int format, int type, int filter, int wrap, void* data); //data shall be NULL if not required

	void resize(int width, int height); //will wipe whatever is in it! primarily for use in framebuffers.

	void bind();
	//important for bindless textures
	void MakeResident(); 
	//only important if freeing some memory without deleting textures is important
	void MakeNonResident();
};

