
#include "../../Renderer/sprite.h"
#include "../../Resources/ResourceManager.h"
#include "../../Renderer/spriteAnimator.h"
#include "../../System/Timer.h"
#include "Bullet.h"
#include "Tank.h"
#include <iostream>
#include "../GameManager.h"

Bullet::Bullet(const double velocity, const glm::vec2& position,
	glm::vec2& size, glm::vec2& explosionSize, const float layer, GameManager* pGameManager)
	: IGameObject(position, size, 0.f, layer, EObjectType::Bullet)
	, m_explosionSize(explosionSize)
	, m_explosionOffset((m_explosionSize - m_size))
	, m_pSprite_top(Resources::ResourceManager::getSprite("bullet_Top"))
	, m_pSprite_bottom(Resources::ResourceManager::getSprite("bullet_Bottom"))
	, m_pSprite_left(Resources::ResourceManager::getSprite("bullet_Left"))
	, m_pSprite_right(Resources::ResourceManager::getSprite("bullet_Right"))
	, m_pSprite_explosion(Resources::ResourceManager::getSprite("explosion"))
	, m_spriteAnimator_explosion(m_pSprite_explosion)
	, m_eOrientation(EOrientation::Top)
	, m_isActive(false)
	, m_maxVelocityTank(velocity)
	, m_isExplosion(false)
	, m_pGameManager(pGameManager)
	, m_canDestroyBetton(false)
{
	m_velocity = 0;
	auto onCollisionCallback = [&](const IGameObject& object, const Physics::ECollisionDirection direction)
	{
		//m_pGameManager->playSound(AudioManager::EAudioType::Explosion);
		setVelocity(0);
		m_isExplosion = true;
		m_explosionTimer.start(m_spriteAnimator_explosion.getTotalDuration());
		
		//Bullet::onCollisionCallback(object, direction);
	};
		

	m_colliders.emplace_back(glm::vec2(0), m_size, onCollisionCallback);

	m_explosionTimer.setCallback([&]()
		{
			destroy();

		}
	);

}
void Bullet::render() const 
{
	if (m_isActive)
	{
		if (m_isExplosion)
		{
			switch (m_eOrientation)
			{
			case Bullet::EOrientation::Top:
				m_pSprite_explosion->render(glm::vec2(m_position.x  - m_explosionOffset.x/2, m_position.y + m_size.y / 4.f), m_explosionSize, m_rotation, m_layer + 1.f, m_spriteAnimator_explosion.getCurrentFrame());
				break;
			case Bullet::EOrientation::Bottom:
				m_pSprite_explosion->render(glm::vec2(m_position.x - m_explosionOffset.x/2, m_position.y - m_size.y ), m_explosionSize, m_rotation, m_layer + 1.f, m_spriteAnimator_explosion.getCurrentFrame());
				break;
			case Bullet::EOrientation::Left:
				m_pSprite_explosion->render(glm::vec2(m_position.x - m_explosionOffset.x, m_position.y - m_size.y / 3.f), m_explosionSize, m_rotation, m_layer + 1.f, m_spriteAnimator_explosion.getCurrentFrame());
				break;
			case Bullet::EOrientation::Right:
				m_pSprite_explosion->render(glm::vec2(m_position.x + m_explosionOffset.x / 2, m_position.y - m_size.y / 3.f), m_explosionSize, m_rotation, m_layer + 1.f, m_spriteAnimator_explosion.getCurrentFrame());
				break;
			}
		}
		else if(m_isActive)
		{
			switch (m_eOrientation)
			{
			case Bullet::EOrientation::Top:
				m_pSprite_top->render(m_position, m_size, m_rotation, m_layer);
				break;
			case Bullet::EOrientation::Bottom:
				m_pSprite_bottom->render(m_position, m_size, m_rotation, m_layer);
				break;
			case Bullet::EOrientation::Left:
				m_pSprite_left->render(m_position, m_size, m_rotation, m_layer);
				break;
			case Bullet::EOrientation::Right:
				m_pSprite_right->render(m_position, m_size, m_rotation, m_layer);
				break;
			}
		}
	}
}

void Bullet::update(const double delta)
{
	if (m_isExplosion)
	{
		m_spriteAnimator_explosion.update(delta);
		m_explosionTimer.update(delta);
	}
}

void Bullet::fire(const glm::vec2& position, const glm::vec2& direction)
{
	m_position = position;
	m_direction = direction;
	if (m_direction.x == 0.f)
	{
		m_eOrientation = (m_direction.y < 0) ? EOrientation::Bottom : EOrientation::Top;
	}
	else
	{
		m_eOrientation = (m_direction.x < 0) ? EOrientation::Left : EOrientation::Right;
	}
	m_isActive = true;
	setVelocity(m_maxVelocityTank);

}

void Bullet::onCollisionCallback(const IGameObject& object, const Physics::ECollisionDirection direction)
{
	if (object.getObjectType() == IGameObject::EObjectType::Tank)
	{
		//std::cout << "Bullet got tank!\n";
	}
}

void Bullet::onCollision(IGameObject& object)
{
	if (object.getObjectType() == IGameObject::EObjectType::Bullet)
	{
		//m_isActive = false;
		m_isExplosion = true;
		m_velocity = 0;
		m_explosionTimer.start(m_spriteAnimator_explosion.getTotalDuration());
	}

	if (object.getObjectType() == IGameObject::EObjectType::Tank && dynamic_cast<Tank*>(&object)->m_isEnemyTank && dynamic_cast<Tank*>(m_pOwner)->m_isEnemyTank)
	{
		destroy();
	}

}


void Bullet::destroy()
{
	Physics::PhysicsEngine::nullifyDyanmicObject(this);
	m_isExplosion = false;
	m_isActive = false;
	m_spriteAnimator_explosion.reset();
}