#pragma once
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace Renderer
{
	class ShaderProgram; // нам не обязательно включать файл в заголовночм. Можно это сделать в срр файле
	class Texture2D;
	class Sprite;
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

	std::shared_ptr<Renderer::ShaderProgram>loadShaders
	(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath);

	std::shared_ptr<Renderer::ShaderProgram> getShaderProgram(const std::string& shaderName);
	std::shared_ptr<Renderer::Texture2D> loadTexture(const std::string& textureName, const std::string& texturePath);
	std::shared_ptr<Renderer::Texture2D> getTexture(const std::string& textureName);
	std::shared_ptr<Renderer::Sprite> loadSprite(const std::string& spriteName, 
											    const std::string& textureName,
												const std::string& shaderName,
												const unsigned int spriteWidth,
												const unsigned int spriteHeight,
												const std::string& subTextureName = "default");

	std::shared_ptr<Renderer::Sprite> getSprite(const std::string& spriteName);

	std::shared_ptr<Renderer::Texture2D> loadTextureAtlas(const std::string textureName,
														  const std::string texturePath,
														  std::vector<std::string> subTextures,
														  const unsigned int subTextureWidth, 
														  const unsigned int subTextureHeight);
private:
	std::string getFileString(const std::string& ralativeFilePath) const;

	typedef std::map<const std::string, std::shared_ptr<Renderer::ShaderProgram>> ShaderProgramsMap;
	ShaderProgramsMap m_shaderPrograms;

	typedef std::map<const std::string, std::shared_ptr<Renderer::Texture2D>> TexturesMap;
	TexturesMap m_textures;
	
	typedef std::map<const std::string, std::shared_ptr<Renderer::Sprite>> SpritesMap;
	SpritesMap m_sprites;

	std::string m_path;



};