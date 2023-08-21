#pragma once 

#include <memory>
#include <vector>
//#include "Game.h" // level
class Level;
class IGameObject;
class Game;
class Tank;

class GameManager
{
public:
	GameManager(Game* pGame);

	void reduceCountTankPlayers(Tank* pTank);
	void reduceCountTankEnemy(Tank* enemyTank);
	void setStartScreen();
	//int getCurrentCountTankEnemies() const { return m_pGame->getCurrentCountTankEnemies(); }
	void setCrrentLevel(Level* pLevel);
	bool isLevel() { return m_pCurrentLevel != nullptr; }
	void nextLevel();
	void gameOver();
	void restart();
	void pause();
	glm::vec2 getScaleScreenInPixels() const ;
	glm::vec2 getScaleScreenInBlocks() const ;
	IGameObject* getObjectByIndex(int index) const ;
	void Exit();

private:
	Game* m_pGame;
	Level* m_pCurrentLevel;
	//std::vector<std::weak_ptr> objectsOnLevel;

};