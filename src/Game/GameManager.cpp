
#include <iostream>
#include "GameObjects/IGameObject.h"
#include "UI/GameStates/Level.h" // tank Game state
#include "GameObjects/Bonus.h"


GameManager::GameManager(Game* pGame)
	: m_isGameOver(false)
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
}

void GameManager::gameOver()
{
	
	m_pGame->gameOver();
}

void GameManager::setStartScreen()
{
	m_pGame->setStartScreen();
}

void GameManager::restart(Tank* pTank1, Tank* pTank2)
{
	m_pGame->restart(pTank1, pTank2);
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

