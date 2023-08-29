#pragma once

#include <memory>
#include <vector>
# include <string>
#include <glm/vec2.hpp>
#include "../../../Renderer/spriteAnimator.h"
#include "IGameState.h"
#include "../../../System/Timer.h"
#include "../../Game.h"

namespace RenderEngine
{
	class Sprite;
}
class GameManager;

class SelectStage : public IGameState
{
public:

	SelectStage(GameManager* pGameManager, glm::vec2 screenScale, Game::EGameMode eGameMode);

	virtual void render() const override;
	virtual void update(const double delta) override;
	virtual void processInput(std::array<char, 349>& keys) override;
	virtual unsigned int getStateWidth() const override;
	virtual unsigned int getStateHeight() const override;
	//friend class GameManager;
private:
	//friend class GameManager;

	int m_currentMenuSelection;
	bool m_keyReleased;
	std::vector<std::shared_ptr<RenderEngine::Sprite>> m_spritesStage;
	GameManager* m_pGameManager;
	glm::vec2 m_screenScale;
	glm::vec2 m_stagePos;
	Game::EGameMode m_eGameMode;
	Timer m_waitOneSecondBeforStartTimer; 
	bool m_canStart = false;
};