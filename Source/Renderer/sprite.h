#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"


#include <glad/glad.h>
#include <memory>
#include <glm/vec2.hpp>
#include <string>

namespace RenderEngine
{
	class Texture2D;
	class ShaderProgram;

	class Sprite
	{
	public:

		struct FrameDescription
		{
			FrameDescription(const glm::vec2 _leftBottomUV, glm::vec2 _rightTopUV, uint64_t _duration)
				: leftBottomUV(_leftBottomUV)
				, rightTopUV(_rightTopUV)
				, duration(_duration)
			{}

			glm::vec2 leftBottomUV;
			glm::vec2 rightTopUV;
			uint64_t duration;
		};

		Sprite(std::shared_ptr<Texture2D> pTexture,
			std::string initialSubTexture,
			std::shared_ptr<ShaderProgram> pShaderProgram);
		~Sprite();
		Sprite(const Sprite&) = delete;
		Sprite& operator=(const Sprite&) = delete;

		void render(const glm::vec2& position, const glm::vec2& size, const float rotation, const size_t frameId=0) const;

		uint64_t getFrameDuration(const size_t frameID) const;
		size_t getFramesCount() const;
		void insertFrames(std::vector<FrameDescription> framesDescriptions);

	protected:
		std::shared_ptr<Texture2D> m_pTexture;
		std::shared_ptr<ShaderProgram> m_pShaderProgram;
		GLuint m_VAO;

		VertexArray m_vertexArray;
		VertexBuffer m_vertexCoordsBuffer;
		VertexBuffer m_textureCoordsBuffer;
		IndexBuffer m_indexBuffer;

		std::vector<FrameDescription> m_frameDescriprtions;

		mutable size_t m_lastFrameId;

	};

}