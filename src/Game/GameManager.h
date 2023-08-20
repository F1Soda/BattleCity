#pragma once 

#include <memory>
#include <vector>

class Level;

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
	void nextLevel();
	void gameOver();

private:
	Game* m_pGame;
	Level* m_pCurrentLevel;
	//std::vector<std::weak_ptr> objectsOnLevel;

};