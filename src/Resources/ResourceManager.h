#pragma once
#include <string>
#include <memory>
#include <map>
#include <vector>
#include "../Game/GameObjects/Tank.h"

namespace RenderEngine
{
	class ShaderProgram; // нам не обязательно включать файл в заголовночм. Можно это сделать в срр файле
	class Texture2D;
	class Sprite;
}

namespace Resources
{
	struct descriptionLevel
	{
		descriptionLevel(std::vector<std::string>, std::unordered_map<Tank::ETankType, int>);

		std::vector<std::string> mapStructure;
		std::unordered_map<Tank::ETankType, int> enemiesTypeMaps;
	};

	class ResourceManager
	{
	public:
		static void setExecutablePath(const std::string& executablePath);
		static void unloadAllResources();

		~ResourceManager() = delete; // конструктор по умолчанию
		ResourceManager() = delete;

		ResourceManager(const ResourceManager&) = delete; // запрет на конструктор копирования
		ResourceManager& operator=(const ResourceManager&) = delete;
		ResourceManager& operator=(ResourceManager&&) = delete;
		ResourceManager(ResourceManager&&) = delete;

		static std::shared_ptr<RenderEngine::ShaderProgram>loadShaders
		(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath);

		static std::shared_ptr<RenderEngine::ShaderProgram> getShaderProgram(const std::string& shaderName);
		static std::shared_ptr<RenderEngine::Texture2D> loadTexture(const std::string& textureName, const std::string& texturePath);
		static std::shared_ptr<RenderEngine::Texture2D> getTexture(const std::string& textureName);
		static std::shared_ptr<RenderEngine::Sprite> loadSprite(const std::string& spriteName,
			const std::string& textureName,
			const std::string& shaderName,
			const std::string& subTextureName = "default");

		static std::shared_ptr<RenderEngine::Sprite> getSprite(const std::string& spriteName);


		static std::shared_ptr<RenderEngine::Texture2D> loadTextureAtlas(std::string textureName,
			std::string texturePath,
			std::vector<std::string> subTextures,
			const unsigned int subTextureWidth,
			const unsigned int subTextureHeight);

		static bool loadJSONResources(const std::string& JSONPath);

		static const std::vector<descriptionLevel>* getLevels() { return &m_levels; }
		static const std::vector<std::string>& getStartScreen() { return m_startScreen; }
		static const std::vector<std::string>& getGameOver() { return m_gameOver; }
		static const size_t getCountLevels() { return m_levels.size(); }

	private:
		static std::string getFileString(const std::string& ralativeFilePath);

		typedef std::map<const std::string, std::shared_ptr<RenderEngine::ShaderProgram>> ShaderProgramsMap;
		static ShaderProgramsMap m_shaderPrograms;

		typedef std::map<const std::string, std::shared_ptr<RenderEngine::Texture2D>> TexturesMap;
		static TexturesMap m_textures;

		typedef std::map<const std::string, std::shared_ptr<RenderEngine::Sprite>> SpritesMap;
		static SpritesMap m_sprites;


		static std::vector<descriptionLevel> m_levels;
		static std::vector<std::string> m_startScreen;
		static std::vector<std::string> m_gameOver;

		static std::string m_path;
	};
}

