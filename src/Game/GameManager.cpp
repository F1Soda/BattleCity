
#include <iostream>
#include "GameObjects/IGameObject.h"
#include "UI/GameStates/Level.h" // tank Game state


GameManager::GameManager(Game* pGame)
{
	m_pGame = pGame;
	m_pCurrentLevel = nullptr;
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

void GameManager::nextLevel()
{
	m_pGame->nextLevel(m_pGame->m_currentGameMode);
}

void GameManager::gameOver()
{
	m_pGame->gameOver();
}

void GameManager::setStartScreen()
{
	m_pGame->setStartScreen();
}

void GameManager::restart()
{
	m_pGame->restart();
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
