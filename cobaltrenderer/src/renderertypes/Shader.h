#pragma once

#include "GL/glew.h"

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <variant>
#include <any>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

inline std::string shader_path = "../engine/cobaltrenderer/src/";

using UniformValue = std::variant<int, float, glm::vec2, glm::dvec2, glm::vec3, glm::dvec3, glm::vec4, glm::dvec4, glm::mat3, glm::dmat3, glm::mat4, glm::dmat4>;

class Shader
{
public:
	//the program ID
	unsigned int ID;
	std::string vertexPath;
	std::string fragmentPath;


	std::unordered_map<std::string, UniformValue> static_uniforms;
	//std::unordered_map<std::string, std::any> uniforms;

	//constructor reads the shader from a file and builds it
	Shader() {}
	Shader(std::string ivertexPath, std::string ifragmentPath)
	{
		vertexPath = ivertexPath;
		fragmentPath = ifragmentPath;

		loadShader();
	}
	void reload()
	{

		loadShader();
		use();
		for (const auto& uniform : static_uniforms) //if you want to use std::any in the future, compare types with uniform.second.type() == typeid(type)
		{
			if (std::holds_alternative<int>(uniform.second))
				glUniform1i(glGetUniformLocation(ID, uniform.first.c_str()), std::get<int>(uniform.second));
			if (std::holds_alternative<float>(uniform.second))
				glUniform1f(glGetUniformLocation(ID, uniform.first.c_str()), std::get<float>(uniform.second));
			if (std::holds_alternative<glm::vec2>(uniform.second)) //perhaps implement dvec2 aswell?
				glUniform2fv(glGetUniformLocation(ID, uniform.first.c_str()), 1, &(std::get<glm::vec2>(uniform.second))[0]);
			if (std::holds_alternative<glm::vec3>(uniform.second))
				glUniform3fv(glGetUniformLocation(ID, uniform.first.c_str()), 1, &(std::get<glm::vec3>(uniform.second))[0]);
			if (std::holds_alternative<glm::dvec3>(uniform.second))
				glUniform3dv(glGetUniformLocation(ID, uniform.first.c_str()), 1, &(std::get<glm::dvec3>(uniform.second))[0]);
			if (std::holds_alternative<glm::mat3>(uniform.second))
				glUniformMatrix3fv(glGetUniformLocation(ID, uniform.first.c_str()), 1, GL_FALSE, &(std::get<glm::mat3>(uniform.second))[0][0]);
			if (std::holds_alternative<glm::mat4>(uniform.second))
				glUniformMatrix4fv(glGetUniformLocation(ID, uniform.first.c_str()), 1, GL_FALSE, &(std::get<glm::mat4>(uniform.second))[0][0]);
		}
	}
	void loadShader()
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);

			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		unsigned int vertex, fragment;
		int success;
		char infoLog[512];

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);

		glGetProgramiv(ID, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAMM::LINKING_FAILED" << infoLog << std::endl;

		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	//use the shader
	void use() const
	{
		glUseProgram(ID);
	}
	//utility uniform functions
	void setBool(const std::string& name, bool value)
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
		static_uniforms[name] = int(value);
	}
	void setInt(const std::string& name, int value)
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
		static_uniforms[name] = value;
	}
	void setFloat(const std::string& name, float value)
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
		static_uniforms[name] = value;
	}
	void setMat4(const std::string& name, glm::mat4 value)
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
		static_uniforms[name] = value;
	}
	void setMat3(const std::string& name, glm::mat3 value)
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
		static_uniforms[name] = value;
	}
	void setVec2(const std::string& name, const glm::vec2& value)
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
		static_uniforms[name] = value;
	}
	void setVec3(const std::string& name, const glm::vec3& value)
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
		static_uniforms[name] = value;
	}
	void setDVec3(const std::string& name, const glm::dvec3& value)
	{
		glUniform3dv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
		static_uniforms[name] = value;
	}
	static void CheckGLError()
	{
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cerr << "OpenGL error: " << err << std::endl;
		}
	}
};

#endif



