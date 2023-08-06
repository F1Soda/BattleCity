#pragma once

#include <array>
#include <glm/vec2.hpp>
#include "../Game/GameObjects/Tank.h"


class Level;
struct GLFWwindow;

class Game
{
public:
	Game(const glm::ivec2& windowSize);
	~Game();

	void render();
	void update(GLFWwindow* pWindow, const double delta);
	void setKey(const int key,const int action);
	bool init();
	size_t getCurrentLevelWidth() const;
	size_t getCurrentLevelHeight() const;


	static Tank::EOrientation eMoveStateFirstButton;
	static Tank::EOrientation eMoveStateSecondButton;
	friend void CheckButtonStatus(int key, Tank::EOrientation eCurrentState, GLFWwindow* pWindow);
	
	static bool lighting;

private:
	std::array<char, 349> m_keys;

	enum class EGameState
	{
		Active,
		Pause
	};
	glm::ivec2 m_windowSize;
	EGameState m_eCurrentGameSTate;
	std::shared_ptr<Tank> m_pTank;
	std::shared_ptr<Level> m_pLevel;
};