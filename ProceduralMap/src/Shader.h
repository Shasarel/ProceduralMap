#pragma once
#include <string>
#include <map>
#include <glm/glm.hpp>

class Shader 
{
public:
	Shader(const std::string& vertexShader, const std::string& fragmentShader);
	~Shader();

	void use();

	void setUniformValue(const std::string& uniformName, const int value);
	void setUniformValue(const std::string& uniformName, const float value);
	void setUniformValue(const std::string& uniformName, const glm::mat4& value);
	void setUniformValue(const std::string& uniformName, const glm::vec3& value);
	void setUniformValue(const std::string uniformName, const glm::vec2& value);

private:
	std::map<std::string, unsigned int> uniforms;
	unsigned int programId;

	unsigned int compileShader(const std::string& shaderName, const unsigned int shaderType);
	void loadUniforms();
	int getUniformLocation(const std::string& uniformName);

};