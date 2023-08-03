#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>



#include "Game.h"
#include "GameObjects/Tank.h"
#include "../Resources/ResourceManager.h"
#include "../Renderer/ShaderProgram.h"
#include "../Resources/ResourceManager.h"
#include "../Renderer/Texture2D.h"
#include "../Renderer/sprite.h"
#include "Level.h"

#include <GLFW/glfw3.h>


bool Game::lighting = true;
Game::EMoveState Game::eMoveStateFirstButton = EMoveState::Idle;
Game::EMoveState Game::eMoveStateSecondButton = EMoveState::Idle;
uint64_t durationBetweenButtonsClicking = 1000000000;
uint64_t currentFrameTime = 0;

Game::Game(const glm::ivec2& windowSize)
	: m_eCurrentGameSTate(EGameState::Active)
    , m_windowSize(windowSize)
{
	m_keys.fill(false);
}
Game::~Game()
{

}

void Game::render()
{
    //ResourceManager::getAnimatedSprite("NewAnimatedSprite")->render();
    
    if (m_pTank)
        m_pTank->render();
    
    if(m_pLevel)
        m_pLevel->render();

    //pAnimatedSprite->render();

}
void Game::update(const double delta)
{
    if (currentFrameTime <= durationBetweenButtonsClicking)
        currentFrameTime += delta;

    if (m_pLevel)
    {
        m_pLevel->update(delta);
    }

    if (m_pTank)
    {
        if (m_keys[GLFW_KEY_W])
        {
            m_pTank->setOrientation(Tank::EOrientation::Top);
            m_pTank->move(true);
        }
        if (m_keys[GLFW_KEY_A])
        {
            m_pTank->setOrientation(Tank::EOrientation::Left);
            m_pTank->move(true);
        }
        if (m_keys[GLFW_KEY_S])
        {
            m_pTank->setOrientation(Tank::EOrientation::Bottom);
            m_pTank->move(true);
        }
        if (m_keys[GLFW_KEY_D])
        {
            m_pTank->setOrientation(Tank::EOrientation::Right);
            m_pTank->move(true);
        }
        if (!m_keys[GLFW_KEY_W] && !m_keys[GLFW_KEY_A] && !m_keys[GLFW_KEY_S] && !m_keys[GLFW_KEY_D])
        {
            m_pTank->move(false);
        }
        
        m_pTank->update(delta);
    }

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

    auto pSpriteShaderProgram = ResourceManager::getShaderProgram("spriteShader");
    if (!pSpriteShaderProgram)
    {
        std::cerr << "Can't find shader program: " << "spriteShader" << std::endl;
        return false;
    }

    auto pTextureAtlas = ResourceManager::getTexture("mapTextureAtlas_16x16");
    if (!pTextureAtlas)
    {
        std::cerr << "Can't find texture atlas: " << "mapTextureAtlas" << std::endl;
        return false;
    }

    auto pTanksTextureAtlas = ResourceManager::getTexture("tanksTextureAtlas");
    if (!pTanksTextureAtlas)
    {
        std::cerr << "Can't find texture atlas: " << "tanksTextureAtlas" << std::endl;
        return false;
    }

    m_pLevel = std::make_unique<Level>(ResourceManager::getLevels()[0]);
    m_windowSize.x = static_cast<int>(m_pLevel->getLevelWidth());
    m_windowSize.y = static_cast<int>(m_pLevel->getLevelHeight());


    glm::mat4 projectionMatrix = glm::ortho(0.0f, static_cast<float>(m_windowSize.x), 0.0f, static_cast<float>(m_windowSize.y), -100.0f, 100.0f);

    pSpriteShaderProgram->use();
    pSpriteShaderProgram->setInt("tex", 0);
    pSpriteShaderProgram->setMatrix4("projectionMat", projectionMatrix);

    m_pTank = std::make_unique<Tank>(0.05, m_pLevel->getPlayerRespawn_1(), glm::vec2(Level::BLOCK_SIZE, Level::BLOCK_SIZE), 0.f);


   

    return true;

}

size_t Game::getCurrentLevelWidth() const
{
    return m_pLevel->getLevelWidth();
}
size_t Game::getCurrentLevelHeight() const
{
    return m_pLevel->getLevelHeight();
}


