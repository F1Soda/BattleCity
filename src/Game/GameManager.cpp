#include "GameManager.h"

#include "Game.h"
#include <iostream>
#include "UI/GameStates/Level.h"

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