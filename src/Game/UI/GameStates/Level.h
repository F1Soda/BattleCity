#pragma once

#include "IGameState.h" 
#include "../../GameObjects/Tank.h"
#include <set>
#include "../../Game.h"
#include "../../GameObjects/Spawner.h"

class IGameObject;
struct GLFWwindow;
//class UIMenuLevel;

class Level : public IGameState
{
public:
	static constexpr unsigned int BLOCK_SIZE = 16;
	static const unsigned int DURATION_RESPAWNING = 1500;
	Level(const std::vector<std::string>& levelDescription, std::unordered_map<Tank::ETankType, int> enemiesTypeMap, const Game::EGameMode eGameMode, GameManager* pGameManager);
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

	static Tank::EOrientation Level::eMoveStateSecondButtonFirstPlayer;
	static Tank::EOrientation Level::eMoveStateFirstButtonFirstPlayer;

	static Tank::EOrientation Level::eMoveStateSecondButtonSecondPlayer;
	static Tank::EOrientation Level::eMoveStateFirstButtonSecondPlayer;

	void destroyEnemyTank(Tank* pTankToDelete);
	void destroyPlayerTank(Tank* pTank);

private:
	size_t m_widthBlocks = 0;
	size_t m_heightBlocks = 0;

	bool isOnPause;

	friend void Spawner::setPosition(glm::vec2 pos);

	unsigned int m_widthPixels = 0;
	unsigned int m_heightPixels = 0;

	unsigned int m_currentCountEnemyTanksOnLevel;

	size_t m_currentCountPlayersTanks;

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
	
	//std::shared_ptr<UIMenuLevel> m_pUIMenuLevel;

	Timer m_endingLevelTimer;
	Timer m_respawnTimer;

	bool isDestroed;

	//                 type,            count 
	std::unordered_map<Tank::ETankType, int> m_enemiesTankTypeMap;

	void addEnemyTank(Tank::ETankType tankType = Tank::ETankType::EnemyWhite_type1);
	void respawnEnemyTank();
	void nextLevel();
	void setStartScreen();
};