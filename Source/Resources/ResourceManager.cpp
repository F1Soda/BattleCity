#include "ResourceManager.h"
#include "../Renderer/ShaderProgram.h"
#include <iostream>
#include <sstream>
#include <fstream>

ResourceManager::ResourceManager(const std::string& executablePath)
{
	size_t found = executablePath.find_last_of("/\\");
	m_path = executablePath.substr(0, found);
}
std::string ResourceManager::getFileString(const std::string& relativeFilePath) const
{
	std::fstream f;
	f.open(m_path + "/" + relativeFilePath.c_str(), std::ios::in, std::ios::binary);
	if (!f.is_open())
	{
		std::cerr << "Failed to open file:" << "relativeFilePath" << std::endl;
		return std::string();
	}
	std::stringstream buffer;
	buffer << f.rdbuf();
	return buffer.str();
}

std::shared_ptr<Renderer::ShaderProgram> ResourceManager::loadShaders
(const std::string & shaderName, const std::string & vertexPath, 
	const std::string & fragmentPath)
{
	std::string vertexString = getFileString(vertexPath);
	if (vertexString.empty())
	{
		std::cerr << "No vertex shader! " << std::endl;
		return nullptr;
	}

	std::string fragmentString = getFileString(fragmentPath);
	if (fragmentString.empty())
	{
		std::cerr << "No fragment shader! " << std::endl;
		return nullptr;
	}
	std::shared_ptr<Renderer::ShaderProgram> temp_ptr
	(new Renderer::ShaderProgram(vertexString, fragmentString));

	std::shared_ptr<Renderer::ShaderProgram>& newShader = 
		m_shaderPrograms.emplace(shaderName, 
		temp_ptr).first->second;

	if (newShader->isCompiled())
	{
		return newShader;
	}
	else
	{
		std::cerr << "Can't load shader program:\n"
			"Vertex:" << vertexPath << std::endl
			<<"Fragment: " << fragmentPath << std::endl;
		return nullptr;
	}

}

std::shared_ptr<Renderer::ShaderProgram> ResourceManager::getSHaderProgram(const std::string& shaderName)
{
	ShaderProgramsMap::const_iterator it = m_shaderPrograms.find(shaderName);
	if (it != m_shaderPrograms.end())
		return it->second;
	std::cerr << "Can't find the shader program: " << shaderName << std::endl;
	return nullptr;
}