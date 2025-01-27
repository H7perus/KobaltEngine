#pragma once


#include "GL/glew.h"
#include "GLTexture.h"
#include <utility>


class FrameBufferObject
{
public:
	unsigned int FBO_handle;
	unsigned int renderbuffer_handle;

	GLTexture color_buffer;
	GLTexture depth_buffer;

	FrameBufferObject(int color_iformat, int depth_iformat, int width, int height);

	void Bind();

	void resize(int width, int height);
};

