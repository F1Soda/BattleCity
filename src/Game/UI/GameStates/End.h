#pragma once

#include <memory>
#include <vector>
# include <string>
#include <glm/vec2.hpp>
#include "../../../Renderer/spriteAnimator.h"
#include "IGameState.h"
#include "../../../System/Timer.h"

namespace RenderEngine
{
	class Sprite;
}

class Game;
class GameManager;

class End : public IGameState
{
public:
	/*static constexpr unsigned int BLOCK_SIZE = 8;
	static constexpr unsigned int STARTSCREEN_WIDTH = 15 * BLOCK_SIZE * 2;
	static constexpr unsigned int STARTSCREEN_HEIGHT = 14 * BLOCK_SIZE * 2;
	static constexpr unsigned int MENU_WIDTH = 128;
	static constexpr unsigned int MENU_HEIGHT = 64;
	static constexpr unsigned int TANK_SIZE = BLOCK_SIZE*2;*/

	End(GameManager* pGameManager, glm::vec2 scaleWindow);

	virtual void render() const override;
	virtual void update(const double delta) override;
	virtual void processInput(std::array<char, 349>& keys) override;
	virtual unsigned int getStateWidth() const override;
	virtual unsigned int getStateHeight() const override;

private:
	int m_currentMenuSelection;
	bool m_keyReleased;
	//RenderEngine::Sprite m_sprite;
	//std::pair<std::shared_ptr<RenderEngine::Sprite>, glm::vec2 > m_menuSprite;
	std::pair<std::shared_ptr<RenderEngine::Sprite>, glm::vec2 > m_mainSprite;
	std::vector<std::pair<std::shared_ptr<RenderEngine::Sprite>, glm::vec2>> m_F1SodaSprites;
	//Timer m_waitOneSecondBeforStartTimer; // При нажаитии enter в окне GameOver 
	//сражу enter срабатывает и в окне StartScreen, потому немного ждем
	//bool canStart = false; // for timer waitOneSecondBeforStartTimer

	GameManager* m_pGameManager;

	double m_sizes_F1SodaHeaders = 30;

	 const int m_endSprite_width = 200;
	 const int m_endSprite_height = 300;
	  
	 Timer m_rotationTimer;
	 const double m_durationBetweenRotatingHeads = 450;
	 bool m_leftRightRotate;

	 float m_musicEndVolume;

};