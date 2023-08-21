#pragma once 

#include <array>
#include <memory>
#include "../../Renderer/ShaderProgram.h"
#include "../../Renderer/VertexBufferLayout.h"

#include "../../Renderer/VertexBuffer.h"
#include "../../Renderer/VertexArray.h"
#include "../../Renderer/IndexBuffer.h"
#include "../../Renderer/sprite.h"

class Level;


namespace primitives
{
	class Line
	{
	public:

		enum class ELinePosition
		{
			MENU,
			RESTART,
			SOUND
		};

		Line(unsigned int width, unsigned int height, glm::vec2 offset = glm::vec2(0));
		void render();
		void setPosition(ELinePosition pos);
		ELinePosition m_eCurrentPosition;



	private:

		GLuint m_VAO;
		std::vector<glm::vec2> m_linePositions ;
		RenderEngine::VertexArray m_vertexArray;
		RenderEngine::VertexBuffer m_vertexCoordsBuffer;
		RenderEngine::VertexBuffer m_colorCoordsBuffer;
		RenderEngine::IndexBuffer m_indexBuffer;
		std::shared_ptr<RenderEngine::ShaderProgram> m_pShaderProgram;

		glm::vec2 m_pos;
		glm::vec2 m_size;
	};
}


class UIMenuLevel
{
public:
	UIMenuLevel(Level* pLevel, std::shared_ptr<RenderEngine::ShaderProgram> pShaderProgram);

	void render() const;
	void upLine();
	void downLine();
	primitives::Line::ELinePosition getCUrrentPositionLine() const { return m_pLine->m_eCurrentPosition; }


	
	

private:
	Level* m_pLevel;
	GLuint m_VAO;

	std::shared_ptr<primitives::Line> m_pLine;

	const float m_color[4] = { 0.38f, 0.38f, 0.38f, 0.5f };

	RenderEngine::VertexArray m_vertexArray;
	RenderEngine::VertexBuffer m_vertexCoordsBuffer;
	RenderEngine::VertexBuffer m_colorCoordsBuffer;
	RenderEngine::IndexBuffer m_indexBuffer;

	//glm::vec2 m_pos = {m_widthPixels / 2 - BLOCK_SIZE * 3, m_heightPixels / 2 - BLOCK_SIZE * 2)};

	std::shared_ptr<RenderEngine::ShaderProgram> m_pShaderProgram;

	std::vector<std::pair<std::shared_ptr<RenderEngine::Sprite>, glm::vec2>> m_sprites;
	std::shared_ptr<RenderEngine::Sprite> m_menuSprite;

	glm::vec2 m_posBack;
	glm::vec2 m_sizeBack;
	glm::vec2 m_scaleScreen;



};

