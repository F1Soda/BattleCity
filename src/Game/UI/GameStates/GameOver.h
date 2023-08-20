#pragma once

#include <vector>
# include <string>
#include <memory>
#include <glm/vec2.hpp>
#include "IGameState.h"
#include "../../../System/Timer.h"

namespace RenderEngine
{
	class Sprite;
}

class Game;

class GameOver : public IGameState
{
public:

	static constexpr unsigned int BLOCK_SIZE = 8;

	GameOver(const std::vector<std::string>& gameOverDescription, Game* pGame, glm::vec2 scaleWindow);

	virtual void render() const override;
	virtual void update(const double delta) override;
	virtual void processInput(std::array<char, 349>& keys) override;
	virtual unsigned int getStateWidth() const override;
	virtual unsigned int getStateHeight() const override;

private:
	Game* m_pGame;
	bool m_keyReleased;
	std::vector<std::pair<std::shared_ptr<RenderEngine::Sprite>, glm::vec2>> m_sprites;
	glm::ivec2 m_scaleScreen;
	Timer m_autoGoToTheStartSCreenTimer;
	void moveText(double delta);
};