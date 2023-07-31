#pragma once

#include <array>
#include <glm/vec2.hpp>

class Tank;
class Level;

class Game
{
public:
	Game(const glm::ivec2& windowSize);
	~Game();

	void render();
	void update(const uint64_t delta);
	void setKey(const int key,const int action);
	bool init();

	static bool lighting;

private:
	std::array<bool, 349> m_keys;

	enum class EGameState
	{
		Active,
		Pause
	};
	glm::ivec2 m_windowSize;
	EGameState m_eCurrentGameSTate;
	std::unique_ptr<Tank> m_pTank;
	std::unique_ptr<Level> m_pLevel;

};