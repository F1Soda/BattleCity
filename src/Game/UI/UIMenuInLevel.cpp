
#include "UIMenuInLevel.h"
#include "../../Renderer/ShaderProgram.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../../Renderer/Renderer.h"
//#include <glm/mat4x4.hpp>
#include "../../Resources/ResourceManager.h"
#include <glad/glad.h>
#include "GameStates/Level.h"

#include <GLFW/glfw3.h>

static GLuint id1;
static GLuint id2;

UIMenuLevel::UIMenuLevel(Level* pLevel, std::shared_ptr<RenderEngine::ShaderProgram> pShaderProgram)
	: m_pLevel(pLevel)
	, m_VAO(0)
	, m_pShaderProgram(pShaderProgram)
	, m_menuSprite(Resources::ResourceManager::getSprite("menuLevel"))
	
{
	const GLfloat vertexCoords[] =
	{
		// 1---2
		// | / |
		// 0---3
		0.0f, 0.0f, 
		0.0f, 1.0f, 
		1.0f, 1.0f,
		1.0f, 0.0f

	};
	
	const GLfloat colorCoords[] =
	{
		0.5f, 0.5f, 0.5f, 0.8f ,
		0.5f, 0.5f, 0.5f, 0.8f ,
		0.5f, 0.5f, 0.5f, 0.8f ,
		0.5f, 0.5f, 0.5f, 0.8f
	};

	const GLuint indices[] =
	{
		0,1,2,
		2,3,0
	};

	m_vertexCoordsBuffer.init(vertexCoords, 2 * 4 * sizeof(GLfloat));
	RenderEngine::VertexBufferLayout vertexCoordsLayout;
	vertexCoordsLayout.addElementLayoutFloat(2, false);
	m_vertexArray.addBuffer(m_vertexCoordsBuffer, vertexCoordsLayout);

	m_colorCoordsBuffer.init(colorCoords, 4 * 4 * sizeof(GLfloat));
	RenderEngine::VertexBufferLayout colorCoordsLayout;
	colorCoordsLayout.addElementLayoutFloat(4, false);
	m_vertexArray.addBuffer(m_colorCoordsBuffer, colorCoordsLayout);

	m_indexBuffer.init(indices, 6);

	m_vertexArray.unbind();
	m_indexBuffer.unbind();

	m_posBack = glm::vec2(m_pLevel->getWindowSizeInPixels().x / 2 - Level::BLOCK_SIZE * 2, m_pLevel->getWindowSizeInPixels().y / 2 - Level::BLOCK_SIZE * 4);
	m_sizeBack = glm::vec2(Level::BLOCK_SIZE * 5, Level::BLOCK_SIZE * 8);
	m_scaleScreen = m_pLevel->getWindowSizeInPixels();

	m_pLine = std::make_shared<primitives::Line>(60, 2, m_posBack);
}

void UIMenuLevel::render() const
{
	//glm::vec2(m_widthPixels / 2 - BLOCK_SIZE * 3, m_heightPixels / 2 - BLOCK_SIZE * 2), glm::vec2(BLOCK_SIZE * 8, BLOCK_SIZE * 5), 0.f, 1.2f

	m_pShaderProgram->use();
	
	glm::mat4 model(1.0f);

	model = glm::translate(model, glm::vec3(m_posBack, 0.0f));
	model = glm::translate(model, glm::vec3(0.5f * m_sizeBack.x, 0.5f * m_sizeBack.y, 0.0f));
	model = glm::rotate(model, glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-0.5f * m_sizeBack.x, -0.5f * m_sizeBack.y, 0.0f));
	model = glm::scale(model, glm::vec3(m_sizeBack, 1.0f));

	m_pShaderProgram->setMatrix4("modelMat", model);
	m_pShaderProgram->setFloat("layer", 1.2f);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	RenderEngine::Renderer::draw(m_vertexArray, m_indexBuffer, *m_pShaderProgram);
	
	glDisable(GL_BLEND);
	

	m_menuSprite->render(m_posBack+glm::vec2(0), m_sizeBack, 0.f, 1.21f);
	
	m_pLine->render();

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	
	m_pShaderProgram->use();

}

void UIMenuLevel::upLine()
{
	if (m_pLine->m_eCurrentPosition == primitives::Line::ELinePosition::MENU)
	{
		m_pLine->m_eCurrentPosition = primitives::Line::ELinePosition::SOUND;
	}
	else
	{
		m_pLine->m_eCurrentPosition = primitives::Line::ELinePosition((int(m_pLine->m_eCurrentPosition) - 1) % 3);
	}
	m_pLine->setPosition(m_pLine->m_eCurrentPosition);
}

void UIMenuLevel::downLine()
{
	m_pLine->m_eCurrentPosition = primitives::Line::ELinePosition((int(m_pLine->m_eCurrentPosition) + 1) % 3);
	m_pLine->setPosition(m_pLine->m_eCurrentPosition);
}

namespace primitives
{
	Line::Line(unsigned int width, unsigned int height, glm::vec2 offset)
		:  m_VAO(0)
		, m_eCurrentPosition(ELinePosition::MENU)
		, m_size(glm::vec2(width, height))
		, m_pShaderProgram(Resources::ResourceManager::getShaderProgram("UIShader"))
		
	{

		m_linePositions  = { offset + glm::vec2(8, 75) ,
						 offset + glm::vec2(8,50),
						offset + glm::vec2(8, 20)
		};

		m_pos = m_linePositions[int(m_eCurrentPosition)];

		const GLfloat vertexCoords[] =
		{
			// 1---2
			// | / |
			// 0---3
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f

		};

		const GLfloat colorCoords[] =
		{
			1.f, 1.f, 1.f, 1.f ,
			1.f, 1.f, 1.f, 1.f ,
			1.f, 1.f, 1.f, 1.f ,
			1.f, 1.f, 1.f, 1.f 
		};

		const GLuint indices[] =
		{
			0,1,2,
			2,3,0
		};

		m_vertexCoordsBuffer.init(vertexCoords, 2 * 4 * sizeof(GLfloat));
		RenderEngine::VertexBufferLayout vertexCoordsLayout;
		vertexCoordsLayout.addElementLayoutFloat(2, false);
		m_vertexArray.addBuffer(m_vertexCoordsBuffer, vertexCoordsLayout);

		m_colorCoordsBuffer.init(colorCoords, 4 * 4 * sizeof(GLfloat));
		RenderEngine::VertexBufferLayout colorCoordsLayout;
		colorCoordsLayout.addElementLayoutFloat(4, false);
		m_vertexArray.addBuffer(m_colorCoordsBuffer, colorCoordsLayout);

		m_indexBuffer.init(indices, 6);

		m_vertexArray.unbind();
		m_indexBuffer.unbind();
	}
	void Line::render()
	{
		m_pShaderProgram->use();

		glm::mat4 model(1.0f);

		model = glm::translate(model, glm::vec3(m_pos, 0.0f));
		model = glm::translate(model, glm::vec3(0.5f * m_size.x, 0.5f * m_size.y, 0.0f));
		model = glm::rotate(model, glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.5f * m_size.x, -0.5f * m_size.y, 0.0f));
		model = glm::scale(model, glm::vec3(m_size, 1.0f));

		m_pShaderProgram->setMatrix4("modelMat", model);
		m_pShaderProgram->setFloat("layer", 1.22f);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		RenderEngine::Renderer::draw(m_vertexArray, m_indexBuffer, *m_pShaderProgram);
	}
	void Line::setPosition(ELinePosition pos)
	{
		m_pos = m_linePositions[int(pos)];
	}

}