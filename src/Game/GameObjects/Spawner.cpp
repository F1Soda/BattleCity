#include "../GameObjects/Tank.h"
#include "Spawner.h"
#include "../UI/GameStates/Level.h"
#include "../RandomLib/include/effolkronium/random.hpp"
#include <iostream>

const double Spawner::MAX_VELOCITY_PLAYER_TANK_TYPE1 = 0.04;
const double Spawner::MAX_VELOCITY_PLAYER_TANK_TYPE2 = 0.05;
const double Spawner::MAX_VELOCITY_PLAYER_TANK_TYPE3 = 0.05;
const double Spawner::MAX_VELOCITY_PLAYER_TANK_TYPE4 = 0.04;

const double Spawner::MAX_VELOCITY_PLAYER_BULLET_TYPE1 = 0.2;
const double Spawner::MAX_VELOCITY_PLAYER_BULLET_TYPE2 = 0.2;
const double Spawner::MAX_VELOCITY_PLAYER_BULLET_TYPE3 = 0.2;
const double Spawner::MAX_VELOCITY_PLAYER_BULLET_TYPE4 = 0.25;

const double Spawner::MAX_VELOCITY_ENEMY_TANK_TYPE1 = 0.035;
const double Spawner::MAX_VELOCITY_ENEMY_TANK_TYPE2 = 0.05;
const double Spawner::MAX_VELOCITY_ENEMY_TANK_TYPE3 = 0.04;
const double Spawner::MAX_VELOCITY_ENEMY_TANK_TYPE4 = 0.030;

const double Spawner::MAX_VELOCITY_ENEMY_BULLET_TYPE1 = 0.15;
const double Spawner::MAX_VELOCITY_ENEMY_BULLET_TYPE2 = 0.15;
const double Spawner::MAX_VELOCITY_ENEMY_BULLET_TYPE3 = 0.2;
const double Spawner::MAX_VELOCITY_ENEMY_BULLET_TYPE4 = 0.15;


Spawner::Spawner(glm::vec2 position, Level* pLevel, GameManager* pGameManager)
	: m_position(position)
	, m_isBusy(false)
	, m_pLevel(pLevel)
	, m_pGameManager(pGameManager)
{
	respawnTimer.setCallback([&]() { m_isBusy = false; });
}

void Spawner::update(double delta)
{
	respawnTimer.update(delta);
}

std::shared_ptr<Tank> Spawner::spawn(Tank::ETankType eTankType, bool isEnemy, int beginingLifes)
{
	using Random = effolkronium::random_static;
	bool isBonusTank;
	if (isEnemy && true)
	{
		isBonusTank = Random::get<bool>(0.25f);
	}
	else
	{
		isBonusTank = false;
	}


	respawnTimer.start(Level::DURATION_RESPAWNING);
	m_isBusy = true;

	double maxVelocutyBullet = 0;
	double maxVelocutyTank = 0;

	switch (eTankType)
	{
	case Tank::ETankType::Player1Yellow_type1:
		maxVelocutyBullet = MAX_VELOCITY_PLAYER_BULLET_TYPE1;
		maxVelocutyTank = MAX_VELOCITY_PLAYER_TANK_TYPE1;
		break;
	case Tank::ETankType::Player1Yellow_type2:
		maxVelocutyBullet = MAX_VELOCITY_PLAYER_BULLET_TYPE2;
		maxVelocutyTank = MAX_VELOCITY_PLAYER_TANK_TYPE2;
		break;
	case Tank::ETankType::Player1Yellow_type3:
		maxVelocutyBullet = MAX_VELOCITY_PLAYER_BULLET_TYPE3;
		maxVelocutyTank = MAX_VELOCITY_PLAYER_TANK_TYPE3;
		break;
	case Tank::ETankType::Player1Yellow_type4:
		maxVelocutyBullet = MAX_VELOCITY_PLAYER_BULLET_TYPE4;
		maxVelocutyTank = MAX_VELOCITY_PLAYER_TANK_TYPE4;
		break;
	case Tank::ETankType::Player2Green_type1:
		maxVelocutyBullet = MAX_VELOCITY_PLAYER_BULLET_TYPE1;
		maxVelocutyTank = MAX_VELOCITY_PLAYER_TANK_TYPE1;
		break;
	case Tank::ETankType::Player2Green_type2:
		maxVelocutyBullet = MAX_VELOCITY_PLAYER_BULLET_TYPE2;
		maxVelocutyTank = MAX_VELOCITY_PLAYER_TANK_TYPE2;
		break;
	case Tank::ETankType::Player2Green_type3:
		maxVelocutyBullet = MAX_VELOCITY_PLAYER_BULLET_TYPE3;
		maxVelocutyTank = MAX_VELOCITY_PLAYER_TANK_TYPE3;
		break;
	case Tank::ETankType::Player2Green_type4:
		maxVelocutyBullet = MAX_VELOCITY_PLAYER_BULLET_TYPE4;
		maxVelocutyTank = MAX_VELOCITY_PLAYER_TANK_TYPE4;
		break;
	case Tank::ETankType::EnemyWhite_type1:
		maxVelocutyBullet = MAX_VELOCITY_ENEMY_BULLET_TYPE1;
		maxVelocutyTank = MAX_VELOCITY_ENEMY_TANK_TYPE1;
		break;
	case Tank::ETankType::EnemyWhite_type2:
		maxVelocutyBullet = MAX_VELOCITY_ENEMY_BULLET_TYPE2;
		maxVelocutyTank = MAX_VELOCITY_ENEMY_TANK_TYPE2;
		break;
	case Tank::ETankType::EnemyWhite_type3:
		maxVelocutyBullet = MAX_VELOCITY_ENEMY_BULLET_TYPE3;
		maxVelocutyTank = MAX_VELOCITY_ENEMY_TANK_TYPE3;
		break;
	case Tank::ETankType::EnemyWhite_type4:
		maxVelocutyBullet = MAX_VELOCITY_ENEMY_BULLET_TYPE4;
		maxVelocutyTank = MAX_VELOCITY_ENEMY_TANK_TYPE4;
		break;
	default:
		
		std::cerr << "ERROR: Wrong type tank in Spawner::spawn(), type(int) = " << int(eTankType) << std::endl;
		break;
	}

	return std::make_shared<Tank>(eTankType, m_pLevel, isEnemy, !isEnemy, isEnemy ? Tank::EOrientation::Bottom : Tank::EOrientation::Top, maxVelocutyTank, maxVelocutyBullet, m_position, glm::vec2(Level::BLOCK_SIZE, Level::BLOCK_SIZE), 0.05f, m_pGameManager, isBonusTank, beginingLifes);
}

void Spawner::respawn(Tank& tank)
{
	tank.getTargetPosition() = m_position;
	tank.respawn();
	tank.setVelocity(0);
}

