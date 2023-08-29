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

	std::shared_ptr<Tank> spawn(Tank::ETankType eTankType, bool isEnemy, int beginingLifes);



	// only for players Tanks
	void respawn(Tank& tank);

	bool isBusy() const { return m_isBusy; }

	void update(double delta);
	void setPosition(glm::vec2 pos) { m_position = pos; }

	static const double MAX_VELOCITY_PLAYER_TANK_TYPE1;
	static const double MAX_VELOCITY_PLAYER_TANK_TYPE2;
	static const double MAX_VELOCITY_PLAYER_TANK_TYPE3;
	static const double MAX_VELOCITY_PLAYER_TANK_TYPE4;

	static const double MAX_VELOCITY_PLAYER_BULLET_TYPE1;
	static const double MAX_VELOCITY_PLAYER_BULLET_TYPE2;
	static const double MAX_VELOCITY_PLAYER_BULLET_TYPE3;
	static const double MAX_VELOCITY_PLAYER_BULLET_TYPE4;

	static const double MAX_VELOCITY_ENEMY_TANK_TYPE1;
	static const double MAX_VELOCITY_ENEMY_TANK_TYPE2;
	static const double MAX_VELOCITY_ENEMY_TANK_TYPE3;
	static const double MAX_VELOCITY_ENEMY_TANK_TYPE4;
									 
	static const double MAX_VELOCITY_ENEMY_BULLET_TYPE1;
	static const double MAX_VELOCITY_ENEMY_BULLET_TYPE2;
	static const double MAX_VELOCITY_ENEMY_BULLET_TYPE3;
	static const double MAX_VELOCITY_ENEMY_BULLET_TYPE4;

private:

	glm::vec2 m_position;
	bool m_isBusy;
	Timer respawnTimer;
	Level* m_pLevel;
	GameManager* m_pGameManager;

};