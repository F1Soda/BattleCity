#pragma once

#include "IGameState.h" 
#include "../../GameObjects/Tank.h"
#include <set>
#include "../../Game.h"
#include "../../GameObjects/Spawner.h"
#include "../../../System/Timer.h"
#include <map>

class IGameObject;
struct GLFWwindow;
class UIMenuLevel;
class Bonus;

class Level : public IGameState
{
public:
	static constexpr unsigned int BLOCK_SIZE = 16;
	static const unsigned int DURATION_RESPAWNING = 2000;
	Level(const std::vector<std::string>& levelDescription, std::vector<std::pair<Tank::ETankType, int>> enemiesType, const Game::EGameMode eGameMode, GameManager* pGameManager
		  , Tank::ETankType primalTypeTank1, Tank::ETankType, int beginingLifeTank1, int beginingLifeTank2 = 0);
	virtual void render() const override;
	virtual void update(const double  delta) override;
	virtual unsigned int getStateWidth() const override;
	virtual unsigned int getStateHeight() const override;
	virtual void processInput(std::array<char, 349>& keys) override;

	const Spawner* getPlayerSpawner_1() const { return m_playerSpawner_1.get(); }
	const Spawner* getPlayerSpawner_2() const { return m_playerSpawner_2.get(); }
	const Spawner* getEnemySpawner_1() const { return m_enemySpawner_1.get(); }
	const Spawner* getEnemySpawner_2() const { return m_enemySpawner_2.get(); }
	const Spawner* getEnemySpawner_3() const { return m_enemySpawner_3.get(); }
	const glm::vec2 getWindowSizeInPixels() const;
	const glm::vec2 getSizeLevelInBlocks() const;

	const std::shared_ptr<IGameObject> getObjectByIndex(unsigned int index) const { return m_levelObjects[index]; }

	void changeLevelObject(unsigned int indexToCahnge, std::shared_ptr<IGameObject> newObject);

	std::vector<std::shared_ptr<IGameObject>> getObjectsInArea(const glm::vec2& bottomLeft, const glm::vec2& topRight);
	void initLevel();

	int getCountEnemyTank();

	friend void CheckButtonStatusFirstPlayer(int key, Tank::EOrientation eCurrentState);
	friend void CheckButtonStatusSecondPlayer(int key, Tank::EOrientation eCurrentState);
	std::shared_ptr<IGameObject> createGameObjectFromDiscription(const char description, const glm::vec2& position, const glm::vec2& size, const float rotation);

	static Tank::EOrientation eMoveStateSecondButtonFirstPlayer;
	static Tank::EOrientation eMoveStateFirstButtonFirstPlayer;

	static Tank::EOrientation eMoveStateSecondButtonSecondPlayer;
	static Tank::EOrientation eMoveStateFirstButtonSecondPlayer;

	void destroyEnemyTank(Tank* pTankToDelete);
	void destroyPlayerTank(Tank* pTank);

	void setPause() { m_isOnPause = true; }

	Tank::ETankType getTypeFirstPlayer() { return m_pTank1->getTankType(); }
	Tank::ETankType getTypeSecondPlayer() { return m_pTank2->getTankType(); }

	Bonus* createBonus();
	void destroyBonus(Bonus* pBonus);
	void freeze(double duration);
	bool isFreeze()const { return m_isFreeze; }
	void destroyAllEnemyTanks();
	void buildFort(double duration);

	glm::vec2 getEaglePos() const { return m_posEagle; }

private:
	int m_maxCountTanksOnLevel;
	size_t m_widthBlocks = 0;
	size_t m_heightBlocks = 0;

	bool m_isOnPause;
	bool m_isFreeze;

	Timer m_freezeTimer;
	Timer m_spawningEnemyTimer;
	double m_durationBetweenSpawning = 2000;
	bool m_canSpawnEnemyTanks;


	Tank::ETankType m_primalTypeTankFirstPlayer;
	Tank::ETankType m_primalTypeTankSecondPlayer;
	int m_primalLifesFirstTank;
	int m_primalLifesSecondTank;

	friend void Spawner::setPosition(glm::vec2 pos);

	unsigned int m_widthPixels = 0;
	unsigned int m_heightPixels = 0;

	unsigned int m_currentCountEnemyTanksOnLevel;

	glm::vec2 m_posEagle;

	size_t m_currentCountPlayersTanks;

	std::vector<std::shared_ptr<Bonus>> m_bonuses;

	std::shared_ptr<Spawner> m_playerSpawner_1;
	std::shared_ptr<Spawner> m_playerSpawner_2;
	std::shared_ptr<Spawner> m_enemySpawner_1;
	std::shared_ptr<Spawner> m_enemySpawner_2;
	std::shared_ptr<Spawner> m_enemySpawner_3;
	std::vector<std::shared_ptr<IGameObject>> m_levelObjects;
	std::shared_ptr<Tank> m_pTank1;
	std::shared_ptr<Tank> m_pTank2;
	std::vector<std::shared_ptr<Tank>> m_enemyTanks;
	Game::EGameMode m_eGameMode;
	GameManager* m_pGameManager;
	
	std::shared_ptr<UIMenuLevel> m_pUIMenuLevel;

	Timer m_endingLevelTimer;
	Timer m_respawnTimer;
	Timer m_betonFortTimer;

	bool isDestroed;
	bool m_isCreatedBettonFort;
	//                 type,            count 
	std::vector<std::pair<Tank::ETankType, int>> m_enemiesTankType;

	void addEnemyTank(int index);
	void respawnEnemyTank();
	void nextLevel();
	void setStartScreen();

	std::vector<std::shared_ptr<IGameObject>> m_betonWallFort;
};