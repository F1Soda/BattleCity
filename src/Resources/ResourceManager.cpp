#include "ResourceManager.h"
#include "../Renderer/ShaderProgram.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "../Renderer/texture2D.h"
#include "../Renderer/Sprite.h"
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG // �������������� �������������� ������ png ������ 
#include "stb_image.h"
#include<rapidjson/document.h>
#include <rapidjson/error/en.h>


namespace Resources
{
	Resources::ResourceManager::ShaderProgramsMap ResourceManager::m_shaderPrograms;
	Resources::ResourceManager::TexturesMap ResourceManager::m_textures;
	Resources::ResourceManager::SpritesMap ResourceManager::m_sprites;
	std::vector<descriptionLevel> ResourceManager::m_levels;
	std::vector<std::string> ResourceManager::m_startScreen;
	std::vector<std::string> ResourceManager::m_gameOver;

	std::string ResourceManager::m_path;

	descriptionLevel::descriptionLevel(std::vector<std::string> _ms, std::vector<std::pair<Tank::ETankType, int>> _etm)
	{
		mapStructure = std::move(_ms);
		enemiesTypeMaps = std::move(_etm) ;
	}


	void ResourceManager::setExecutablePath(const std::string& executablePath)
	{
		size_t found = executablePath.find_last_of("/\\");
		m_path = executablePath.substr(0, found);
	}

	void ResourceManager::unloadAllResources()
	{
		m_shaderPrograms.clear();
		m_textures.clear();
		m_sprites.clear();
	}

	std::string ResourceManager::getFileString(const std::string& relativeFilePath)
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

	std::shared_ptr<RenderEngine::ShaderProgram> ResourceManager::loadShaders
	(const std::string& shaderName, const std::string& vertexPath,
		const std::string& fragmentPath)
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
		std::shared_ptr<RenderEngine::ShaderProgram> temp_ptr
		(new RenderEngine::ShaderProgram(vertexString, fragmentString));

		std::shared_ptr<RenderEngine::ShaderProgram>& newShader =
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
				<< "Fragment: " << fragmentPath << std::endl;
			return nullptr;
		}

	}

	std::shared_ptr<RenderEngine::ShaderProgram> ResourceManager::getShaderProgram(const std::string& shaderName)
	{
		ShaderProgramsMap::const_iterator it = m_shaderPrograms.find(shaderName);
		if (it != m_shaderPrograms.end())
			return it->second;
		std::cerr << "Can't find the shader program: " << shaderName << std::endl;
		return nullptr;
	}

	std::shared_ptr<RenderEngine::Texture2D> ResourceManager::loadTexture
	(const std::string& textureName, const std::string& texturePath)
	{
		int channels = 0;
		int width = 0;
		int height = 0;

		stbi_set_flip_vertically_on_load(true);
		unsigned char* pixels = stbi_load(std::string(m_path + "/" + texturePath).c_str(),
			&width, &height, &channels, 0); // ������ ������/��������

		if (!pixels)
		{
			std::cerr << "Can't load image: " << texturePath << std::endl;
			return nullptr;
		}

		std::shared_ptr<RenderEngine::Texture2D> newTexture =
			m_textures.emplace(textureName, std::make_shared<RenderEngine::Texture2D>(width,
				height,
				pixels,
				channels,
				GL_NEAREST,
				GL_CLAMP_TO_EDGE)
			).first->second;

		stbi_image_free(pixels); // ����������� ������, ���������� ��� �������� � ����������� ������, ��� 
		// ��� �������� ��� �������� � ������ ����������
		return newTexture;
	}

	std::shared_ptr<RenderEngine::Texture2D> ResourceManager::getTexture(const std::string& textureName)
	{
		TexturesMap::const_iterator it = m_textures.find(textureName);
		if (it != m_textures.end())
			return it->second;
		std::cerr << "Can't find the texture: " << textureName << std::endl;
		return nullptr;
	}

	std::shared_ptr<RenderEngine::Sprite> ResourceManager::loadSprite(const std::string& spriteName,
		const std::string& textureName,
		const std::string& shaderName,
		const std::string& subTextureName)
	{
		auto pTexture = getTexture(textureName);
		if (!pTexture)
		{
			std::cerr << "Can't find the texture: "
				<< textureName << " for the sprite: " << spriteName << std::endl;
		}

		auto pShader = getShaderProgram(shaderName);
		if (!pTexture)
		{
			std::cerr << "Can't find the shader program: "
				<< shaderName << " for the sprite: " << spriteName << std::endl;
		}

		std::shared_ptr<RenderEngine::Sprite> newSprite =
			m_sprites.emplace(spriteName, std::make_shared<RenderEngine::Sprite>(pTexture,
				subTextureName,
				pShader)
			).first->second;

		return newSprite;

	}

	std::shared_ptr<RenderEngine::Sprite> ResourceManager::getSprite(const std::string& spriteName)
	{
		SpritesMap::const_iterator it = m_sprites.find(spriteName);
		if (it != m_sprites.end())
			return it->second;
		std::cerr << "Can't find the sprite: " << spriteName << std::endl;
		return nullptr;
	}

	std::shared_ptr<RenderEngine::Texture2D> ResourceManager::loadTextureAtlas(std::string textureName,
		std::string texturePath,
		std::vector<std::string> subTextures,
		const unsigned int subTextureWidth,
		const unsigned int subTextureHeight)
	{
		auto pTexture = loadTexture(std::move(textureName), std::move(texturePath));
		if (pTexture)
		{
			const unsigned int textureWidth = pTexture->width();
			const unsigned int textureHeight = pTexture->height();
			unsigned int currentTextureOffsetX = 0;
			unsigned int currentTextureOffsetY = textureHeight;
			for (auto& currentSubTextureName : subTextures)
			{
				glm::vec2 leftBottomUV(static_cast<float>(currentTextureOffsetX + 0.01f) / textureWidth,
					static_cast<float>(currentTextureOffsetY - subTextureHeight + 0.01f) / textureHeight);
				glm::vec2 rightTopUV(static_cast<float>(currentTextureOffsetX + subTextureWidth - 0.01f) / textureWidth,
					static_cast<float>(currentTextureOffsetY - 0.01f) / textureHeight);

				pTexture->addSubtexture(std::move(currentSubTextureName), leftBottomUV, rightTopUV);

				currentTextureOffsetX += subTextureWidth;
				if (currentTextureOffsetX >= textureWidth)
				{
					currentTextureOffsetX = 0;
					currentTextureOffsetY -= subTextureHeight;
				}

			}
		}
		return pTexture;
	}

	bool ResourceManager::loadJSONResources(const std::string& JSONPath)
	{
		const std::string JSONstring = getFileString(JSONPath);
		if (JSONstring.empty())
		{
			std::cerr << "No JSON recources faile!" << std::endl;
			return false;
		}
		rapidjson::Document document;
		rapidjson::ParseResult parseResult = document.Parse(JSONstring.c_str());
		if (!parseResult)
		{
			std::cerr << "JSON parse error: " << rapidjson::GetParseError_En(parseResult.Code())
				<< "(" << parseResult.Offset() << ")" << std::endl;
			std::cerr << "In JSON file: " << JSONPath << std::endl;
			return false;
		}

		auto shadersIt = document.FindMember("shaders");
		if (shadersIt != document.MemberEnd())
		{
			for (const auto& currentShader : shadersIt->value.GetArray())
			{
				const std::string name = currentShader["name"].GetString();
				const std::string filePath_v = currentShader["filePath_v"].GetString();
				const std::string filePath_f = currentShader["filePath_f"].GetString();
				loadShaders(name, filePath_v, filePath_f);
			}
		}

		auto textureAtlasIt = document.FindMember("textureAtlas");
		if (textureAtlasIt != document.MemberEnd())
		{
			for (const auto& currentTextureAtlas : textureAtlasIt->value.GetArray())
			{
				const std::string name = currentTextureAtlas["name"].GetString();
				const std::string filePath = currentTextureAtlas["filePath"].GetString();
				const unsigned int subTextureWidth = currentTextureAtlas["subTextureWidth"].GetUint();
				const unsigned int subTextureHeight = currentTextureAtlas["subTextureHeight"].GetUint();
				const auto subTexturesArray = currentTextureAtlas["subTextures"].GetArray();
				std::vector<std::string> subTextures;
				subTextures.reserve(subTexturesArray.Size());
				for (const auto& currentSubTexture : subTexturesArray)
				{
					subTextures.emplace_back(currentSubTexture.GetString());
				}
				loadTextureAtlas(name, filePath, std::move(subTextures), subTextureWidth, subTextureHeight);

			}
		}

		auto spritesIt = document.FindMember("sprites");
		if (spritesIt != document.MemberEnd())
		{
			for (const auto& currentSprite : spritesIt->value.GetArray())
			{
				const std::string name = currentSprite["name"].GetString();
				const std::string textureAtlas = currentSprite["textureAtlas"].GetString();
				const std::string shader = currentSprite["shader"].GetString();
				const std::string subTexture = currentSprite["initialSubTexture"].GetString();


				auto pSprite = loadSprite(name, textureAtlas, shader, subTexture);
				if (!pSprite)
				{
					continue;
				}
				auto framesIt = currentSprite.FindMember("frames");
				if (framesIt != currentSprite.MemberEnd())
				{
					const auto framesArray = framesIt->value.GetArray();
					std::vector<RenderEngine::Sprite::FrameDescription> framesDesciptions;
					framesDesciptions.reserve(framesArray.Size());
					for (const auto& currentFrame : framesArray)
					{
						const std::string subTextureStr = currentFrame["subTexture"].GetString();
						const double duration = currentFrame["duration"].GetDouble();
						const auto pTextureAtlas = getTexture(textureAtlas);
						const auto pSubTexture = pTextureAtlas->getSubTexture(subTextureStr);
						framesDesciptions.emplace_back(pSubTexture.leftBottomUV, pSubTexture.rightTopUV, duration);
					}
					pSprite->insertFrames(framesDesciptions);
				}

			}
		}

		auto startScreenIt = document.FindMember("start_screen");
		if (startScreenIt != document.MemberEnd())
		{
			const auto descriptionArray = startScreenIt->value.GetArray();
			m_startScreen.reserve(descriptionArray.Size());
			size_t maxLength = 0;
			for (const auto& currentRow : descriptionArray)
			{
				m_startScreen.emplace_back(currentRow.GetString());
				if (maxLength < m_startScreen.back().length())
					maxLength = m_startScreen.back().length();


			}
			for (auto& currentRow : m_startScreen)
			{
				while (currentRow.length() < maxLength)
				{
					currentRow.append("F");
				}
			}
		}

		auto gameOverIt = document.FindMember("game_over");
		if (gameOverIt != document.MemberEnd())
		{
			const auto descriptionArray = gameOverIt->value.GetArray();
			m_gameOver.reserve(descriptionArray.Size());
			size_t maxLength = 0;
			for (const auto& currentRow : descriptionArray)
			{
				m_gameOver.emplace_back(currentRow.GetString());
				if (maxLength < m_gameOver.back().length())
					maxLength = m_gameOver.back().length();


			}
			for (auto& currentRow : m_gameOver)
			{
				while (currentRow.length() < maxLength)
				{
					currentRow.append("F");
				}
			}
		}


		auto levelsIt = document.FindMember("levels");
		if (levelsIt != document.MemberEnd())
		{
			for (const auto& currentLevel : levelsIt->value.GetArray())
			{
				const auto description = currentLevel["description"].GetArray();
				std::vector<std::string> levelRows;
				levelRows.reserve(description.Size());
				size_t maxLength = 0;
				for (const auto& currentRow : description)
				{
					levelRows.emplace_back(currentRow.GetString());
					if (maxLength < levelRows.back().length())
					{
						maxLength = levelRows.back().length();
					}
				}

				for (auto& currentRow : levelRows)
				{
					while (currentRow.length() < maxLength)
					{
						currentRow.append("D");
					}
				}

				const auto enemies = currentLevel["enemies"].GetArray();
				std::vector<std::pair<Tank::ETankType, int>> mapTanksType;
				for (const auto& currentType : enemies)
				{
					if (currentType.FindMember("type1") != currentType.MemberEnd())
					{
						const int count = currentType["type1"].GetInt();
						mapTanksType.emplace_back( Tank::ETankType::EnemyWhite_type1, count);
					}
					else if (currentType.FindMember("type2") != currentType.MemberEnd())
					{
						const int count = currentType["type2"].GetInt();
						mapTanksType.emplace_back( Tank::ETankType::EnemyWhite_type2, count);
					}
					else if (currentType.FindMember("type3") != currentType.MemberEnd())
					{
						const int count = currentType["type3"].GetInt();
						mapTanksType.emplace_back( Tank::ETankType::EnemyWhite_type3, count);
					}
					else
					{
						const int count = currentType["type4"].GetInt();
						mapTanksType.emplace_back( Tank::ETankType::EnemyWhite_type4, count);
					}
				}
				auto res = descriptionLevel(std::move(levelRows), std::move(mapTanksType));
				m_levels.push_back(res);
			}
		}
		return true;
	}
}

