#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include "../Resources/ResourceManager.h"
#include "../Renderer/ShaderProgram.h"
#include "../Renderer/Texture2D.h"
#include "../Renderer/sprite.h"
#include "../Game/GameStates/Level.h"
#include "../Game/GameStates/StartScreen.h"
#include "Game.h"
#include "../Physics/PhysicsEngine.h"
#include "../Renderer/Renderer.h"
#include <GLFW/glfw3.h>
#include "AIComponent.h"

bool Game::lighting = true;

GLFWwindow* Game::pWindow;

double durationBetweenButtonsClicking = 1000;
double currentFrameTime = 0;

// Prototypes

Game::Game(const glm::uvec2& windowSize)
    : m_eCurrentGameState(EGameState::StartScreen)
    , m_windowSize(windowSize)
    , m_currentLevelIndex(0)
{
	m_keys.fill(false);
    
}
Game::~Game()
{
}

void Game::render()
{
    m_pCurrentGameState->render();
}

void Game::update(GLFWwindow* pWindow,const double delta)
{
    if (currentFrameTime <= durationBetweenButtonsClicking)
        currentFrameTime += delta;
    m_pCurrentGameState->update(delta);
    m_pCurrentGameState->processInput(m_keys);

    if (m_keys[GLFW_KEY_O] && currentFrameTime >= durationBetweenButtonsClicking)
    {
        currentFrameTime = 0;
        lighting = !lighting;
        std::cout << "Turn " << (Game::lighting ? "on " : "off") << " lighting" << std::endl;
    }
    //pAnimatedSprite->update(delta);
}
void Game::setKey(const int key, const int action)
{
	m_keys[key] = action;
}
bool Game::init()
{
    ResourceManager::loadJSONResources("res/resources.json");

    m_pSpriteShaderProgram = ResourceManager::getShaderProgram("spriteShader");
    if (!m_pSpriteShaderProgram)
    {
        std::cerr << "Can't find shader program: " << "spriteShader" << std::endl;
        return false;
    }
    m_pSpriteShaderProgram->use();
    m_pSpriteShaderProgram->setInt("tex", 0);
    auto pLevel = std::make_shared<Level>(ResourceManager::getLevels()[2], Game::EGameMode::OnePlayer);
    m_pCurrentGameState = pLevel; //std::make_shared<StartScreen>(ResourceManager::getStartScreen(), this);
    Physics::PhysicsEngine::setCurrentLevel(pLevel);
    setWindowSize(m_windowSize);
    updateViewport();

    AIComponent::m_eEnemyState = AIComponent::EEnemyState::Сhaotic;

    return true;

}

unsigned int Game::getCurrentWidth() const
{
    return m_pCurrentGameState->getStateWidth();
}
unsigned int Game::getCurrentHeight() const
{
    return m_pCurrentGameState->getStateHeight();
} 
 
void Game::startNewLevel(const size_t level, const EGameMode eGameMode)
{
    m_currentLevelIndex = level;
    auto pLevel = std::make_shared<Level>(ResourceManager::getLevels()[m_currentLevelIndex], eGameMode);
    Physics::PhysicsEngine::setCurrentLevel(pLevel);
    m_pCurrentGameState = pLevel;
    updateViewport();
}

void Game::setWindowSize(const glm::uvec2 windowSize)
{
    m_windowSize = windowSize;
    updateViewport();
}

void Game::updateViewport()
{
    const float level_aspect_ratio = static_cast<float>(getCurrentWidth()) /getCurrentHeight();;

    unsigned int viewPortWidth = m_windowSize.x;
    unsigned int viewPortHeight = m_windowSize.y;
    unsigned int viewPortLeftOffset = 0;
    unsigned int viewPortBottomOffset = 0;

    if (static_cast<float>(m_windowSize.x) / m_windowSize.y > level_aspect_ratio)
    {
        viewPortWidth = static_cast<int>(m_windowSize.y * level_aspect_ratio);
        viewPortLeftOffset = (m_windowSize.x - viewPortWidth) / 2;
    }
    else
    {
        viewPortHeight = static_cast<int>(m_windowSize.x / level_aspect_ratio);
        viewPortBottomOffset = (m_windowSize.y - viewPortHeight) / 2;
    }

    // Показываме OpenGL где мы хотим рисовать. 
    // Первые два параметра -- кординаты начала полотна, затем ширина и высота
    RenderEngine::Renderer::setViewport(viewPortWidth, viewPortHeight, viewPortLeftOffset, viewPortBottomOffset);

    glm::mat4 projectionMatrix = glm::ortho(0.0f, static_cast<float>(getCurrentWidth()), 0.0f, static_cast<float>(getCurrentHeight()), -100.0f, 100.0f);
    m_pSpriteShaderProgram->setMatrix4("projectionMat", projectionMatrix);
}

void Game::nextLevel(const EGameMode eGameMode)
{
    startNewLevel(++m_currentLevelIndex, eGameMode);
}

