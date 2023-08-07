#pragma once

#include <array>
#include <glm/vec2.hpp>


class IGameState;
struct GLFWwindow;

namespace RenderEngine
{
	class ShaderProgram;
}

class Game
{
public:
	enum class EGameMode
	{
		OnePlayer,
		TwoPlayer
	};

	Game(const glm::uvec2& windowSize);
	~Game();

	void render();
	void update(GLFWwindow* pWindow, const double delta);
	void setKey(const int key,const int action);
	bool init();
	unsigned int getCurrentWidth() const;
	unsigned int getCurrentHeight() const;
	void startNewLevel(const size_t level, const EGameMode eGameMode);
	void setWindowSize(const glm::uvec2 windowSize);
	void updateViewport();
	void nextLevel(const EGameMode eGameMode);

	static bool lighting;
	static GLFWwindow* pWindow;

private:
	std::array<char, 349> m_keys;

	enum class EGameState
	{
		StartScreen,
		LevelStart,
		Level,
		Pause, 
		Scores, 
		GameOver
	};
	glm::ivec2 m_windowSize;
	EGameState m_eCurrentGameState;

	std::shared_ptr<IGameState> m_pCurrentGameState;
	std::shared_ptr<RenderEngine::ShaderProgram> m_pSpriteShaderProgram;
	size_t m_currentLevelIndex;

};