#pragma once

#include "glm/vec2.hpp"
#include <memory>
#include "../../System/Timer.h"
#include "../GameObjects/Tank.h"

class Level;
class GameManager;

class Spawner
{
public:
	Spawner(glm::vec2 position, Level* pLevel, GameManager* pGameManager);

	std::shared_ptr<Tank> Spawner::spawn(Tank::ETankType eTankType, bool isEnemy);



	// only for players Tanks
	void respawn(Tank& tank);

	bool isBusy() const { return m_isBusy; }

	void update(double delta);
	void setPosition(glm::vec2 pos) { m_position = pos; }

private:

	glm::vec2 m_position;
	bool m_isBusy;
	Timer respawnTimer;
	Level* m_pLevel;
	GameManager* m_pGameManager;
};