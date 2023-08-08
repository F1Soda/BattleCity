#include "GameObjects/Tank.h"
#include "AIComponent.h"
#include "../RandomLib/include/effolkronium/random.hpp"
#include "GameStates/Level.h"
#include <algorithm>
#include "iostream"
#include <cmath>

AIComponent::EEnemyState AIComponent::m_eEnemyState;




AIComponent::AIComponent(Tank* pParrentTank, Level* pLevel)
	: m_isCollisionedByDirectionMove(false)
	, m_pParentTank(pParrentTank)
	, m_pLevel(pLevel)
	, durationBetweenStepAI(1000)
{
	m_checkerBoxs.fill(IGameObject::EObjectType::Unknown);
}

void AIComponent::update(const double delta)
{
	using Random = effolkronium::random_static;
	static double currentFrameTime = 0;

	if (currentFrameTime <= durationBetweenStepAI)
		currentFrameTime += delta;
	else
	{
		currentFrameTime = 0;
		switch (m_eEnemyState)
		{
		case AIComponent::EEnemyState::Ñhaotic:
			if (Random::get<bool>(chanceFire))
			{
				m_pParentTank->fire();
			}
			if (Random::get<bool>(chanceChangeDirection))
			{
				//m_pParentTank->setOrientation(static_cast<Tank::EOrientation>(Random::get(0, 3)));
			}
			system("cls");

			updateObjectInCheckerBox();

			for (int i = 0; i < m_checkerBoxs.size(); i++)
			{
				std::cout << getStringObjectInCheckerBox(i) << std::endl;
			}

			if (m_isCollisionedByDirectionMove || Random::get<bool>(chanceChangeDirection))
			{
				std::vector<Tank::EOrientation> freeCeil = getFreedomTankOrientation();
				int randVal = Random::get(0, static_cast<int>(freeCeil.size()) - 1);
				m_pParentTank->setOrientation(freeCeil[randVal]);
				m_isCollisionedByDirectionMove = false;
			}

			break;
		case AIComponent::EEnemyState::PursuePlayers:
			break;
		case AIComponent::EEnemyState::PursueEagle:
			break;
		}
	}
	
}


std::array<IGameObject::EObjectType, 8>& AIComponent::updateObjectInCheckerBox()
{
	for (size_t i = 0; i < m_checkerBoxs.size(); i++)
	{
		m_checkerBoxs[i] = getObjectInBox(static_cast<AIComponent::ESubBoxChecker>(i));
	}

	return m_checkerBoxs;
}

IGameObject::EObjectType AIComponent::getObjectInBox(AIComponent::ESubBoxChecker eSubBoxChecker)
{
	glm::vec2 positionTank = m_pParentTank->getCurrentPosition();
	glm::vec2 windowScaleInPixels = m_pLevel->getWindowSizeInPixels();
	glm::vec2 correctedPositionTank = glm::vec2(std::clamp(positionTank.x - Level::BLOCK_SIZE, 0.f, static_cast<float>(windowScaleInPixels.x)), std::clamp(windowScaleInPixels.y - positionTank.y + Level::BLOCK_SIZE , 0.f, static_cast<float>(windowScaleInPixels.y)));

	float blockX = 0;
	float blockY = 0;


	switch (eSubBoxChecker)
	{
	case AIComponent::ESubBoxChecker::Left:
		correctedPositionTank.x -= Level::BLOCK_SIZE/4;
		blockX = std::floor(correctedPositionTank.x / Level::BLOCK_SIZE);
		blockY = std::floor(correctedPositionTank.y / Level::BLOCK_SIZE);
		break;
	case AIComponent::ESubBoxChecker::Top:
		correctedPositionTank.y -= Level::BLOCK_SIZE *1.25;
		correctedPositionTank.x += Level::BLOCK_SIZE / 2;
		blockX = std::floor(correctedPositionTank.x / Level::BLOCK_SIZE);
		blockY = std::round(correctedPositionTank.y / Level::BLOCK_SIZE);
		break;
	case AIComponent::ESubBoxChecker::Right:
		correctedPositionTank.x += Level::BLOCK_SIZE *1.25;
		blockX = std::floor(correctedPositionTank.x / Level::BLOCK_SIZE);
		blockY = std::floor(correctedPositionTank.y / Level::BLOCK_SIZE);
		break;
	case AIComponent::ESubBoxChecker::Bottom:
		correctedPositionTank.y += Level::BLOCK_SIZE / 2;
		correctedPositionTank.x += Level::BLOCK_SIZE / 2;
		blockX = std::floor(correctedPositionTank.x / Level::BLOCK_SIZE);
		blockY = std::round(correctedPositionTank.y / Level::BLOCK_SIZE);
		break;
	case AIComponent::ESubBoxChecker::LeftLeft:
		correctedPositionTank.x -= Level::BLOCK_SIZE *1.25;
		blockX = std::floor(correctedPositionTank.x / Level::BLOCK_SIZE);
		blockY = std::floor(correctedPositionTank.y / Level::BLOCK_SIZE);
		break;
	case AIComponent::ESubBoxChecker::TopTop:
		correctedPositionTank.y -= Level::BLOCK_SIZE * 2.25;
		correctedPositionTank.x += Level::BLOCK_SIZE / 2;
		blockX = std::floor(correctedPositionTank.x / Level::BLOCK_SIZE);
		blockY = std::round(correctedPositionTank.y / Level::BLOCK_SIZE);
		break;
	case AIComponent::ESubBoxChecker::RightRight:
		correctedPositionTank.x += Level::BLOCK_SIZE * 2.25;
		blockX = std::floor(correctedPositionTank.x / Level::BLOCK_SIZE);
		blockY = std::floor(correctedPositionTank.y / Level::BLOCK_SIZE);
		break;
	case AIComponent::ESubBoxChecker::BottomBottom:
		correctedPositionTank.y += Level::BLOCK_SIZE *1.5;
		correctedPositionTank.x += Level::BLOCK_SIZE / 2;
		blockX = std::floor(correctedPositionTank.x / Level::BLOCK_SIZE);
		blockY = std::round(correctedPositionTank.y / Level::BLOCK_SIZE);
		break;
	}

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
	else if (blockX > m_pLevel->getSizeLevelInBlocks().x)
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
	std::vector<Tank::EOrientation> res = {};
	for (int i = 0; i < m_checkerBoxs.size(); i++)
	{
		int debugInt = i;
		AIComponent::ESubBoxChecker eSubBoxChecker = static_cast<AIComponent::ESubBoxChecker>(i);

		switch (eSubBoxChecker)
		{
		case AIComponent::ESubBoxChecker::Left:
			if (m_checkerBoxs[i] == IGameObject::EObjectType::Empty)
				res.push_back(Tank::EOrientation::Left);
			continue;
		case AIComponent::ESubBoxChecker::LeftLeft:
			if (m_checkerBoxs[i] == IGameObject::EObjectType::Empty)
				res.push_back(Tank::EOrientation::Left);
			continue;
		case AIComponent::ESubBoxChecker::Top:
			if (m_checkerBoxs[i] == IGameObject::EObjectType::Empty)
				res.push_back(Tank::EOrientation::Top);
			continue;
		case AIComponent::ESubBoxChecker::TopTop:
			if (m_checkerBoxs[i] == IGameObject::EObjectType::Empty)
				res.push_back(Tank::EOrientation::Top);
			continue;
		case AIComponent::ESubBoxChecker::Right:
			if (m_checkerBoxs[i] == IGameObject::EObjectType::Empty)
				res.push_back(Tank::EOrientation::Right);
			continue;
		case AIComponent::ESubBoxChecker::RightRight:
			if (m_checkerBoxs[i] == IGameObject::EObjectType::Empty)
				res.push_back(Tank::EOrientation::Right);
			continue;
		case AIComponent::ESubBoxChecker::Bottom:
			if (m_checkerBoxs[i] == IGameObject::EObjectType::Empty)
				res.push_back(Tank::EOrientation::Bottom);
			continue;
		case AIComponent::ESubBoxChecker::BottomBottom:
			if (m_checkerBoxs[i] == IGameObject::EObjectType::Empty)
				res.push_back(Tank::EOrientation::Bottom);
			continue;
		default:
			std::cerr << "Out of switch in AIComponent::getFreedomTankOrientation " << std::endl;
			std::abort();
		}
	}
	std::cerr << "";
	return res;
}