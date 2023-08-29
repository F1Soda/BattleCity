#include "End.h"

#include <iostream>
#include "../../../Resources/ResourceManager.h"
#include "../../../Renderer/sprite.h"
#include <GLFW/glfw3.h>
#include <array>
#include "../../GameManager.h"

End::End(GameManager* pGameManager, glm::vec2 scaleWindow)
    : m_currentMenuSelection(0)
    , m_keyReleased(true)
    , m_mainSprite(std::make_pair(Resources::ResourceManager::getSprite("end"), glm::vec2(35, 0)))
    , m_pGameManager(pGameManager)
    , m_leftRightRotate(true)
{
    m_F1SodaSprites.reserve(4);
    m_F1SodaSprites.emplace_back(Resources::ResourceManager::getSprite("F1SodaType1"), glm::vec2(0, 10));
    m_F1SodaSprites.emplace_back(Resources::ResourceManager::getSprite("F1SodaType2"), glm::vec2(0, 260));
    m_F1SodaSprites.emplace_back(Resources::ResourceManager::getSprite("F1SodaType3"), glm::vec2(170 , 260));
    m_F1SodaSprites.emplace_back(Resources::ResourceManager::getSprite("F1SodaType4"), glm::vec2(170, 10));

    m_rotationTimer.setCallback([&]() {m_leftRightRotate = !m_leftRightRotate; m_rotationTimer.start(m_durationBetweenRotatingHeads); });
    m_rotationTimer.start(m_durationBetweenRotatingHeads);
    m_pGameManager->playMusic(AudioManager::EMusicType::End);
    m_musicEndVolume = m_pGameManager->getVolumeMusic(AudioManager::EMusicType::End);
}

void End::render() const
{
    m_mainSprite.first->render(m_mainSprite.second, glm::vec2(130, 290), 0.f);

    for (const auto& current : m_F1SodaSprites)
    {
        if (current.first)
        {
            if (m_leftRightRotate)
            {
                current.first->render(current.second, glm::vec2(m_sizes_F1SodaHeaders, m_sizes_F1SodaHeaders + 0), 30.f);
            }
            else
            {
                current.first->render(current.second, glm::vec2(m_sizes_F1SodaHeaders, m_sizes_F1SodaHeaders + 0), 330.f);
            }
            
        }
    }
    
   
}
void End::update(const double delta)
{
    m_rotationTimer.update(delta);
}
unsigned int End::getStateWidth() const
{
    return m_endSprite_width;
}
unsigned int End::getStateHeight() const
{
    return m_endSprite_height;
}


void End::processInput(std::array<char, 349>& keys)
{
    if (!keys[GLFW_KEY_ENTER])
        m_keyReleased = true;

    if (m_keyReleased)
    {
        
        if (keys[GLFW_KEY_ENTER])
        {
            m_pGameManager->isEndSceen = false;
            m_pGameManager->stopMusic(AudioManager::EMusicType::End);
            m_pGameManager->setStartScreen();
        }

    }

    

}