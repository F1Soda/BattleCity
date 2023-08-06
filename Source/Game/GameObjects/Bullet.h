#pragma once

#include <memory>
#include <array>
#include "IGameObject.h"


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
		 glm::vec2& size, glm::vec2& explosionSize ,const float layer);
	virtual void render() const override;
	void update(const double delta) override;
	bool isActive() const { return m_isActive; }
	void fire(const glm::vec2& position, const glm::vec2& direction);
		
private:

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
	double m_maxVelocity;
	bool m_isExplosion;
};