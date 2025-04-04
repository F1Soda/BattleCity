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

class StartScreen : public IGameState
{
public:
	static constexpr unsigned int BLOCK_SIZE = 8;
	static constexpr unsigned int STARTSCREEN_WIDTH = 15 * BLOCK_SIZE * 2;
	static constexpr unsigned int STARTSCREEN_HEIGHT = 14 * BLOCK_SIZE * 2;
	static constexpr unsigned int MENU_WIDTH = 128;
	static constexpr unsigned int MENU_HEIGHT = 40;
	static constexpr unsigned int TANK_SIZE = BLOCK_SIZE*2;


	StartScreen(const std::vector<std::string>& startScreenDescription, Game* pGame);

	virtual void render() const override;
	virtual void update(const double delta) override;
	virtual void processInput(std::array<char, 349>& keys) override;
	virtual unsigned int getStateWidth() const override;
	virtual unsigned int getStateHeight() const override;

private:
	Game* m_pGame;
	int m_currentMenuSelection;
	bool m_keyReleased;
	std::vector<std::pair<std::shared_ptr<RenderEngine::Sprite>, glm::vec2>> m_sprites;
	std::pair<std::shared_ptr<RenderEngine::Sprite>, glm::vec2 > m_menuSprite;
	std::pair<std::shared_ptr<RenderEngine::Sprite>, glm::vec2 > m_tankSprite;
	RenderEngine::SpriteAnimator m_tankSpriteAnimator;
	Timer m_waitOneSecondBeforStartTimer; // ��� �������� enter � ���� GameOver 
	//����� enter ����������� � � ���� StartScreen, ������ ������� ����
	bool canStart = false; // for timer waitOneSecondBeforStartTimer

	GameManager* m_pGameManager;

};