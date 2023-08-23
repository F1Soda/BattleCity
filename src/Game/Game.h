#pragma once

#include <array>
#include <glm/vec2.hpp>
#include "../System/Timer.h"
#include "GameManager.h"


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
	void startNewLevel(const size_t level, const EGameMode eGameMode, Tank::ETankType typeTank1 = Tank::ETankType::Player1Yellow_type1, Tank::ETankType typeTank2 = Tank::ETankType::EnemyGreen_type1, int begingLifesTank1 = 3, int beginigLifesTank2 = 3);
	void setWindowSize(const glm::uvec2 windowSize);
	void updateViewport();
	void nextLevel(const EGameMode eGameMode, Tank* pTank1, Tank* pTank2 = nullptr);
	void setStartScreen();
	void gameOver(glm::vec2& scaleScreen = glm::vec2(0));
	void restart(Tank* pTank1, Tank* pTank2 = nullptr);

	static bool lighting;
	static GLFWwindow* pWindow;
	void reduceCountTankPlayers(int count = 1);
	void reduceCountTankEnemy(int count = 1);
	//int getCurrentCountTankEnemies() const { return m_currentCountEnemyTanks; }
	void pauseWhenChangingSizeOrPosionWindow();
	GameManager* getGameManager() { return m_pGameManager.get(); }
	void exit();

private:

	friend class GameManager;

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

	

    size_t m_countLevels;

	std::shared_ptr<IGameState> m_pCurrentGameState;
	std::shared_ptr<RenderEngine::ShaderProgram> m_pSpriteShaderProgram;
	std::shared_ptr<RenderEngine::ShaderProgram> m_pUIShaderProgram;
	size_t m_currentLevelIndex;
	EGameMode m_currentGameMode;

	Timer m_endingLevelTimer;

	std::shared_ptr<GameManager> m_pGameManager;

};