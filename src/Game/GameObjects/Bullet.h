#pragma once

#include <memory>
#include <array>
#include "IGameObject.h"

class GameManager;


namespace RenderEngine
{
	class Sprite;
	class SpriteAnimator;
}

class Timer;

class Bullet : public IGameObject
{
public:

	enum class EOrientation 
	{
		Top,
		Bottom,
		Left, 
		Right,
		Idle
	};

	Bullet(const double velocity, const glm::vec2& position,
		 glm::vec2& size, glm::vec2& explosionSize ,const float layer, GameManager* pGameManager);
	virtual void render() const override;
	void update(const double delta) override;
	bool isActive() const { return m_isActive; }
	bool isExplosioning() const { return m_isExplosion; }
	void fire(const glm::vec2& position, const glm::vec2& direction);
	void onCollision(IGameObject& object) override;
	bool canDestroyBetton() const { return m_canDestroyBetton; }
	void setCanDestroyBetton(bool var) { m_canDestroyBetton = var; }
private:

	void onCollisionCallback(const IGameObject& object, const Physics::ECollisionDirection direction);

	glm::vec2 m_explosionSize;
	glm::vec2 m_explosionOffset;
	EOrientation m_eOrientation;
	std::shared_ptr<RenderEngine::Sprite> m_pSprite_top;
	std::shared_ptr<RenderEngine::Sprite> m_pSprite_bottom;
	std::shared_ptr<RenderEngine::Sprite> m_pSprite_left;
	std::shared_ptr<RenderEngine::Sprite> m_pSprite_right;
	std::shared_ptr<RenderEngine::Sprite> m_pSprite_explosion;
	RenderEngine::SpriteAnimator m_spriteAnimator_explosion;
	Timer m_explosionTimer;
	bool m_isActive;
	double m_maxVelocityTank;
	bool m_canDestroyBetton;
	bool m_isExplosion;
	GameManager* m_pGameManager;
};