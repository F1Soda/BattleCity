#pragma once
#include <string>
#include <memory>
#include <map>

namespace Renderer
{
	class ShaderProgram; // нам не обязательно включать файл в заголовночм. Можно это сделать в срр файле
}
class ResourceManager
{
public:
	ResourceManager(const std::string& executablePath);
	~ResourceManager() = default; // конструктор по умолчанию

	ResourceManager(const ResourceManager&) = delete; // запрет на конструктор копирования
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager& operator=(ResourceManager&&) = delete;
	ResourceManager(ResourceManager&&) = delete;

	std::shared_ptr<Renderer::ShaderProgram>loadShaders(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath);
	std::shared_ptr<Renderer::ShaderProgram> getSHaderProgram(const std::string& shaderName);

private:
	std::string getFileString(const std::string& ralativeFilePath) const;

	typedef std::map<const std::string, std::shared_ptr<Renderer::ShaderProgram>> ShaderProgramsMap;
	ShaderProgramsMap m_shaderPrograms;
	
	std::string m_path;



};