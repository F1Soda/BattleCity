#pragma once

#include <vector>
#include <string>
#include <memory>
#include <glm/vec2.hpp>
#include "IGameState.h"
#include "../GameObjects/Tank.h"
#include <set>
#include "../Game.h"
class IGameObject;
struct GLFWwindow;


class Level : public IGameState
{
public:
	static constexpr unsigned int BLOCK_SIZE = 16;
	Level(const std::vector<std::string>& levelDescription, const Game::EGameMode eGameMode);
	virtual void render() const override;
	virtual void update(const double  delta) override;
	virtual unsigned int getStateWidth() const override;
	virtual unsigned int getStateHeight() const override;
	virtual void processInput(std::array<char, 349>& keys) override;

	const glm::ivec2& getPlayerRespawn_1() const { return m_playerRespawn_1; }
	const glm::ivec2& getPlayerRespawn_2() const { return m_playerRespawn_2; }
	const glm::ivec2& getEnemyRespawn_1() const { return m_enemyRespawn_1; }
	const glm::ivec2& getEnemyRespawn_2() const { return m_enemyRespawn_2; }
	const glm::ivec2& getEnemyRespawn_3() const { return m_enemyRespawn_3; }
	const glm::vec2 getWindowSizeInPixels() const;
	const glm::vec2 getSizeLevelInBlocks() const;
	const std::shared_ptr<IGameObject> getObjectByIndex(unsigned int index) const { return m_levelObjects[index]; }
	void changeLevelObject(unsigned int indexToCahnge, std::shared_ptr<IGameObject> newObject);

	std::vector<std::shared_ptr<IGameObject>> getObjectsInArea(const glm::vec2& bottomLeft, const glm::vec2& topRight);
	void initLevel();



	friend void CheckButtonStatusFirstPlayer(int key, Tank::EOrientation eCurrentState);
	friend void CheckButtonStatusSecondPlayer(int key, Tank::EOrientation eCurrentState);

	static Tank::EOrientation Level::eMoveStateSecondButtonFirstPlayer;
	static Tank::EOrientation Level::eMoveStateFirstButtonFirstPlayer;

	static Tank::EOrientation Level::eMoveStateSecondButtonSecondPlayer;
	static Tank::EOrientation Level::eMoveStateFirstButtonSecondPlayer;

	

private:
	size_t m_widthBlocks = 0;
	size_t m_heightBlocks = 0;

	unsigned int m_widthPixels = 0;
	unsigned int m_heightPixels = 0;
	
	glm::ivec2 m_playerRespawn_1;
	glm::ivec2 m_playerRespawn_2;
	glm::ivec2 m_enemyRespawn_1;
	glm::ivec2 m_enemyRespawn_2;
	glm::ivec2 m_enemyRespawn_3;
	std::vector<std::shared_ptr<IGameObject>> m_levelObjects;
	std::shared_ptr<Tank> m_pTank1;
	std::shared_ptr<Tank> m_pTank2;
	std::set<std::shared_ptr<Tank>> m_EnemyTanks;
	Game::EGameMode m_eGameMode;
};