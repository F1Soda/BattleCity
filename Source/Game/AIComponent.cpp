#include "GameObjects/Tank.h"
#include "AIComponent.h"
#include "../RandomLib/include/effolkronium/random.hpp"
#include "GameStates/Level.h"
#include <algorithm>
#include "iostream"
#include <cmath>

AIComponent::EEnemyState AIComponent::m_eEnemyState;




AIComponent::AIComponent(Tank* pParrentTank, Level* pLevel) 
	: m_lastFramePositionTank(pParrentTank->getCurrentPosition())
	, m_durationStandingSteal(0)
	, m_isCollisionedByDirectionMove(false)
	, m_canChangeDirection(false)
	, m_pParentTank(pParrentTank)
	, m_pLevel(pLevel)
	, durationBetweenStepAI(500)
	, timeBetweenChangiingDirection(3000)
	, isDirectionOnWall(false)
{
	m_checkerBoxs.fill(IGameObject::EObjectType::Unknown);

	m_changeDirectionTimer.setCallback([&]()
		{
			m_canChangeDirection = true;
			isDirectionOnWall = false;
			//m_changeDirectionTimer.start(5000);
		}
	);

	m_changeDirectionTimer.start(timeBetweenChangiingDirection);

}

void AIComponent::update(const double delta)
{
	using Random = effolkronium::random_static;
	static double currentFrameTime = 0;
	m_changeDirectionTimer.update(delta);
	if (m_lastFramePositionTank == glm::ivec2(m_pParentTank->getCurrentPosition().x, m_pParentTank->getCurrentPosition().y))
	{
		m_durationStandingSteal += delta;
		if (m_durationStandingSteal >= 4000)
		{
			m_durationStandingSteal = 0;
			m_pParentTank->setOrientation(Tank::EOrientation((int(m_pParentTank->getOrintationTank()) +1) % 4));
		}
	}
	else
	{
		m_durationStandingSteal = 0;
	}

	m_lastFramePositionTank = m_pParentTank->getCurrentPosition();
	if (currentFrameTime <= durationBetweenStepAI)
		currentFrameTime += delta;
	else
	{
		chanceChangeDirection += 0.01;
		currentFrameTime = 0;
	}
	//glm::ivec2 posTank = {int(m_pParentTank->getCurrentPosition().x), int(m_pParentTank->getCurrentPosition().y)};
	//if (posTank.x % 16 == 0 && posTank.y % 16 == 0)
	
	switch (m_eEnemyState)
	{
	case AIComponent::EEnemyState::Ñhaotic:
		if (Random::get<bool>(1))
		{
			m_pParentTank->fire();
		}

		glm::vec2 positionTank = m_pParentTank->getCurrentPosition();
		glm::vec2 windowScaleInPixels = m_pLevel->getWindowSizeInPixels();
		glm::ivec2 correctedPositionTank = glm::vec2(std::clamp(std::round(positionTank.x - Level::BLOCK_SIZE), 0.f, static_cast<float>(windowScaleInPixels.x)), std::clamp(std::round(windowScaleInPixels.y - positionTank.y) + Level::BLOCK_SIZE / 2, 0.f, static_cast<float>(windowScaleInPixels.y)));

		if (correctedPositionTank.x % 16 == 0 && correctedPositionTank.y % 16 == 0)
		{
			//system("cls");
			//std::cout << "1" << std::endl;
			updateObjectInCheckerBox();

			//for (int i = 0; i < m_checkerBoxs.size(); i++)
			//{
			//	std::cout << getStringObjectInCheckerBox(i) << std::endl;
			//}
			int randVal;
			std::vector<Tank::EOrientation> freeCeil = getFreedomTankOrientation();
			if (m_isCollisionedByDirectionMove && !isDirectionOnWall && freeCeil.size() != 0)
			{
				randVal = Random::get(0, static_cast<int>(freeCeil.size()) - 1);
				m_pParentTank->setOrientation(freeCeil.at(randVal));
				m_isCollisionedByDirectionMove = false;
	
			}
			else if (m_canChangeDirection)
			{
				std::vector<Tank::EOrientation> ceils = std::move(RandomChageDirection(std::move(freeCeil), m_pParentTank->getOrintationTank()));
				if (ceils.size() != 0 && Random::get<bool>(chanceChangeDirection))
				{
					//std::cout << "change direction, chance = "<< chanceChangeDirection << std::endl;
					int randVal = Random::get(0, static_cast<int>(ceils.size()) - 1);
					std::cout << randVal << " " << chanceChangeDirection << std::endl;
					m_pParentTank->setOrientation(ceils[randVal]);
					chanceChangeDirection = 0.1;
					m_canChangeDirection = false;
					m_changeDirectionTimer.start(timeBetweenChangiingDirection);
					if (m_checkerBoxs[int(ceils[randVal])] == IGameObject::EObjectType::BrickWall)
					{
						isDirectionOnWall = true;
					}	
				}
			}
				
		}
		else
		{
			//std::abort();
		}
		break;
		
	}


}

std::vector<Tank::EOrientation> AIComponent::RandomChageDirection(std::vector<Tank::EOrientation>& freedomCeil, Tank::EOrientation orientationTank)
{
	using Random = effolkronium::random_static;

	std::vector<Tank::EOrientation> res;

	for (Tank::EOrientation currentOrientation : freedomCeil)
	{
		if (currentOrientation != orientationTank )
		{
			res.push_back(currentOrientation);
		}
	}

	return res;
}


std::array<IGameObject::EObjectType, 8>& AIComponent::updateObjectInCheckerBox()
{
	for (size_t i = 0; i < m_checkerBoxs.size(); i++)
	{
		m_checkerBoxs[i] = getObjectInBox(static_cast<AIComponent::ESubBoxChecker>(i));
		if (m_checkerBoxs[i] != IGameObject::EObjectType::Empty && !m_isCollisionedByDirectionMove && i <4)
		{
			glm::vec2 directionTank = m_pParentTank->getCurrentDirection();
			if (directionTank.x > 0 && m_pParentTank->getOrintationTank() == Tank::EOrientation::Right)
			{
				m_isCollisionedByDirectionMove = (i == static_cast<int>(AIComponent::ESubBoxChecker::Right));
			}
			else if (directionTank.x < 0 && m_pParentTank->getOrintationTank() == Tank::EOrientation::Left)
			{
				m_isCollisionedByDirectionMove = (i == static_cast<int>(AIComponent::ESubBoxChecker::Left));
			}
			else if (directionTank.y > 0 && m_pParentTank->getOrintationTank() == Tank::EOrientation::Top)
			{
				m_isCollisionedByDirectionMove = (i == static_cast<int>(AIComponent::ESubBoxChecker::Top));
			}
			else if (directionTank.y < 0 && m_pParentTank->getOrintationTank() == Tank::EOrientation::Bottom)
			{
				m_isCollisionedByDirectionMove = (i == static_cast<int>(AIComponent::ESubBoxChecker::Bottom));
			}
		}
	}

	return m_checkerBoxs;
}

IGameObject::EObjectType AIComponent::getObjectInBox(AIComponent::ESubBoxChecker eSubBoxChecker)
{
	int BLOCK_SIZE = static_cast<int>(Level::BLOCK_SIZE);

	glm::vec2 positionTank = m_pParentTank->getCurrentPosition();
	glm::vec2 windowScaleInPixels = m_pLevel->getWindowSizeInPixels();
	glm::ivec2 correctedPositionTank = glm::vec2(std::clamp(std::round(positionTank.x - BLOCK_SIZE), 0.f, static_cast<float>(windowScaleInPixels.x)), std::clamp(std::round(windowScaleInPixels.y - positionTank.y) + BLOCK_SIZE / 2, 0.f, static_cast<float>(windowScaleInPixels.y)));

	

	int blockX = 0;
	int blockY = 0;

	bool debBool;
	int debInt;
	switch (eSubBoxChecker)
	{
	case AIComponent::ESubBoxChecker::Left:
		debInt = correctedPositionTank.x;
		if (correctedPositionTank.x % BLOCK_SIZE != 0)
		{
			return IGameObject::EObjectType::Empty;
		}
		correctedPositionTank.x -= BLOCK_SIZE;
		blockX = correctedPositionTank.x / BLOCK_SIZE;
		blockY = correctedPositionTank.y / BLOCK_SIZE;
		break;
	case AIComponent::ESubBoxChecker::Top:
		//std::cout << correctedPositionTank.y;
		if (correctedPositionTank.y % BLOCK_SIZE != 0)
		{
			return IGameObject::EObjectType::Empty;
		}
		correctedPositionTank.y -= BLOCK_SIZE;
		//correctedPositionTank.x += BLOCK_SIZE / 2;
		blockX = correctedPositionTank.x / BLOCK_SIZE;
		blockY = correctedPositionTank.y / BLOCK_SIZE;
		break;
	case AIComponent::ESubBoxChecker::Right:
		if (correctedPositionTank.x % BLOCK_SIZE != 0)
		{
			return IGameObject::EObjectType::Empty;
		}

		correctedPositionTank.x += BLOCK_SIZE *1;
		blockX = correctedPositionTank.x / BLOCK_SIZE;
		blockY = correctedPositionTank.y / BLOCK_SIZE;
		break;
	case AIComponent::ESubBoxChecker::Bottom:
		if (correctedPositionTank.y % BLOCK_SIZE != 0)
		{
			return IGameObject::EObjectType::Empty;
		}


		correctedPositionTank.y += BLOCK_SIZE;
		correctedPositionTank.x += BLOCK_SIZE/2;
		blockX = (correctedPositionTank.x) / BLOCK_SIZE;
		blockY = correctedPositionTank.y / BLOCK_SIZE;
		break;
	case AIComponent::ESubBoxChecker::LeftLeft:
		if (correctedPositionTank.x % BLOCK_SIZE != 0)
		{
			return IGameObject::EObjectType::Empty;
		}

		correctedPositionTank.x -= BLOCK_SIZE *1;
		debInt = (correctedPositionTank.x) / (BLOCK_SIZE);
		blockX = debInt;
		blockY =correctedPositionTank.y / BLOCK_SIZE;
		break;
	case AIComponent::ESubBoxChecker::TopTop:
		if (correctedPositionTank.y % BLOCK_SIZE != 0)
		{
			return IGameObject::EObjectType::Empty;
		}

		correctedPositionTank.y -= BLOCK_SIZE * 2;
		correctedPositionTank.x += BLOCK_SIZE / 2;
		blockX = correctedPositionTank.x / BLOCK_SIZE;
		blockY = correctedPositionTank.y / BLOCK_SIZE;

		break;
	case AIComponent::ESubBoxChecker::RightRight:
		if (correctedPositionTank.x % BLOCK_SIZE != 0)
		{
			return IGameObject::EObjectType::Empty;
		}

		correctedPositionTank.x += BLOCK_SIZE * 2;
		blockX =correctedPositionTank.x / BLOCK_SIZE;
		blockY = correctedPositionTank.y / BLOCK_SIZE;
		break;
	case AIComponent::ESubBoxChecker::BottomBottom:
		if (correctedPositionTank.y % BLOCK_SIZE != 0)
		{
			return IGameObject::EObjectType::Empty;
		}

		correctedPositionTank.y += BLOCK_SIZE *2;
		correctedPositionTank.x += BLOCK_SIZE / 2;
		blockX = correctedPositionTank.x / BLOCK_SIZE;
		blockY = correctedPositionTank.y / BLOCK_SIZE;
		break;
	}

	//std::cout << " BlockX = " << blockX << ", BlockY = " << blockY << std::endl;

	if (blockX < 0)
	{
		//std::cout << " , " << "Border" << std::endl;
		return IGameObject::EObjectType::Border;
	}
	else if (blockY > m_pLevel->getSizeLevelInBlocks().y)
	{
		//std::cout << " , " << "Border" << std::endl;
		return IGameObject::EObjectType::Border;
	}
	else if (blockY <= 0)
	{
		//std::cout << " , " << "Border" << std::endl;
		return IGameObject::EObjectType::Border;
	}
	else if (blockX > m_pLevel->getSizeLevelInBlocks().x-1)
	{
		//std::cout << " , " << "Border" << std::endl;
		return IGameObject::EObjectType::Border;
	}


	unsigned int index = static_cast<unsigned int>((blockY-1) * m_pLevel->getSizeLevelInBlocks().x + blockX);
	auto& currentObject = m_pLevel->getObjectByIndex(index);

	if (!currentObject)
	{
		return IGameObject::EObjectType::Empty;
		
	}
	else if (currentObject->getObjectType() == IGameObject::EObjectType::Bullet)
	{
		return IGameObject::EObjectType::Empty;
	}

	if (!currentObject->isActive())
	{
		m_pLevel->changeLevelObject(index,nullptr);
		return IGameObject::EObjectType::Empty;
	}
		return currentObject->getObjectType();

}

std::string AIComponent::getStringObjectInCheckerBox(unsigned int index)
{
	switch (m_checkerBoxs[index])
	{
	case IGameObject::EObjectType::BettonWall:
		return "BettonWall";
	case IGameObject::EObjectType::Border:
		return "Border";
	case IGameObject::EObjectType::BrickWall:
		return "BrickWall";
	case IGameObject::EObjectType::Bullet:
		return "Bullet";
	case IGameObject::EObjectType::Eagle:
		return "Eagle";
	case IGameObject::EObjectType::Empty:
		return "Empty";
	case IGameObject::EObjectType::Ice:
		return "Ice";
	case IGameObject::EObjectType::Tank:
		return "Tank";
	case IGameObject::EObjectType::Unknown:
		return "Unknown";
	case IGameObject::EObjectType::Trees:
		return "Trees";
	default:
		return "Unknown";
	}
}

std::vector<Tank::EOrientation> AIComponent::getFreedomTankOrientation()
{
	using Random = effolkronium::random_static;

	std::vector<Tank::EOrientation> res = {};
	for (int i = 0; i < m_checkerBoxs.size(); i++)
	{
		checkCheckerBox(m_checkerBoxs[i], m_pParentTank->getOrintationTank(), static_cast<AIComponent::ESubBoxChecker>(i), res);
	}
	if (res.size() == 0)
	{
		std::cout << "";
	}
	return res;
}

void AIComponent::checkCheckerBox(IGameObject::EObjectType objectType, Tank::EOrientation tankOrientation, AIComponent::ESubBoxChecker eSubBoxChecker, std::vector<Tank::EOrientation>& freedomCeils)
{

	if (int(eSubBoxChecker) < 4)
	{
		Tank::EOrientation val = static_cast<Tank::EOrientation>(eSubBoxChecker);
		using Random = effolkronium::random_static;
		if (objectType == IGameObject::EObjectType::Empty)
		{
			if (int(tankOrientation) == (int(eSubBoxChecker) + 2) % 4)
			{
				if (Random::get<bool>(changeToReturnBack))
					freedomCeils.push_back(static_cast<Tank::EOrientation>(int(eSubBoxChecker) % 4));
			}
			else
				freedomCeils.push_back(static_cast<Tank::EOrientation>(int(eSubBoxChecker) % 4));
		}
		else if (objectType == IGameObject::EObjectType::BrickWall && int(eSubBoxChecker) < 4)
		{
			if (Random::get<bool>(chanceToTurnOnBrickWall))
			{
				freedomCeils.push_back(val);
				//isDirectionOnWall = true;
			}
		}
	}
}
