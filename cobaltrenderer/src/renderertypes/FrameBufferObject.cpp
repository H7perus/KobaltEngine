#include "FrameBufferObject.h"

FrameBufferObject::FrameBufferObject(int color_iformat, int depth_iformat, int width, int height)
{
	glGenFramebuffers(1, &FBO_handle);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_handle);
	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	glDepthFunc(GL_GEQUAL);

	if (color_iformat)
		color_buffer = GLTexture(GL_TEXTURE_2D, color_iformat, width, height, 0, 0, GL_LINEAR, GL_CLAMP_TO_EDGE, 0);

	if (depth_iformat)
		depth_buffer = GLTexture(GL_TEXTURE_2D, depth_iformat, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_EDGE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer.handle, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_buffer.handle, 0);
}

void FrameBufferObject::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_handle);
}

void FrameBufferObject::resize(int width, int height)
{
	color_buffer.resize(width, height);
	depth_buffer.resize(width, height);
}