#pragma once

#include "IGameObject.h"
#include <memory>
#include <array>
#include "../../System/Timer.h"

class GameManager;

namespace RenderEngine
{
	class Sprite;
}

class Eagle : public IGameObject
{
public: 

	enum class EEagleState : uint8_t
	{
		Alive = 0,
		Dead
	};

	Eagle(const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer, GameManager* pGameManager);
	virtual void render() const override;
	virtual void update(const double delta) override;
	virtual void onCollision(IGameObject&) override;
	GameManager* m_pGameManager;


private:
	std::array<std::shared_ptr<RenderEngine::Sprite>,2> m_sprites;
	EEagleState m_eCurrentState;
	Timer gameOverTimer;
};
