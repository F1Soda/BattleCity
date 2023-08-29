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
	if(!m_bufferSoundWin.loadFromFile("res/Audio/Win.wav")) { logErrorLadAudio("Win"); }
	if (!m_bufferSoundShoot.loadFromFile("res/Audio/Shoot.wav")) { logErrorLadAudio("Shoot"); }
	if (!m_bufferSoundExplosion.loadFromFile("res/Audio/Explosion.wav")) { logErrorLadAudio("Explosion"); }
	if (!m_bufferSoundGameOver.loadFromFile("res/Audio/GameOver.wav")) { logErrorLadAudio("GameOver"); }
	if (!m_bufferSoundBonus.loadFromFile("res/Audio/Bonus.wav")) { logErrorLadAudio("Bonus"); }
	if (!m_bufferSoundSelectMenu.loadFromFile("res/Audio/SelectMenu.wav")) { logErrorLadAudio("SelectMenu"); }
	if (!m_bufferSoundNavigateInMenu.loadFromFile("res/Audio/NavigateInMenu.wav")) { logErrorLadAudio("NavigateInMenu"); }
	if (!m_bufferSoundGameStart.loadFromFile("res/Audio/GameStart.wav")) { logErrorLadAudio("Game start"); }

	if (!m_musicInEnd.openFromFile("res/Audio/Silhouette.ogg")) { logErrorLadAudio("music in End"); }

	m_musicEndVolume = 15.f;
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
	float volume = 25.f;

	if (!m_isSoundOn) return;

	switch (type)
	{
	case AudioManager::EAudioType::Win:
		m_soundWin.setBuffer(m_bufferSoundWin);
		m_soundWin.setVolume(volume);
		m_soundWin.play();
		break;
	case AudioManager::EAudioType::Shoot:
		m_soundShoot.setBuffer(m_bufferSoundShoot);
		m_soundShoot.setVolume(volume);
		m_soundShoot.play();
		break;
	case AudioManager::EAudioType::Explosion:
		m_soundExplosion.setBuffer(m_bufferSoundExplosion);
		m_soundExplosion.setVolume(volume);
		m_soundExplosion.play();
		break;
	case AudioManager::EAudioType::GameOver:
		m_soundGameOver.setBuffer(m_bufferSoundGameOver);
		m_soundGameOver.setVolume(volume);
		m_soundGameOver.play();
		break;
	case AudioManager::EAudioType::Bonus:
		m_soundBonus.setBuffer(m_bufferSoundBonus);
		m_soundBonus.setVolume(volume);
		m_soundBonus.play();
		break;
	case AudioManager::EAudioType::SelectInMenu:
		m_soundSelectInMenu.setBuffer(m_bufferSoundSelectMenu);
		m_soundSelectInMenu.setVolume(volume);
		m_soundSelectInMenu.play();
		break;
	case AudioManager::EAudioType::NavigateInMenu:
		m_soundNavigatInMenu.setBuffer(m_bufferSoundNavigateInMenu);
		m_soundNavigatInMenu.setVolume(volume);
		m_soundNavigatInMenu.play();
		break;
	case AudioManager::EAudioType::GameStart:
		m_soundGameStart.setBuffer(m_bufferSoundGameStart);
		m_soundGameStart.setVolume(volume);
		m_soundGameStart.play();
	}
}

void AudioManager::playMusic(EMusicType eType)
{
	switch (eType)
	{
	case AudioManager::EMusicType::End:
		m_musicInEnd.setVolume(m_musicEndVolume);
		//m_musicInEnd.setPlayingOffset(sf::seconds(60));
		m_musicInEnd.play();
		break;
	}
}

void AudioManager::stopMusic(EMusicType eType)
{
	switch (eType)
	{
	case AudioManager::EMusicType::End:
		m_musicInEnd.stop();
		break;
	}
}

void AudioManager::setVolumeMusic(EMusicType eType, float volume)
{
	switch (eType)
	{
	case AudioManager::EMusicType::End:
		m_musicEndVolume = volume;
		m_musicInEnd.setVolume(m_musicEndVolume);
		break;
	}
}


float AudioManager::getVolumeMusic(EMusicType eType)
{
	switch (eType)
	{
	case AudioManager::EMusicType::End:
		return m_musicEndVolume;
	}
	return 0.f;
}


