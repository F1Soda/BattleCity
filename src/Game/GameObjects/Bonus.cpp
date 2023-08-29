#include "Bonus.h"
#include "../../Resources/ResourceManager.h"
#include "../../Renderer/sprite.h"
#include "../GameManager.h"
#include "../AudioManager.h"
#include <iostream>

Bonus::Bonus(ETypeBonus type, glm::vec2 pos, float layer, GameManager* pGameManager)
	: IGameObject(pos, glm::vec2(28), 0.f, layer, IGameObject::EObjectType::Bonus)
	, m_renderBonus(true)
	, m_pGameManager(pGameManager)
	, m_typeBonus(type)
{
	switch (type)
	{
	case Bonus::ETypeBonus::Armor:
		m_pSprite = Resources::ResourceManager::getSprite("bonus_helmet");
		break;
	case Bonus::ETypeBonus::TimeFreeze:
		m_pSprite = Resources::ResourceManager::getSprite("bonus_clock");
		break;
	case Bonus::ETypeBonus::Shovel:
		m_pSprite = Resources::ResourceManager::getSprite("bonus_shovel");
		break;
	case Bonus::ETypeBonus::Star:
		m_pSprite = Resources::ResourceManager::getSprite("bonus_star");
		break;
	case Bonus::ETypeBonus::Granade:
		m_pSprite = Resources::ResourceManager::getSprite("bonus_grenade");
		break;
	case Bonus::ETypeBonus::DoubleTank:
		m_pSprite = Resources::ResourceManager::getSprite("bonus_tank");
		break;
	}

	m_bonusExistingTimer.setCallback([&]() { destroy(); });
	m_bonusExistingTimer.start(m_durationExistion);
	m_flashingTimer.setCallback([&]() {m_renderBonus = !m_renderBonus; m_flashingTimer.start(m_timeBetweenFlashing); });
	m_flashingTimer.start(m_timeBetweenFlashing);

	auto onCollisionCallback = [&](const IGameObject& object, const Physics::ECollisionDirection direction)
		{
			
		};
	m_colliders.emplace_back(glm::vec2(2), m_size - glm::vec2(2), onCollisionCallback);
}

void Bonus::update(double duration)
{
	m_flashingTimer.update(duration);
	m_bonusExistingTimer.update(duration);
}

void Bonus::render() const
{
	if (m_renderBonus)
	{
		m_pSprite->render(m_position, m_size, 0.f, m_layer);
	}
	
}
void Bonus::onCollision(IGameObject& object)
{
	if (object.getObjectType() == IGameObject::EObjectType::Tank)
	{
		//IGameObject* objectWithoutConst = const_cast<IGameObject*>(&object);
		Tank* pTank = dynamic_cast<Tank*>(&object);
		m_pGameManager->playSound(AudioManager::EAudioType::Bonus);
		if (!pTank->m_isEnemyTank)
		{
			std::cout << "Bonus taken!\n";
			switch (m_typeBonus)
			{
			case Bonus::ETypeBonus::Armor:
				m_pGameManager->bonusAddShieldToTank(pTank, m_durationShield);
				break;
			case Bonus::ETypeBonus::TimeFreeze:
				m_pGameManager->bonusFreezeTime(m_durationFreeze);
				break;
			case Bonus::ETypeBonus::Shovel:
				m_pGameManager->bonusBuildFort(m_durationExistingBetonFord);
				break;
			case Bonus::ETypeBonus::Star:
				m_pGameManager->bonusUpgradeTank(pTank);
				break;
			case Bonus::ETypeBonus::Granade:
				m_pGameManager->bonusDestroyAllEnemyTanks();
				break;
			case Bonus::ETypeBonus::DoubleTank:
				m_pGameManager->bonusAddLife(pTank);
				break;
			}
			destroy();
		}
	}
}


void Bonus::destroy()
{
	m_pGameManager->destroyBonus(this);
}
