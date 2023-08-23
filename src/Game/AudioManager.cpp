#include "GameManager.h"
#include "AudioManager.h"
#include <iostream>

void logErrorLadAudio(char* name) {
	std::cerr << "Errror: can't find audio file(" << name << ") -- AudioManager::AudioManager";
}

AudioManager::AudioManager(GameManager* pGameManager)
	: m_pGameManager(pGameManager)
	, m_isSoundOn(true)
{
	if(!m_bufferSoundWin.loadFromFile("../res/Audio/Win.wav")) { logErrorLadAudio("Win"); }
	if (!m_bufferSoundShoot.loadFromFile("../res/Audio/Shoot.wav")) { logErrorLadAudio("Shoot"); }
	if (!m_bufferSoundExplosion.loadFromFile("../res/Audio/Explosion.wav")) { logErrorLadAudio("Explosion"); }
	if (!m_bufferSoundGameOver.loadFromFile("../res/Audio/GameOver.wav")) { logErrorLadAudio("GameOver"); }
	if (!m_bufferSoundBonus.loadFromFile("../res/Audio/Bonus.wav")) { logErrorLadAudio("Bonus"); }
	if (!m_bufferSoundSelectMenu.loadFromFile("../res/Audio/SelectMenu.wav")) { logErrorLadAudio("SelectMenu"); }
	if (!m_bufferSoundBonus.loadFromFile("../res/Audio/NavigateInMenu.wav")) { logErrorLadAudio("NavigateInMenu"); }

	//m_soundWin.play();
	//m_soundWin.setBuffer(m_bufferSoundWin);
	//m_soundShoot.setBuffer(m_bufferSoundShoot);
	//m_soundExplosion.setBuffer(m_bufferSoundExplosion);
	//m_soundGameOver.setBuffer(m_bufferSoundGameOver);
	//m_soundBonus.setBuffer(m_bufferSoundBonus);
	//m_soundSelectInMenu.setBuffer(m_bufferSoundSelectMenu);
	//m_soundNavigatInMenu.setBuffer(m_bufferSoundNavigateInMenu);

}

void AudioManager::playAudio(EAudioType type)
{
	if (!m_isSoundOn) return;

	switch (type)
	{
	case AudioManager::EAudioType::Win:
		m_soundWin.setBuffer(m_bufferSoundWin);
		m_soundWin.play();
		break;
	case AudioManager::EAudioType::Shoot:
		m_soundShoot.setBuffer(m_bufferSoundShoot);
		m_soundShoot.play();
		break;
	case AudioManager::EAudioType::Explosion:
		m_soundExplosion.setBuffer(m_bufferSoundExplosion);
		m_soundExplosion.play();
		break;
	case AudioManager::EAudioType::GameOver:
		m_soundGameOver.setBuffer(m_bufferSoundGameOver);
		m_soundGameOver.play();
		break;
	case AudioManager::EAudioType::Bonus:
		m_soundBonus.setBuffer(m_bufferSoundBonus);
		m_soundBonus.play();
		break;
	case AudioManager::EAudioType::SelectInMenu:
		m_soundSelectInMenu.setBuffer(m_bufferSoundSelectMenu);
		m_soundSelectInMenu.play();
		break;
	case AudioManager::EAudioType::NavigateInMenu:
		m_soundNavigatInMenu.setBuffer(m_bufferSoundBonus);
		m_soundNavigatInMenu.play();
		break;
	}
}


