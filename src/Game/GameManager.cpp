
#include <iostream>
#include "GameObjects/IGameObject.h"
#include "UI/GameStates/Level.h" // tank Game state
#include "GameObjects/Bonus.h"
#include "UI/GameStates/selectStage.h"

GameManager::GameManager(Game* pGame)
	: m_isGameOver(false)
	, isEndSceen(false)
{
	m_pGame = pGame;
	m_pCurrentLevel = nullptr;
	m_pAudioManager = std::make_shared<AudioManager>(this);
}


void GameManager::setCrrentLevel(Level* pLevel)
{
	m_pCurrentLevel = pLevel;
}


void GameManager::reduceCountTankPlayers(Tank* pTank)
{
	m_pCurrentLevel->destroyPlayerTank(pTank);
}
void GameManager::reduceCountTankEnemy(Tank* enemyTank)
{
	m_pCurrentLevel->destroyEnemyTank(enemyTank);
}

void GameManager::nextLevel(Tank* pTank1, Tank* pTank2)
{
	m_pGame->nextLevel(m_pGame->m_currentGameMode, pTank1, pTank2);
	m_isGameOver = false;
}

void GameManager::gameOver()
{
	m_pGame->gameOver();
}

void GameManager::end()
{
	isEndSceen = true;
	m_pGame->end();
}


void GameManager::setStartScreen()
{
	m_pGame->setStartScreen();
	m_isGameOver = false;
}

void GameManager::restart(Tank* pTank1, Tank* pTank2)
{
	m_pGame->restart(pTank1, pTank2);
	m_isGameOver = false;
}

void GameManager::pause()
{
	m_pCurrentLevel->setPause();
}

glm::vec2 GameManager::getScaleScreenInPixels() const
{
	return m_pCurrentLevel->getWindowSizeInPixels();
}

glm::vec2 GameManager::getScaleScreenInBlocks() const
{
	return m_pCurrentLevel->getSizeLevelInBlocks(); 
}

IGameObject* GameManager::getObjectByIndex(int index) const
{
	 return m_pCurrentLevel->getObjectByIndex(index).get(); 
}

void GameManager::Exit()
{
	m_pGame->exit();
}
void GameManager::addBonus()
{
	m_pCurrentLevel->createBonus();
}

void GameManager::destroyBonus(Bonus* pBonus)
{
	m_pCurrentLevel->destroyBonus(pBonus);
}

void GameManager::bonusAddShieldToTank(Tank* tank, double duration)
{
	tank->activateShield(duration);
}
void GameManager::bonusFreezeTime(double duration)
{
	m_pCurrentLevel->freeze(duration);
}
void GameManager::bonusBuildFort(double duration)
{
	m_pCurrentLevel->buildFort(duration);
}
void GameManager::bonusUpgradeTank(Tank* tank)
{
	tank->levelUp();
}
void GameManager::bonusDestroyAllEnemyTanks()
{
	m_pCurrentLevel->destroyAllEnemyTanks();
}
void GameManager::bonusAddLife(Tank* pTank)
{
	pTank->addLife();
}

void GameManager::setFullWindow()
{
	m_pGame->setFullWindow();
}

void GameManager::SetVolumeMusic(AudioManager::EMusicType eType, float volume)
{
	m_pAudioManager->setVolumeMusic(eType, volume);
}

void GameManager::playMusic(AudioManager::EMusicType eType)
{
	m_pAudioManager->playMusic(eType);
}

float GameManager::getVolumeMusic(AudioManager::EMusicType eType)
{
 	return m_pAudioManager->getVolumeMusic(eType);
}

void GameManager::stopMusic(AudioManager::EMusicType eType)
{
	m_pAudioManager->stopMusic(eType);
}

void GameManager::setSelectStageScreen(int iGameMode)
{
	m_pGame->selecetStageScreen(Game::EGameMode(iGameMode));
	m_isGameOver = false;
}

//void GameManager::startNewLevel(const size_t level, const Game::EGameMode eGameMode, Tank::ETankType typeTank1, Tank::ETankType typeTank2, int beginingLifesTank1, int beginigLifesTank2)
//{
//	m_pGame->startNewLevel(level, eGameMode, typeTank1, typeTank2, beginingLifesTank1, beginigLifesTank2);
//}