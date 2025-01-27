#include "VertexBufferGroup.h"

VertexBufferGroup::VertexBufferGroup(std::vector<std::pair<std::string, uint32_t>>& vInfo) : VertexAttributes(vInfo)
{
}
void VertexBufferGroup::CreateVAO()
{
	glGenVertexArrays(1, &VAOID);

}
void VertexBufferGroup::SetVAOAttributes()
{
	/*glBindVertexArray(VAOID);

	GLuint stride = 0;
	std::vector<unsigned int> cumul_sum(VertexAttributes.size());
	unsigned int index = 0;
	for (std::pair<std::string, uint32_t>& attribute : VertexAttributes)
	{
		cumul_sum[index] = stride;
		stride += attribute.second;

		index++;
	}
	index = 0;

	for (std::pair<std::string, uint32_t>& attribute : VertexAttributes)
	{
		glVertexAttribPointer(index, attribute.second, GL_FLOAT, GL_FALSE, stride, (void*)0);
		index++;
	}*/


	VBO.Bind();
	EBO.Bind();

	glGenVertexArrays(1, &VAOID);
	glBindVertexArray(VAOID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

}

void VertexBufferGroup::Bind()
{
	VBO.Bind();
	EBO.Bind();
	glBindVertexArray(VAOID);
}

