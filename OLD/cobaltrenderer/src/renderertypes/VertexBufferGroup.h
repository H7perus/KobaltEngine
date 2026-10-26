#pragma once

#include "GL/glew.h"
#include "GLBuffer.h"
#include "DrawInfo.h"
#include <vector>

class VertexBufferGroup
{
public:
	GLBuffer VBO		=		GLBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	GLBuffer EBO		=		GLBuffer(GL_ELEMENT_ARRAY_BUFFER,  GL_STATIC_DRAW  );
	GLuint VAOID;
	//Objects will get pointers to the index locators, that way we can rearrange buffers and have objects easily know.
	std::vector<VertLocator> VertexInfo;

	std::vector<std::pair<std::string, uint32_t>> VertexAttributes;

	VertexBufferGroup(std::vector<std::pair<std::string, uint32_t>>& vInfo);
	void CreateVAO();
	void SetVAOAttributes();
	void Bind(); //binds VAO
};

