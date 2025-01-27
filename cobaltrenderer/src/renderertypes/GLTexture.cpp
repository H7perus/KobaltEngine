#include "GLTexture.h"
GLTexture& GLTexture::operator=(GLTexture&& other) noexcept
{
	if (this != &other)
	{
		// Release any resources currently held by this object
		if (ID)
		{
			glDeleteTextures(1, &ID);
		}

		// Transfer ownership of the resources of the new object
		ID = other.ID;
		target = other.target;
		width = other.width;
		height = other.height;
		msaa_samples = other.msaa_samples;
		internalformat = other.internalformat;

		// Invalidate the moved-from object
		other.ID = 0;
	}
	return *this;
}

GLTexture::~GLTexture()
{
	glDeleteTextures(1, &ID);
}

GLTexture::GLTexture(int target, int internalformat, int width, int height, int format, int type, int filter, int wrap, void* data) : target(target), internalformat(internalformat), width(width), height(height) //data shall be NULL if not required
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //see where you can move this to.
	glGenTextures(1, &ID); 
	glBindTexture(target, ID);
	
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
	//glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);

	glTexImage2D(target, 0, internalformat, width, height, 0, format, type, data);
	glGenerateMipmap(target);
	handle = glGetTextureHandleARB(ID);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "OpenGL Error on Tex creation?: " << err << std::endl;
	}

}

void GLTexture::resize(int width, int height) //will wipe whatever is in it! primarily for use with framebuffers.
{
	if (ID)
	{
		bind();
		glTexImage2D(target, 0, internalformat, width, height, 0, internalformat, GL_FLOAT, 0);
		this->width = width;
		this->height = height;
	}
}

void GLTexture::bind()
{
	glBindTexture(target, ID);
}

void GLTexture::MakeResident()
{
	glMakeTextureHandleResidentARB(handle);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "OpenGL Error in Texture?: " << err << std::endl;
	}
}

void GLTexture::MakeNonResident()
{
	glMakeTextureHandleNonResidentARB(handle);
}