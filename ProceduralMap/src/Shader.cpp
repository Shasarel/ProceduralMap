#include "Shader.h"
#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include "Logger.h"

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader):
	programId(0)
{
	Logger::printInfo("Creating shader");
	unsigned int vertexShaderId = compileShader(vertexShader, GL_VERTEX_SHADER);
	unsigned int fragmentShaderId = compileShader(fragmentShader, GL_FRAGMENT_SHADER);

	if (vertexShaderId != 0 && fragmentShaderId != 0) {

		Logger::printInfo("Linking program");
		programId = glCreateProgram();
		glAttachShader(programId, vertexShaderId);
		glAttachShader(programId, fragmentShaderId);
		glLinkProgram(programId);

		int infoLogLength;

		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			char* errorMessage = new char[infoLogLength];
			glGetProgramInfoLog(programId, infoLogLength, NULL, errorMessage);
			Logger::printError(errorMessage);
			delete[] errorMessage;
			glDeleteProgram(programId);
			programId = 0;
		}
		else
		{
			loadUniforms();
		}

		glDetachShader(programId, vertexShaderId);
		glDetachShader(programId, fragmentShaderId);

		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);
	}
}

Shader::~Shader()
{
	if (programId != 0)
		glDeleteProgram(programId);
}

void Shader::use()
{
	glUseProgram(programId);
}

void Shader::setUniformValue(const std::string& uniformName, const int value)
{
	int location = getUniformLocation(uniformName);
	if (location != -1) {
		glUniform1i(location, value);
	}
}

void Shader::setUniformValue(const std::string& uniformName, const float value)
{
	int location = getUniformLocation(uniformName);
	if (location != -1) {
		glUniform1f(location, value);
	}
}

void Shader::setUniformValue(const std::string& uniformName, const glm::mat4& value)
{
	int location = getUniformLocation(uniformName);
	if (location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	}
}

void Shader::setUniformValue(const std::string& uniformName, const glm::vec3& value)
{
	int location = getUniformLocation(uniformName);
	if (location != -1) {
		glUniform3fv(location, 1, &value[0]);
	}
}

void Shader::setUniformValue(const std::string uniformName, const glm::vec2 & value)
{
	int location = getUniformLocation(uniformName);
	if (location != -1) {
		glUniform2fv(location, 1, &value[0]);
	}
}

unsigned int Shader::compileShader(const std::string& shaderName, const unsigned int shaderType)
{
	std::string shaderCode;
	std::ifstream shaderStream(shaderName, std::ios::in);
	if (shaderStream.is_open()) {
		std::string Line = "";
		while (getline(shaderStream, Line))
			shaderCode += "\n" + Line;
		shaderStream.close();
	}
	else {
		Logger::printError("Impossible to open" + shaderName);
		return 0;
	}

	int infoLogLength;

	unsigned int shaderId = glCreateShader(shaderType);

	Logger::printInfo("Compiling shader :" + shaderName);
	const char* temp = shaderCode.c_str();
	glShaderSource(shaderId, 1, &temp , NULL);
	glCompileShader(shaderId);

	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		char* errorMessage = new char[infoLogLength];
		glGetShaderInfoLog(shaderId, infoLogLength, NULL, errorMessage);
		printf("%s\n", errorMessage);
		Logger::printError(errorMessage);
		delete[] errorMessage;
		glDeleteShader(shaderId);
		return 0;
	}

	return shaderId;
}

void Shader::loadUniforms()
{
	int numUniforms;
	glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &numUniforms);

	char buffer[128];
	for (int i = 0; i < numUniforms; i++)
	{
		GLenum type;
		GLint size;
		glGetActiveUniform(programId, i, sizeof(buffer), 0, &size, &type, buffer);

		std::string name = buffer;

		unsigned int location = glGetUniformLocation(programId, buffer);
		uniforms[name] = location;
	}
}

int Shader::getUniformLocation(const std::string& uniformName)
{
	auto& result = uniforms.find(uniformName);

	if (result != uniforms.end()) {
		return result->second;
	}
	Logger::printWarning("Missing uniform:" + uniformName);
	return -1;
}
