#include "selectStage.h"

#include <iostream>
#include "../../../Resources/ResourceManager.h"
#include "../../../Renderer/sprite.h"
#include <GLFW/glfw3.h>
#include <array>



// Prototypes
static std::shared_ptr<RenderEngine::Sprite> getSpriteForDescription(const char description);

SelectStage::SelectStage(GameManager* pGameManager, glm::vec2 screenScale, Game::EGameMode eGameMode)
	: m_currentMenuSelection(0)
	, m_keyReleased(true)
	, m_screenScale(screenScale)
	, m_eGameMode(eGameMode)
	, m_pGameManager(pGameManager)
{
	m_spritesStage.reserve(8);
	m_stagePos = glm::vec2(screenScale.x/2-30, screenScale.y/2);
	m_spritesStage.emplace_back(Resources::ResourceManager::getSprite("Stage1"));
	m_spritesStage.emplace_back(Resources::ResourceManager::getSprite("Stage2"));
	m_spritesStage.emplace_back(Resources::ResourceManager::getSprite("Stage3"));
	m_spritesStage.emplace_back(Resources::ResourceManager::getSprite("Stage4"));
	m_spritesStage.emplace_back(Resources::ResourceManager::getSprite("Stage5"));
	m_spritesStage.emplace_back(Resources::ResourceManager::getSprite("Stage6"));
	m_spritesStage.emplace_back(Resources::ResourceManager::getSprite("Stage7"));
	m_spritesStage.emplace_back(Resources::ResourceManager::getSprite("Stage8"));

	m_waitOneSecondBeforStartTimer.setCallback([&]() {m_canStart = true; });
	m_waitOneSecondBeforStartTimer.start(450);
}

void SelectStage::render() const
{
	m_spritesStage[m_currentMenuSelection]->render(m_stagePos, glm::vec2(64, 15), 0.f);
}
void SelectStage::update(const double delta)
{
	m_waitOneSecondBeforStartTimer.update(delta);
}
unsigned int SelectStage::getStateWidth() const
{
	return m_screenScale.x;
}
unsigned int SelectStage::getStateHeight() const
{
	return m_screenScale.y;
}

void SelectStage::processInput(std::array<char, 349>& keys)
{
	if (!m_canStart)
	{
		return;
	}


	if (!keys[GLFW_KEY_W] && !keys[GLFW_KEY_S])
		m_keyReleased = true;

	if (m_keyReleased)
	{

		if (keys[GLFW_KEY_W])
		{
			m_keyReleased = false;
			if (m_currentMenuSelection > 0)
				--m_currentMenuSelection;
			m_pGameManager->playSound(AudioManager::EAudioType::NavigateInMenu);
		}
		else if (keys[GLFW_KEY_S])
		{
			m_keyReleased = false;
			
			if (m_currentMenuSelection < 7)
				++m_currentMenuSelection;
			m_pGameManager->playSound(AudioManager::EAudioType::NavigateInMenu);
		}

	}

	if (keys[GLFW_KEY_ENTER])
	{
		switch (m_eGameMode)
		{
		case Game::EGameMode::OnePlayer:
			m_pGameManager->playSound(AudioManager::EAudioType::SelectInMenu);
			m_pGameManager->m_pGame->startNewLevel(m_currentMenuSelection, Game::EGameMode::OnePlayer);
			break;
		case Game::EGameMode::TwoPlayer:
			m_pGameManager->playSound(AudioManager::EAudioType::SelectInMenu);
			m_pGameManager->m_pGame->startNewLevel(m_currentMenuSelection, Game::EGameMode::TwoPlayer, Tank::ETankType::Player1Yellow_type1, Tank::ETankType::Player2Green_type1);
			break;
		default:
			break;
		}
	}

}