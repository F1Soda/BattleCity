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
    {
        m_pTank->render();
    }
    if(m_pLevel)
        m_pLevel->render();

    //pAnimatedSprite->render();

}
void Game::update(const uint64_t delta)
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
        else if (m_keys[GLFW_KEY_A])
        {
            m_pTank->setOrientation(Tank::EOrientation::Left);
            m_pTank->move(true);
        }
        else if (m_keys[GLFW_KEY_S])
        {
            m_pTank->setOrientation(Tank::EOrientation::Bottom);
            m_pTank->move(true);
        }
        else if (m_keys[GLFW_KEY_D])
        {
            m_pTank->setOrientation(Tank::EOrientation::Right);
            m_pTank->move(true);
        }
        else
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

   

    glm::mat4 projectionMatrix = glm::ortho(0.0f, static_cast<float>(m_windowSize.x), 0.0f, static_cast<float>(m_windowSize.y), -100.0f, 100.0f);

    pSpriteShaderProgram->use();
    pSpriteShaderProgram->setInt("tex", 0);
    pSpriteShaderProgram->setMatrix4("projectionMat", projectionMatrix);

    m_pTank = std::make_unique<Tank>(0.0000001f, glm::vec2(0, 0), glm::vec2(16.f, 16.f));


    m_pLevel = std::make_unique<Level>(ResourceManager::getLevels()[0]);

    return true;

}