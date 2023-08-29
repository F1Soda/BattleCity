#pragma once 

#include "../Game/GameObjects/Tank.h"
#include <memory>
#include <vector>
//#include "Game.h" // level
#include "AudioManager.h"
#include <glm/vec2.hpp>
class Level;
class IGameObject;
class Game;
class Bonus;
class SelectStage;



class GameManager
{
public:
	GameManager(Game* pGame);

	void reduceCountTankPlayers(Tank* pTank);
	void reduceCountTankEnemy(Tank* enemyTank);
	void setStartScreen();
	//int getCurrentCountTankEnemies() const { return m_pGame->getCurrentCountTankEnemies(); }
	void setCrrentLevel(Level* pLevel);
	bool isLevel() { return m_pCurrentLevel != nullptr; }

	void nextLevel(Tank* pTank1, Tank* pTank2 = nullptr);
	void gameOver();
	void restart(Tank* pTank1, Tank* pTank2 = nullptr);

	void pause();
	glm::vec2 getScaleScreenInPixels() const ;
	glm::vec2 getScaleScreenInBlocks() const ;
	IGameObject* getObjectByIndex(int index) const ;
	void Exit();
	void playSound(AudioManager::EAudioType type) const { m_pAudioManager->playAudio(type); }
	bool m_isGameOver;
	void addBonus();
	void destroyBonus(Bonus* pBonus);

	void bonusAddShieldToTank(Tank* tank, double duration);
	void bonusFreezeTime(double duration);
	void bonusBuildFort(double duration);
	void bonusUpgradeTank(Tank* tank);
	void bonusDestroyAllEnemyTanks();
	void bonusAddLife(Tank* pTank);
	void setFullWindow();
	void setSelectStageScreen(int iGameMode);

	void setSoundOff() { m_pAudioManager->m_isSoundOn = false; }
	void setSoundOn() { m_pAudioManager->m_isSoundOn = true; }
	void setOnOffSound() { m_pAudioManager->m_isSoundOn = !m_pAudioManager->m_isSoundOn; }
	void SetVolumeMusic(AudioManager::EMusicType, float volume);
	void playMusic(AudioManager::EMusicType eType);
	float getVolumeMusic(AudioManager::EMusicType eType);
	void stopMusic(AudioManager::EMusicType eType);
	//void startNewLevel(const size_t level, const Game::EGameMode eGameMode, Tank::ETankType typeTank1 = Tank::ETankType::Player1Yellow_type1, Tank::ETankType typeTank2 = Tank::ETankType::Player2Green_type1, int beginingLifesTank1 = 3, int beginigLifesTank2 = 3);
	void end();

	bool isEndSceen;

private:
	friend class SelectStage;
	Game* m_pGame;
	Level* m_pCurrentLevel;
	std::shared_ptr<AudioManager> m_pAudioManager;
	

	//std::vector<std::weak_ptr> objectsOnLevel;

};