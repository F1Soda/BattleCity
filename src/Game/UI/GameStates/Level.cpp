#include "Level.h"
#include <iostream>
#include "../../GameObjects/BrickWall.h"
#include "../../GameObjects/BetonWall.h"
#include "../../GameObjects/Trees.h"
#include "../../GameObjects/Ice.h"
#include "../../GameObjects/Eagle.h"
#include "../../GameObjects/Water.h"
#include "../../GameObjects/BetonWall.h"
#include "../../GameObjects/Border.h"
#include "../../GameObjects/Tank.h"
#include "../../Game.h"
#include <algorithm>
#include <cmath>
#include "../RandomLib/include/effolkronium/random.hpp"
#include "../UIMenuInLevel.h"
#include "../../../Resources/ResourceManager.h"


#include <GLFW/glfw3.h>


Tank::EOrientation Level::eMoveStateFirstButtonFirstPlayer = Tank::EOrientation::Idle;
Tank::EOrientation Level::eMoveStateSecondButtonFirstPlayer = Tank::EOrientation::Idle;

Tank::EOrientation Level::eMoveStateSecondButtonSecondPlayer = Tank::EOrientation::Idle;
Tank::EOrientation Level::eMoveStateFirstButtonSecondPlayer = Tank::EOrientation::Idle;

static bool button_P_Esc_released = false;
static bool buttonInMenureleased = false;
static const bool playerWithAIComponent = false;
static const int maxCountTanksOnLevel = 4;

std::shared_ptr<IGameObject> Level::createGameObjectFromDiscription(const char description, const glm::vec2& position, const glm::vec2& size, const float rotation)
{
	switch (description)
	{
	case '0':
		return std::make_shared<BrickWall>(BrickWall::EBrickWallType::Right, position, size, rotation, 0.f);

	case '1':
		return std::make_shared<BrickWall>(BrickWall::EBrickWallType::Bottom, position, size, rotation, 0.f);
	case '2':
		return std::make_shared<BrickWall>(BrickWall::EBrickWallType::Left, position, size, rotation, 0.f);
	case '3':
		return std::make_shared<BrickWall>(BrickWall::EBrickWallType::Top, position, size, rotation, 0.f);
	case '4':
		return std::make_shared<BrickWall>(BrickWall::EBrickWallType::All, position, size, rotation, 0.f);
	case 'G':
		return std::make_shared<BrickWall>(BrickWall::EBrickWallType::BottomLeft, position, size, rotation, 0.f);
	case 'H':
		return std::make_shared<BrickWall>(BrickWall::EBrickWallType::BottomRight, position, size, rotation, 0.f);
	case 'I':
		return std::make_shared<BrickWall>(BrickWall::EBrickWallType::TopRight, position, size, rotation, 0.f);
	case 'J':
		return std::make_shared<BrickWall>(BrickWall::EBrickWallType::TopLeft, position, size, rotation, 0.f);
	case '5':
		return std::make_shared<BetonWall>(BetonWall::EBetonWallType::Right, position, size, rotation, 0.f);
	case '6':
		return std::make_shared<BetonWall>(BetonWall::EBetonWallType::Bottom, position, size, rotation, 0.f);
	case '7':
		return std::make_shared<BetonWall>(BetonWall::EBetonWallType::Left, position, size, rotation, 0.f);
	case '8':
		return std::make_shared<BetonWall>(BetonWall::EBetonWallType::Top, position, size, rotation, 0.f);
	case '9':
		return std::make_shared<BetonWall>(BetonWall::EBetonWallType::All, position, size, rotation, 0.f);
	case 'A':
		return std::make_shared<Water>(position, size, rotation, 0.f);
	case 'B':
		return std::make_shared<Trees>(position, size, rotation, 1.f);
	case 'C':
		return std::make_shared<Ice>(position, size, rotation, -1.f);
	case 'E':
		return std::make_shared<Eagle>(position, size, rotation, 0.f, m_pGameManager);
	case 'D':
		return nullptr;
	default:
		std::cerr << "Unknown GameObject description: " << description << std::endl;
	}
	return nullptr;
}


Level::Level(const std::vector<std::string>& levelDescription, std::unordered_map<Tank::ETankType, int> enemiesTypeMap, const Game::EGameMode eGameMode, GameManager* pGameManager)
	: m_eGameMode(eGameMode)
	, m_enemiesTankTypeMap(enemiesTypeMap)
	, m_currentCountPlayersTanks(3)
	, m_currentCountEnemyTanksOnLevel(0)
	, isDestroed(false)
	, isOnPause(false)
	
{
	m_pGameManager = pGameManager;

	if (levelDescription.empty())
	{
		std::cout << "Empty level discription" << std::endl;
	}
	m_endingLevelTimer.setCallback([&]() 
		{
			nextLevel(); 
			isDestroed = true;
		});

	m_widthBlocks = levelDescription[0].length();
	m_heightBlocks = levelDescription.size();

	m_widthPixels = static_cast<unsigned int>(m_widthBlocks * BLOCK_SIZE);
	m_heightPixels = static_cast<unsigned int>(m_heightBlocks * BLOCK_SIZE);

	m_playerSpawner_1 = std::make_shared<Spawner>(glm::vec2(BLOCK_SIZE * (m_widthBlocks * 0.5f - 1), BLOCK_SIZE * 0.5f) , this, m_pGameManager);
	m_playerSpawner_2 = std::make_shared<Spawner>(glm::vec2(BLOCK_SIZE * (m_widthBlocks * 0.5f + 3), BLOCK_SIZE * 0.5f), this, m_pGameManager);
	m_enemySpawner_1  = std::make_shared<Spawner>(glm::vec2(BLOCK_SIZE,						 BLOCK_SIZE * m_heightBlocks - BLOCK_SIZE*0.5f), this, m_pGameManager);
	m_enemySpawner_2  = std::make_shared<Spawner>(glm::vec2(BLOCK_SIZE*(m_widthBlocks *0.5f + 1),     BLOCK_SIZE * m_heightBlocks - BLOCK_SIZE * 0.5f), this, m_pGameManager);
	m_enemySpawner_3  = std::make_shared<Spawner>(glm::vec2(BLOCK_SIZE * m_widthBlocks,              BLOCK_SIZE * m_heightBlocks - BLOCK_SIZE * 0.5f), this, m_pGameManager);

	m_levelObjects.reserve(m_widthBlocks * m_heightBlocks + 4);
	unsigned int currentBottomOffset = static_cast<unsigned int>(BLOCK_SIZE * (m_heightBlocks - 1) + BLOCK_SIZE * 0.5f);
	for (const std::string& currentRow : levelDescription)
	{
		unsigned int currentLeftOffset = BLOCK_SIZE;
		for (const char currentElement : currentRow)
		{
			switch (currentElement)
			{
			case 'K':
				m_playerSpawner_1->setPosition(glm::vec2(currentLeftOffset, currentBottomOffset));
				m_levelObjects.emplace_back(nullptr);
				break;
			case 'L':
				m_playerSpawner_2->setPosition(glm::vec2(currentLeftOffset, currentBottomOffset));
				m_levelObjects.emplace_back(nullptr);
				break;
			case 'M':
				m_enemySpawner_1->setPosition(glm::vec2(currentLeftOffset, currentBottomOffset));
				m_levelObjects.emplace_back(nullptr);
				break;
			case 'N':
				m_enemySpawner_2->setPosition(glm::vec2(currentLeftOffset, currentBottomOffset));
				m_levelObjects.emplace_back(nullptr);
				break;
			case 'O':
				m_enemySpawner_3->setPosition(glm::vec2(currentLeftOffset, currentBottomOffset));
				m_levelObjects.emplace_back(nullptr);
				break;
			default:
				m_levelObjects.emplace_back(createGameObjectFromDiscription(currentElement, glm::vec2(currentLeftOffset, currentBottomOffset), glm::vec2(BLOCK_SIZE, BLOCK_SIZE), 0.0f));
				break;
			}

			

			currentLeftOffset += BLOCK_SIZE;

		}
		currentBottomOffset -= BLOCK_SIZE;
	}
	// bottom border
	m_levelObjects.emplace_back(std::make_shared<Border>( glm::vec2(BLOCK_SIZE, 0.0f), glm::vec2(m_widthBlocks* BLOCK_SIZE, BLOCK_SIZE/2.0f), 0.f, 0.f));

	// top border
	m_levelObjects.emplace_back(std::make_shared<Border>(glm::vec2(BLOCK_SIZE, m_heightBlocks * BLOCK_SIZE + 0.5f * BLOCK_SIZE), glm::vec2(m_widthBlocks * BLOCK_SIZE, BLOCK_SIZE / 2.0f), 0.f, 0.f));
	// left border
	m_levelObjects.emplace_back(std::make_shared<Border>(glm::vec2(0.0f, 0.0f), glm::vec2(BLOCK_SIZE, BLOCK_SIZE*(m_heightBlocks +1)), 0.f, 0.f));
	// right border
	m_levelObjects.emplace_back(std::make_shared<Border>(glm::vec2((m_widthBlocks +1) * BLOCK_SIZE, 0.0f), glm::vec2(BLOCK_SIZE*2.0f, BLOCK_SIZE * (m_heightBlocks + 1)), 0.f, 0.f));

	m_pUIMenuLevel = std::make_shared<UIMenuLevel>(this, Resources::ResourceManager::getShaderProgram("UIShader"));

}

void Level::render() const
{
	


	for (const auto& currentMapObject : m_levelObjects)
	{
		if (currentMapObject)
		{
			currentMapObject->render();
		}
	}

	switch (m_eGameMode)
	{
	case Game::EGameMode::TwoPlayer:
		m_pTank2->render();
		[[fallthrough]]; // преднамеренно не указано break, чтобы компилятор не ругался
	case Game::EGameMode::OnePlayer:
		m_pTank1->render();
	}

	for (const auto& currentTank : m_enemyTanks)
	{
		currentTank->render();
	}
	if (isOnPause)
	{
		m_pUIMenuLevel->render();
	}


}

void Level::update(const double delta)
{

	if (isOnPause)
	{
		return;
	}

	for (const auto& currentMapObject : m_levelObjects)
	{
		if (currentMapObject)
		{
			currentMapObject->update(delta);
		}
	}

	m_endingLevelTimer.update(delta);

	if (isDestroed)
	{
		return;
	}

	switch (m_eGameMode)
	{
	case Game::EGameMode::TwoPlayer:
		m_pTank2->update(delta);
		[[fallthrough]]; // преднамеренно не указано break, чтобы компилятор не ругался
	case Game::EGameMode::OnePlayer:
		m_pTank1->update(delta);
	}

	for (int i = 0; i<m_enemyTanks.size(); i++)
	{
		m_enemyTanks[i].get()->update(delta);
	}

	m_enemyTanks.erase(std::remove(m_enemyTanks.begin(), m_enemyTanks.end(), nullptr), m_enemyTanks.end());



	/*for (int i = 0; i < m_enemyTanks.size(); i++)
	{
		m_enemyTanks[i]->update(delta);
	}*/

	m_enemySpawner_1->update(delta);
	m_enemySpawner_2->update(delta);
	m_enemySpawner_3->update(delta);
	m_playerSpawner_1->update(delta);
	m_playerSpawner_2->update(delta);
	int debCountEnemy = getCountEnemyTank();
	if (m_currentCountEnemyTanksOnLevel < maxCountTanksOnLevel && getCountEnemyTank() >0)
	{
		respawnEnemyTank();
	}
	Physics::PhysicsEngine::update(delta);
}

unsigned int Level::getStateWidth() const
{
	return static_cast<unsigned int>((m_widthBlocks + 3)) * BLOCK_SIZE;
}

unsigned int Level::getStateHeight() const
{
	return static_cast<unsigned int>((m_heightBlocks + 1)) * BLOCK_SIZE;
}

std::vector<std::shared_ptr<IGameObject>> Level::getObjectsInArea(const glm::vec2& bottomLeft, const glm::vec2& topRight)
{
	std::vector<std::shared_ptr<IGameObject>> output;
	output.reserve(9);

	glm::vec2 bottomLeft_converted(std::clamp(bottomLeft.x - BLOCK_SIZE, 0.f, static_cast<float>(m_widthPixels)), std::clamp(m_heightPixels - bottomLeft.y + BLOCK_SIZE /2 ,0.f, static_cast<float>(m_heightPixels)));
	glm::vec2 topRight_converted(std::clamp(topRight.x - BLOCK_SIZE, 0.f, static_cast<float>(m_widthPixels)), std::clamp(m_heightPixels - topRight.y + BLOCK_SIZE / 2, 0.f, static_cast<float>(m_heightPixels)));

	size_t startX = static_cast<size_t>(floor(bottomLeft_converted.x / BLOCK_SIZE));
	size_t endX   = static_cast<size_t>(ceil(topRight_converted.x / BLOCK_SIZE));

	size_t startY = static_cast<size_t>(floor(topRight_converted.y / BLOCK_SIZE));
	size_t endY   = static_cast<size_t>(ceil(bottomLeft_converted.y / BLOCK_SIZE));

	//std::cout << "startX: " << startX << ", endX: " << endX << "," << std::endl << "startY: " << startY << ", endY: " << endY << std::endl;

	//std::cout << "bottomLeft.x: " << bottomLeft_converted.x << ", bottomLeft.y: " << bottomLeft_converted.y << std::endl << ", topRight.x: " << topRight_converted.x << ", toRight.y: " << topRight_converted.y << std::endl;

	for (size_t currentCLoumn = startX; currentCLoumn < endX; ++currentCLoumn)
	{
		for (size_t currentRow = startY; currentRow < endY; ++currentRow)
		{
			auto& currentObject = m_levelObjects[currentRow * m_widthBlocks + currentCLoumn];
			if (currentObject)
			{
				output.push_back(currentObject);
			}
		}
	}

	if (endX >= m_widthBlocks)
		output.push_back(m_levelObjects[m_levelObjects.size() - 1]);
	if (startX <= 1)
		output.push_back(m_levelObjects[m_levelObjects.size() - 2]);
	if (startY <= 1)
		output.push_back(m_levelObjects[m_levelObjects.size() - 3]);
	if (endY >= m_heightBlocks)
		output.push_back(m_levelObjects[m_levelObjects.size() - 4]);


	
	//std::cout << "output.size: " << output.size() << std::endl;

	return output;
}

void Level::processInput(std::array<char, 349>& keys)
{

	if (!keys[GLFW_KEY_P] && !keys[GLFW_KEY_ESCAPE])
	{
		button_P_Esc_released = true;
	}

	if (button_P_Esc_released)
	{
		if (keys[GLFW_KEY_P])
		{
			button_P_Esc_released = false;
			isOnPause = !isOnPause;
		}
		else if (keys[GLFW_KEY_ESCAPE])
		{
			button_P_Esc_released = false;
			isOnPause = !isOnPause;
		}

	}

	if (isOnPause)
	{
		if (!keys[GLFW_KEY_W] && !keys[GLFW_KEY_S] && !keys[GLFW_KEY_ENTER])
		{
			buttonInMenureleased = true;
		}
		if (buttonInMenureleased)
		{
			if (keys[GLFW_KEY_W])
			{
				m_pUIMenuLevel->upLine();
				buttonInMenureleased = false;
			}
			else if (keys[GLFW_KEY_S])
			{
				m_pUIMenuLevel->downLine();
				buttonInMenureleased = false;
			}
			else if (keys[GLFW_KEY_ENTER])
			{
				auto currentPosLine = m_pUIMenuLevel->getCUrrentPositionLine();
				switch (currentPosLine)
				{
				case primitives::Line::ELinePosition::MENU:
					m_pGameManager->setStartScreen();
					break;
				case primitives::Line::ELinePosition::RESTART:
					m_pGameManager->restart();
					break;
				case primitives::Line::ELinePosition::SOUND:
					std::cout << "Not implemented yet =)\n";
					break;
				}
				buttonInMenureleased = false;
			}

		}

		return;
	}




	switch (m_eGameMode)
	{
	case Game::EGameMode::TwoPlayer:
		if (m_pTank2->isActive() && m_pTank2->canDrive())
		{
			CheckButtonStatusSecondPlayer(GLFW_KEY_UP, Tank::EOrientation::Top);
			CheckButtonStatusSecondPlayer(GLFW_KEY_LEFT, Tank::EOrientation::Left);
			CheckButtonStatusSecondPlayer(GLFW_KEY_RIGHT, Tank::EOrientation::Right);
			CheckButtonStatusSecondPlayer(GLFW_KEY_DOWN, Tank::EOrientation::Bottom);

			if (eMoveStateSecondButtonSecondPlayer != Tank::EOrientation::Idle)
			{
				m_pTank2->setOrientation(eMoveStateSecondButtonSecondPlayer);
				m_pTank2->setVelocity(m_pTank2->getMaxVelocity());
			}
			else if (eMoveStateFirstButtonSecondPlayer != Tank::EOrientation::Idle)
			{
				m_pTank2->setVelocity(m_pTank2->getMaxVelocity());
				m_pTank2->setOrientation(eMoveStateFirstButtonSecondPlayer);
			}
			else
			{
				m_pTank2->setVelocity(0);
			}

			if (keys[GLFW_KEY_RIGHT_SHIFT])
			{
				m_pTank2->fire();
			}
		}

		
		[[fallthrough]]; // преднамеренно не указано break, чтобы компилятор не ругался
	case Game::EGameMode::OnePlayer:
		if (m_pTank1->isActive() && m_pTank1->canDrive())
		{
			CheckButtonStatusFirstPlayer(GLFW_KEY_W, Tank::EOrientation::Top);
			CheckButtonStatusFirstPlayer(GLFW_KEY_A, Tank::EOrientation::Left);
			CheckButtonStatusFirstPlayer(GLFW_KEY_D, Tank::EOrientation::Right);
			CheckButtonStatusFirstPlayer(GLFW_KEY_S, Tank::EOrientation::Bottom);

			if (eMoveStateSecondButtonFirstPlayer != Tank::EOrientation::Idle)
			{
				m_pTank1->setOrientation(eMoveStateSecondButtonFirstPlayer);
				m_pTank1->setVelocity(m_pTank1->getMaxVelocity());
			}
			else if (eMoveStateFirstButtonFirstPlayer != Tank::EOrientation::Idle)
			{
				m_pTank1->setVelocity(m_pTank1->getMaxVelocity());
				m_pTank1->setOrientation(eMoveStateFirstButtonFirstPlayer);
			}
			else
			{
				m_pTank1->setVelocity(0);
			}

			if (keys[GLFW_KEY_SPACE])
			{
				m_pTank1->fire();
			}
		}
		
	}
}

void CheckButtonStatusFirstPlayer(int key, Tank::EOrientation eCurrentState)
{
	int state = glfwGetKey(Game::pWindow, key);
	if (state == GLFW_PRESS && Level::eMoveStateFirstButtonFirstPlayer != eCurrentState)
	{
		if (Level::eMoveStateFirstButtonFirstPlayer == Tank::EOrientation::Idle)
		{
			Level::eMoveStateFirstButtonFirstPlayer = eCurrentState;
		}
		else if (Level::eMoveStateFirstButtonFirstPlayer != Tank::EOrientation::Idle)
		{
			Level::eMoveStateSecondButtonFirstPlayer = eCurrentState;
		}
	}
	else if (state == GLFW_RELEASE && Level::eMoveStateFirstButtonFirstPlayer == eCurrentState)
	{
		Level::eMoveStateFirstButtonFirstPlayer = Level::eMoveStateSecondButtonFirstPlayer;
		Level::eMoveStateSecondButtonFirstPlayer = Tank::EOrientation::Idle;
	}
	else if (state == GLFW_RELEASE && Level::eMoveStateSecondButtonFirstPlayer == eCurrentState)
	{
		Level::eMoveStateSecondButtonFirstPlayer = Tank::EOrientation::Idle;
	}
}

void CheckButtonStatusSecondPlayer(int key, Tank::EOrientation eCurrentState)
{
	int state = glfwGetKey(Game::pWindow, key);
	if (state == GLFW_PRESS && Level::eMoveStateFirstButtonSecondPlayer != eCurrentState)
	{
		if (Level::eMoveStateFirstButtonSecondPlayer == Tank::EOrientation::Idle)
		{
			Level::eMoveStateFirstButtonSecondPlayer = eCurrentState;
		}
		else if (Level::eMoveStateFirstButtonSecondPlayer != Tank::EOrientation::Idle)
		{
			Level::eMoveStateSecondButtonSecondPlayer = eCurrentState;
		}
	}
	else if (state == GLFW_RELEASE && Level::eMoveStateFirstButtonSecondPlayer == eCurrentState)
	{
		Level::eMoveStateFirstButtonSecondPlayer = Level::eMoveStateSecondButtonSecondPlayer;
		Level::eMoveStateSecondButtonSecondPlayer = Tank::EOrientation::Idle;
	}
	else if (state == GLFW_RELEASE && Level::eMoveStateSecondButtonSecondPlayer == eCurrentState)
	{
		Level::eMoveStateSecondButtonSecondPlayer = Tank::EOrientation::Idle;
	}
}

void Level::initLevel()
{
	switch (m_eGameMode)
	{
	case Game::EGameMode::TwoPlayer:
		m_pTank2 = m_playerSpawner_2->spawn(Tank::ETankType::Player2Green_type1, false);
		Physics::PhysicsEngine::addDynamicGameObject(m_pTank2);
		[[fallthrough]]; // преднамеренно не указано break, чтобы компилятор не ругался
	case Game::EGameMode::OnePlayer:
		m_pTank1 = m_playerSpawner_1->spawn(Tank::ETankType::Player1Yellow_type1, playerWithAIComponent);
		Physics::PhysicsEngine::addDynamicGameObject(m_pTank1);
		break; 
	
	}
	std::cout << "Enemies:\n";
	for (auto pair : m_enemiesTankTypeMap)
	{
		if (pair.first == Tank::ETankType::EnemyWhite_type1)
			std::cout << "type1, count: " << pair.second << std::endl;
		else if (pair.first == Tank::ETankType::EnemyWhite_type2)
			std::cout << "type2, count: " << pair.second << std::endl;
	}

}

const glm::vec2 Level::getWindowSizeInPixels() const
{
	return glm::vec2(m_widthPixels, m_heightPixels);
	

}
const glm::vec2 Level::getSizeLevelInBlocks() const
{
	return glm::vec2(m_widthBlocks, m_heightBlocks);
}

void Level::changeLevelObject(unsigned int indexToCahnge, std::shared_ptr<IGameObject> newObject)
{
	m_levelObjects[indexToCahnge] = newObject;
}

void Level::addEnemyTank(Tank::ETankType tankType)
{
	using Random = effolkronium::random_static;

	std::vector<Spawner*> freedomSpawners = { m_enemySpawner_1.get(), m_enemySpawner_2.get(), m_enemySpawner_3.get() };
	
	freedomSpawners.erase(std::remove_if(freedomSpawners.begin(), freedomSpawners.end(), [](Spawner* sp) { return sp->isBusy(); }), freedomSpawners.end());

	if (!freedomSpawners.empty())
	{
		int randInd = Random::get<int>(0, int(freedomSpawners.size()-1));
		auto enemyTank = freedomSpawners[randInd]->spawn(tankType, true);
		m_enemyTanks.emplace_back(enemyTank);
		Physics::PhysicsEngine::addDynamicGameObject(enemyTank);
		m_enemiesTankTypeMap[tankType]--;
		m_currentCountEnemyTanksOnLevel++;
	}
}

void Level::destroyEnemyTank(Tank* pTankToDelete)
{
	for (auto it = m_enemyTanks.begin(); it != m_enemyTanks.end(); it++ )
	{
		if (pTankToDelete == (*it).get())
		{
			//std::cout << "Find tank to delete: " << pTankToDelete << std::endl;
			
			//m_enemiesTankTypeMap[pTankToDelete->getTankType()]--;
			std::cout << "Desortoyed enemy tank. Count of enemies tank: " << getCountEnemyTank() << std::endl;
			Physics::PhysicsEngine::removeDynamicGameObject(*it);
			m_currentCountEnemyTanksOnLevel--;
			*it = nullptr;
			auto debug = m_enemiesTankTypeMap[pTankToDelete->getTankType()];
			
			if (getCountEnemyTank() == 0 && m_currentCountEnemyTanksOnLevel == 0)
			{
 				m_endingLevelTimer.setCallback([&]() {nextLevel(); });
				m_endingLevelTimer.start(2000);
				std::cout << "YOU WIN!\n";
			}
			else if (getCountEnemyTank() > 0)
			{
				respawnEnemyTank();
			}
			return;
		}
	}
	std::cerr << "Can't find tank to destroy!";
	std::abort();
}

void Level::destroyPlayerTank(Tank* pTank)
{	
	pTank->reduceCountLifes();
	if (pTank->getCountLeftLifes() == 0)
	{
		if (m_eGameMode == Game::EGameMode::OnePlayer)
		{
			m_endingLevelTimer.setCallback([&]() {m_pGameManager->gameOver(); });
			m_endingLevelTimer.start(2000);
		}
		else
		{
			if (m_pTank1->getCountLeftLifes() == 0 && m_pTank2->getCountLeftLifes() == 0)
			{
				m_endingLevelTimer.setCallback([&]() {m_pGameManager->gameOver(); });
				m_endingLevelTimer.start(2000);
			}
		}
	}
	else if (pTank->getCountLeftLifes() < 0)
	{
		std::cerr << "Something wrong! Player tanks lifes < 0!\n (Level::destroyPlayerTank())";
		std::abort();
	}
	else
	{
		if (m_eGameMode == Game::EGameMode::OnePlayer)
		{
			m_playerSpawner_1->respawn(*pTank);
		}
		else
		{
			if (m_pTank1.get() == pTank)
			{
				m_playerSpawner_1->respawn(*pTank);
			}
			else
			{
				m_playerSpawner_2->respawn(*pTank);
			}
		}
	}
	

	/*m_currentCountPlayersTanks -= i;
	if (m_currentCountPlayersTanks <= 0)
	{
		m_endingLevelTimer.setCallback([&]() {m_pGameManager->gameOver(); });
		m_endingLevelTimer.start(2000);
	}
	else
	{
		m_playerSpawner_1->respawn(*m_pTank1);
	}*/
}


int Level::getCountEnemyTank()
{
	int res = 0;
	for (auto a : m_enemiesTankTypeMap)
	{
		res += a.second;
	}
	return res;
}

void Level::respawnEnemyTank()
{
	auto it = m_enemiesTankTypeMap.find(Tank::ETankType::EnemyWhite_type1);
	if (it != m_enemiesTankTypeMap.end() && it->second != 0)
	{
		addEnemyTank(Tank::ETankType::EnemyWhite_type1);
	}
	else if ((it = m_enemiesTankTypeMap.find(Tank::ETankType::EnemyWhite_type2)) != m_enemiesTankTypeMap.end() && it->second != 0)
	{
		addEnemyTank(Tank::ETankType::EnemyWhite_type2);
	}
	else if ((it = m_enemiesTankTypeMap.find(Tank::ETankType::EnemyWhite_type3)) != m_enemiesTankTypeMap.end() && it->second != 0)
	{
		addEnemyTank(Tank::ETankType::EnemyWhite_type3);
	}
	else if ((it = m_enemiesTankTypeMap.find(Tank::ETankType::EnemyWhite_type4)) != m_enemiesTankTypeMap.end() && it->second != 0)
	{
		addEnemyTank(Tank::ETankType::EnemyWhite_type4);
	}
}

void Level::nextLevel()
{
	m_pGameManager->nextLevel();
}

void Level::setStartScreen()
{
	m_pGameManager->setStartScreen();
	std::cout << "GAME OVER" << std::endl;
}

