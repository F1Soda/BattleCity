#include "../GameObjects/Tank.h"
#include "Spawner.h"
#include "../UI/GameStates/Level.h"

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

std::shared_ptr<Tank> Spawner::spawn(Tank::ETankType eTankType, bool isEnemy)
{
	respawnTimer.start(Level::DURATION_RESPAWNING);
	m_isBusy = true;
	return std::make_shared<Tank>(eTankType, m_pLevel, isEnemy, !isEnemy, Tank::EOrientation::Top, 0.04, m_position, glm::vec2(Level::BLOCK_SIZE, Level::BLOCK_SIZE), 0.05f, m_pGameManager);
}

void Spawner::respawn(Tank& tank)
{
	tank.getTargetPosition() = m_position;
	tank.respawn();
}

