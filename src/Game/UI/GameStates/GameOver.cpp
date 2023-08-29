#include "GameOver.h"

#include "../../../Renderer/sprite.h"
#include "../../../Resources/ResourceManager.h"
#include "../../Game.h"
#include <iostream>
#include <GLFW/glfw3.h>

std::shared_ptr<RenderEngine::Sprite> getSpriteForDescription(const char description)
{
    switch (description)
    {
    case '0':
        return Resources::ResourceManager::getSprite("brickWall_All");
    case '1':
        return Resources::ResourceManager::getSprite("brickWall_TopLeft");
    case '2':
        return Resources::ResourceManager::getSprite("brickWall_TopRight");
    case '3':
        return Resources::ResourceManager::getSprite("brickWall_Top");
    case '4':
        return Resources::ResourceManager::getSprite("brickWall_BottomLeft");
    case '5':
        return Resources::ResourceManager::getSprite("brickWall_Left");
    case '6':
        return Resources::ResourceManager::getSprite("brickWall_TopRight_BottomLeft");
    case '7':
        return Resources::ResourceManager::getSprite("brickWall_Top_BottomLeft");
    case '8':
        return Resources::ResourceManager::getSprite("brickWall_BottomRight");
    case '9':
        return Resources::ResourceManager::getSprite("brickWall_TopLeft_BottomRight");
    case 'A':
        return Resources::ResourceManager::getSprite("brickWall_Right");
    case 'B':
        return Resources::ResourceManager::getSprite("brickWall_Top_BottomRight");
    case 'C':
        return Resources::ResourceManager::getSprite("brickWall_Bottom");
    case 'D':
        return Resources::ResourceManager::getSprite("brickWall_TopLeft_Bottom");
    case 'E':
        return Resources::ResourceManager::getSprite("brickWall_TopRight_Bottom");
    case 'F':
        return nullptr;
    default:
        std::cerr << "Unknown Start Level description: " << description << std::endl;
    }
    return nullptr;
}


GameOver::GameOver(const std::vector<std::string>& gameOverDescription, Game* pGame, glm::vec2 scaleWindow)
	: m_pGame(pGame)
	, m_keyReleased(false)
	, m_scaleScreen(scaleWindow)
{
    //scaleWindow.x / 2 - WIDTH_GAME_OVER_IN_BLOCKS;
    unsigned int leftOffsetPixels = scaleWindow.x / 2 - WIDTH_GAME_OVER_IN_BLOCKS/2 * 16;
    unsigned int bottomOffset = m_scaleScreen.y + 11 * BLOCK_SIZE;

	unsigned int currentBottomOffset = bottomOffset;
	for (const std::string& currentRow : gameOverDescription)
	{
		unsigned int currentLeftOffset = leftOffsetPixels;
		for (const char currentElement : currentRow)
		{
			m_sprites.emplace_back(std::make_pair(getSpriteForDescription(currentElement), glm::vec2(currentLeftOffset, currentBottomOffset)));
			currentLeftOffset += BLOCK_SIZE;
		}
		currentBottomOffset -= BLOCK_SIZE;
	}

    m_autoGoToTheStartSCreenTimer.setCallback([&]() { m_pGame->setStartScreen(); });
    m_autoGoToTheStartSCreenTimer.start(5000);
}



void GameOver::render() const
{
    for (const auto& currentPair : m_sprites)
    {
        if (currentPair.first)
        {
            currentPair.first->render(currentPair.second, glm::vec2(BLOCK_SIZE), 0.f);
        }
    }
}
void GameOver::update(const double delta)
{
    if (m_sprites[1].second.y > m_scaleScreen.y - 4 * BLOCK_SIZE)
    {
        moveText(delta);
    }
    
    m_autoGoToTheStartSCreenTimer.update(delta);

    
}
void GameOver::processInput(std::array<char, 349>& keys)
{
    if (!keys[GLFW_KEY_ENTER])
    {
        m_keyReleased = true;
    }
    if (m_keyReleased)
    {
        if (keys[GLFW_KEY_ENTER])
        {

            m_pGame->setStartScreen();
            m_keyReleased = false;
        }
    }
}

unsigned int GameOver::getStateWidth() const
{
    return m_scaleScreen.x;
}
unsigned int GameOver::getStateHeight() const
{ 
    return m_scaleScreen.y;
}


void GameOver::moveText(double delta)
{
    for (auto& currentPair : m_sprites)
    {
        if (currentPair.first)
        {
            currentPair.second -= glm::vec2(0, delta * 0.055f);
        }
    }
}