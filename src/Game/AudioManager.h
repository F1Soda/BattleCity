#pragma once 
#include <SFML/Audio.hpp>

class GameManager;

class AudioManager
{
public:

	enum class EAudioType
	{
		Win,
		Shoot,
		Explosion,
		GameOver,
		Bonus,
		SelectInMenu,
		NavigateInMenu
	};


	AudioManager(GameManager* pGameManager);
	void playAudio(EAudioType type);
	bool m_isSoundOn;
private:

	GameManager* m_pGameManager;

	sf::SoundBuffer m_bufferSoundWin;
	sf::SoundBuffer m_bufferSoundShoot;
	sf::SoundBuffer m_bufferSoundExplosion;
	sf::SoundBuffer m_bufferSoundGameOver;
	sf::SoundBuffer m_bufferSoundBonus;
	sf::SoundBuffer m_bufferSoundSelectMenu;
	sf::SoundBuffer m_bufferSoundNavigateInMenu;

	sf::Sound m_soundWin;
	sf::Sound m_soundShoot;
	sf::Sound m_soundExplosion;
	sf::Sound m_soundGameOver;
	sf::Sound m_soundBonus;
	sf::Sound m_soundSelectInMenu;
	sf::Sound m_soundNavigatInMenu;

	
};

//f::SoundBuffer buffer;
//if (!buffer.loadFromFile("../res/Audio/Win.wav"))
//return -1;
//sf::Sound sound;
//sound.setBuffer(buffer);
//sound.play();