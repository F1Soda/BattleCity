#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include "../Resources/ResourceManager.h"
#include "../Renderer/ShaderProgram.h"
#include "../Renderer/Texture2D.h"
#include "../Renderer/sprite.h"
#include "../Game/UI/GameStates/Level.h"
#include "../Game/UI/GameStates/StartScreen.h"
#include "Game.h"
#include "../Physics/PhysicsEngine.h"
#include "../Renderer/Renderer.h"
#include <GLFW/glfw3.h>
#include "Components/AIComponent.h"
#include "GameManager.h"
#include "UI/GameStates/GameOver.h"


bool Game::lighting = true;

GLFWwindow* Game::pWindow;

double durationBetweenButtonsClicking = 1000;
double currentFrameTime = 0;
static unsigned int indexStartLevel = 3;

// Prototypes

Game::Game(const glm::uvec2& windowSize)
    : m_eCurrentGameState(EGameState::StartScreen)
    , m_windowSize(windowSize)
    //, m_currentCountEnemyTanks(COUNT_ENEMY_TANKS)
    //, m_currentCountPlayersTanks(COUNT_PLAYERS_TANKS)
    ,m_currentLevelIndex(0)
    
{
    m_endingLevelTimer.setCallback([&]()
        {
            std::cerr << "Call empty edingLevelCallback!";
            std::abort();
        });

	m_keys.fill(false);
    m_pGameManager = std::make_shared<GameManager>(this);
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

    m_endingLevelTimer.update(delta);

    if (m_keys[GLFW_KEY_O] && currentFrameTime >= durationBetweenButtonsClicking)
    {
        currentFrameTime = 0;
        lighting = !lighting;
        std::cout << "Turn " << (Game::lighting ? "on " : "off") << " lighting" << std::endl;
    }
        

}
void Game::setKey(const int key, const int action)
{
	m_keys[key] = action;
}
bool Game::init()
{
    Resources::ResourceManager::loadJSONResources("res/resources.json");
    m_countLevels = Resources::ResourceManager::getCountLevels();
    m_pSpriteShaderProgram = Resources::ResourceManager::getShaderProgram("spriteShader");
    if (!m_pSpriteShaderProgram)
    {
        std::cerr << "Can't find shader program: " << "spriteShader" << std::endl;
        return false;
    }
    m_pSpriteShaderProgram->use();
    m_pSpriteShaderProgram->setInt("tex", 0);
    setStartScreen(); 
    gameOver(glm::vec2(Level::BLOCK_SIZE*13, Level::BLOCK_SIZE * 14));
    //setWindowSize(m_windowSize);
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
    m_currentGameMode = eGameMode;
    auto levelDes = (*Resources::ResourceManager::getLevels())[level];
    auto pLevel = std::make_shared<Level>(levelDes.mapStructure, levelDes.enemiesTypeMaps, eGameMode, m_pGameManager.get());
    Physics::PhysicsEngine::setCurrentLevel(pLevel);
    m_pGameManager->setCrrentLevel(pLevel.get());
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
    m_currentLevelIndex++;
    m_currentLevelIndex %= m_countLevels;
    //std::cout << "YOU WIN" << std::endl;
    startNewLevel(m_currentLevelIndex, eGameMode);
}

void Game::setStartScreen()
{
    m_pCurrentGameState = std::make_shared<StartScreen>(Resources::ResourceManager::getStartScreen(), this);
    updateViewport();
}

void Game::gameOver(glm::vec2& scaleScreen)
{

    if (scaleScreen == glm::vec2(0))
    {
        m_pCurrentGameState = std::make_shared<GameOver>(Resources::ResourceManager::getGameOver(), this, glm::vec2(m_pCurrentGameState->getStateWidth(), m_pCurrentGameState->getStateHeight()));
    }
    else
    {
        m_pCurrentGameState = std::make_shared<GameOver>(Resources::ResourceManager::getGameOver(), this, scaleScreen);
    }
        

}
