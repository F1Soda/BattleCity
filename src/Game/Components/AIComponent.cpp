#include "../GameObjects/Tank.h"
#include "AIComponent.h"
#include "../RandomLib/include/effolkronium/random.hpp"

#include <algorithm>
#include "iostream"
#include <cmath>
//#include <queue>

static const bool canFire = true;


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
	, m_mustFindWay(true)
	, m_eEnemyState(AIComponent::EEnemyState::Ñhaotic)
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

	m_findWayTimer.setCallback([&]() {m_mustFindWay = true;});

	m_changeEnemyStateTimer.setCallback([&]() {m_eEnemyState = AIComponent::EEnemyState::PursueEagle; });
	m_changeEnemyStateTimer.start(m_timeChangingEnemyState);

}

void AIComponent::update(const double delta)
{

	m_changeEnemyStateTimer.update(delta);
	m_findWayTimer.update(delta);

	using Random = effolkronium::random_static;
	static double currentFrameTime = 0;
	m_changeDirectionTimer.update(delta);
	std::vector<Tank::EOrientation> freeCeil = std::move(getFreedomTankOrientation(false));
	glm::ivec2 positionTank = m_pParentTank->getCurrentPosition();
	if (m_lastFramePositionTank == glm::ivec2(positionTank.x, positionTank.y) && true)
	{
		m_durationStandingSteal += delta;
		

		if (m_durationStandingSteal >= 1000 && !freeCeil.empty())
		{
			m_durationStandingSteal = 0;
			m_pParentTank->setOrientation(freeCeil[Random::get<int>(0, static_cast<int>(freeCeil.size()) - 1)]); 
		}

		if (m_durationStandingSteal >= 7000)
		{
			m_pParentTank->destroy();
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
	if (Random::get<bool>(1) && canFire)
	{
		m_pParentTank->fire();
	}

	
	positionTank = Physics::PhysicsEngine::alignPositionByPixels(m_pParentTank, 4);
	if (positionTank.x % 16 == 0 && (positionTank.y - Level::BLOCK_SIZE / 2) % 16 == 0 && m_pParentTank->canDrive())
	{

		switch (m_eEnemyState)
		{
		case AIComponent::EEnemyState::Ñhaotic:

			updateObjectInCheckerBox();
			freeCeil = std::move(getFreedomTankOrientation(true));

			if (m_isCollisionedByDirectionMove && !isDirectionOnWall && freeCeil.size() != 0)
			{

				m_pParentTank->setOrientation(freeCeil.at(Random::get(0, static_cast<int>(freeCeil.size()) - 1)));
				m_isCollisionedByDirectionMove = false;

			}
			else if (m_canChangeDirection)
			{
				std::vector<Tank::EOrientation> ceils = std::move(RandomChageDirection(std::move(freeCeil), m_pParentTank->getOrintationTank()));
				if (ceils.size() != 0 && Random::get<bool>(chanceChangeDirection))
				{
					int randVal = Random::get(0, static_cast<int>(ceils.size()) - 1);
					//std::cout << randVal << " " << chanceChangeDirection << std::endl;
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


			break;
		case AIComponent::EEnemyState::PursueEagle:
			//positionTank.y -= Level::BLOCK_SIZE / 2;
			//glm::vec2(currentDynamicObject->getCurrentPosition().x, static_cast<unsigned int>(currentDynamicObject->getCurrentPosition().y / 4.f + 0.5f) * 4.f)
			//positionTank = Physics::PhysicsEngine::alignPositionByPixels(m_pParentTank, 4);
			if (m_instructionNavigation.size() != 0 && !m_mustFindWay)
			{

				glm::ivec2 frontElement = m_instructionNavigation.front();

				if (positionTank == frontElement)
				{
					positionTank = m_pParentTank->getCurrentPosition() = frontElement;
					m_instructionNavigation.pop();

				}
				if (positionTank.x == frontElement.x)
				{
					if (positionTank.y > frontElement.y)
					{
						m_pParentTank->setOrientation(Tank::EOrientation::Bottom);
					}
					else
					{
						m_pParentTank->setOrientation(Tank::EOrientation::Top);
					}
				}
				else if (positionTank.y == frontElement.y)
				{
					if (positionTank.x > frontElement.x)
					{
						m_pParentTank->setOrientation(Tank::EOrientation::Left);
					}
					else
					{
						m_pParentTank->setOrientation(Tank::EOrientation::Right);
					}
				}
				else
				{
					std::cerr << "X or Y axis coordinate don't equal! AIComponent::update()\n";
					std::abort();
				}
			}
			else if (m_mustFindWay)
			{
				getInstructionToGetPosition(glm::vec2(std::round(positionTank.x), std::round(positionTank.y)), m_pLevel->getEaglePos());
				m_mustFindWay = false;
			}
			else
			{
				m_findWayTimer.start(m_durationBetweenFindingWay);
				m_eEnemyState = AIComponent::EEnemyState::Ñhaotic;
				m_changeEnemyStateTimer.start(m_durationBetweenFindingWay);
			}

		}

	}
	//else if ((positionTank.x % 16 != 0  &&  (positionTank.y - Level::BLOCK_SIZE / 2) % 16 == 0) || (positionTank.x % 16 == 0 && (positionTank.y - Level::BLOCK_SIZE / 2) % 16 != 0))
	//{
	//	//m_pParentTank->setVelocity(m_pPare)
	//}


}

void updateFreedomCeilArrayForPusposiong(std::vector<std::pair<Tank::EOrientation, int>>& freedomCeil, glm::vec2& currentPos)
{
	int BLOCK = Level::BLOCK_SIZE;
	IGameObject* object = Physics::PhysicsEngine::getObjectByPos(currentPos - glm::vec2(BLOCK, 0));
	IGameObject::EObjectType objectType = object ? object->getObjectType() : IGameObject::EObjectType::Empty;
	if (objectType != IGameObject::EObjectType::BettonWall && objectType != IGameObject::EObjectType::Water && objectType != IGameObject::EObjectType::Border && objectType != IGameObject::EObjectType::BrickWall)
	{
		freedomCeil[0].first = Tank::EOrientation::Left;
		freedomCeil[0].second = 0;
	}
	else if (objectType == IGameObject::EObjectType::BrickWall)
	{
		freedomCeil[0].first = Tank::EOrientation::Left;
		freedomCeil[0].second = 1;
	}
	else
	{
		freedomCeil[0].first = Tank::EOrientation::Idle;
		freedomCeil[0].second = -1;
	}
	object = Physics::PhysicsEngine::getObjectByPos(currentPos + glm::vec2(0,BLOCK));
	objectType = object ? object->getObjectType() : IGameObject::EObjectType::Empty;
	if (objectType != IGameObject::EObjectType::BettonWall && objectType != IGameObject::EObjectType::Water && objectType != IGameObject::EObjectType::Border && objectType != IGameObject::EObjectType::BrickWall)
	{
		freedomCeil[1].first = Tank::EOrientation::Top;
		freedomCeil[1].second = 0;
	}
	else if (objectType == IGameObject::EObjectType::BrickWall)
	{
		freedomCeil[1].first = Tank::EOrientation::Top;
		freedomCeil[1].second = 1;
	}
	else
	{
		freedomCeil[1].first = Tank::EOrientation::Idle;
		freedomCeil[1].second = -1;
	}
	object = Physics::PhysicsEngine::getObjectByPos(currentPos + glm::vec2(BLOCK, 0));
	objectType = object ? object->getObjectType() : IGameObject::EObjectType::Empty;
	if (objectType != IGameObject::EObjectType::BettonWall && objectType != IGameObject::EObjectType::Water && objectType != IGameObject::EObjectType::Border && objectType != IGameObject::EObjectType::BrickWall)
	{
		freedomCeil[2].first = Tank::EOrientation::Right;
		freedomCeil[2].second = 0;
	}
	else if (objectType == IGameObject::EObjectType::BrickWall)
	{
		freedomCeil[2].first = Tank::EOrientation::Right;
		freedomCeil[2].second = 1;
	}
	else
	{
		freedomCeil[2].first = Tank::EOrientation::Idle;
		freedomCeil[2].second = -1;
	}
	object = Physics::PhysicsEngine::getObjectByPos(currentPos - glm::vec2(0, BLOCK));
	objectType = object ? object->getObjectType() : IGameObject::EObjectType::Empty;
	if (objectType != IGameObject::EObjectType::BettonWall && objectType != IGameObject::EObjectType::Water && objectType != IGameObject::EObjectType::Border && objectType != IGameObject::EObjectType::BrickWall)
	{
		freedomCeil[3].first = Tank::EOrientation::Bottom;
		freedomCeil[3].second = 0;
	}
	else if (objectType == IGameObject::EObjectType::BrickWall)
	{
		freedomCeil[3].first = Tank::EOrientation::Bottom;
		freedomCeil[3].second = 1;
	}
	else
	{
		freedomCeil[3].first = Tank::EOrientation::Idle;
		freedomCeil[3].second = -1;
	}

}

Tank::EOrientation getOrientationByAxisToPurpose(bool for_y_axis, std::vector<std::pair<Tank::EOrientation, int>>& freedomCeil, std::vector<Tank::EOrientation>& probablyOrientation, glm::vec2& posObject, glm::vec2& posSubject, Tank::EOrientation lastOrinetation, int priority)
{
	float difference;
	int index;
	if (for_y_axis)
	{
		difference = posObject.y - posSubject.y;
		index = 1;
	}
	else
	{
		index = 2;
		difference = posObject.x - posSubject.x;
	}
	if (difference > 0)
	{
		if (freedomCeil[(index + 2) % 4].first != Tank::EOrientation::Idle && freedomCeil[(index + 2) % 4].second == priority && freedomCeil[(index + 2) % 4].first != Tank::EOrientation((int(lastOrinetation) + 2) % 4))
		{
			return Tank::EOrientation((index + 2) % 4);
		}
		else if (freedomCeil[index].first != Tank::EOrientation::Idle && freedomCeil[index].second == priority && freedomCeil[index].first != Tank::EOrientation((int(lastOrinetation) + 2) % 4))
		{
			probablyOrientation.push_back(Tank::EOrientation(index));
		}
	}
	else if (difference < 0)
	{
		if (freedomCeil[index].first != Tank::EOrientation::Idle && freedomCeil[index].second == priority && freedomCeil[index].first != Tank::EOrientation((int(lastOrinetation) + 2)%4))
		{
			return Tank::EOrientation(index);
		}
		else if (freedomCeil[(index + 2) % 4].first != Tank::EOrientation::Idle && freedomCeil[(index + 2) % 4].second == priority && freedomCeil[(index + 2) % 4].first != Tank::EOrientation((int(lastOrinetation) + 2) % 4))
		{
			probablyOrientation.push_back(Tank::EOrientation((index + 2) % 4));
		}
	}
	return Tank::EOrientation::Idle;

}

void setNewPositionRay(Tank::EOrientation orientation, glm::vec2& rayPos)
{
	int BLOCK = Level::BLOCK_SIZE;
	switch (orientation)
	{
	case Tank::EOrientation::Left:
		rayPos -= glm::vec2(BLOCK, 0);
		break;
	case Tank::EOrientation::Top:
		rayPos += glm::vec2(0, BLOCK);
		break;
	case Tank::EOrientation::Right:
		rayPos += glm::vec2(BLOCK, 0);
		break;
	case Tank::EOrientation::Bottom:
		rayPos -= glm::vec2(0, BLOCK);
		break;
	}
}

void AIComponent::getInstructionToGetPosition(glm::vec2& posWhere, glm::vec2& currentPos)
{

	//std::cout << "called AIComponent::getInstructionToGetPosition\n";
	using Random = effolkronium::random_static;

	Tank::EOrientation lockedOrientation = Tank::EOrientation::Idle;
	//updateObjectInCheckerBox();
	int BLOCK = Level::BLOCK_SIZE;
	glm::vec2 posEagle = currentPos;
	std::vector<std::pair<Tank::EOrientation, int>> freedomCeil = { {Tank::EOrientation::Idle, -1}, {Tank::EOrientation::Idle, -1} ,{Tank::EOrientation::Idle, -1} ,{Tank::EOrientation::Idle, -1} };
	updateFreedomCeilArrayForPusposiong(freedomCeil, posWhere);

	glm::vec2 rayPos = posWhere;
	Tank::EOrientation currentFreeOrintation;
	std::vector<Tank::EOrientation> probablyOrientation;
	std::vector<glm::vec2> instuction;
	Tank::EOrientation lastFreeOrintation;
	bool getEagle = false;
	instuction.reserve(10);
	for (int i = 0; i < freedomCeil.size(); i++)
	{
		updateFreedomCeilArrayForPusposiong(freedomCeil, posWhere);
		rayPos = posWhere;
		lastFreeOrintation = freedomCeil[i].first;
		if (lastFreeOrintation == Tank::EOrientation::Idle)
		{
			continue;
		}
		lockedOrientation = Tank::EOrientation::Idle;
		setNewPositionRay(lastFreeOrintation, rayPos);
		probablyOrientation.clear();
		
		instuction.clear();

		while (true)
		{
			if (rayPos == posEagle)
			{
				//std::cout << "find way to eagle!\n";
				getEagle = true;
				instuction.push_back(rayPos);
				break;
			}
			updateFreedomCeilArrayForPusposiong(freedomCeil, rayPos);

			if (lockedOrientation != Tank::EOrientation::Idle)
			{
				if (freedomCeil[int(lockedOrientation)].first == lockedOrientation)
				{
					lastFreeOrintation = lockedOrientation;
					instuction.push_back(rayPos);
					lockedOrientation = Tank::EOrientation::Idle;
					setNewPositionRay(lastFreeOrintation, rayPos);
					continue;
				}
				else
				{
					setNewPositionRay(lastFreeOrintation, rayPos);
					continue;
				}
			}


			currentFreeOrintation = Tank::EOrientation::Idle;

			if (currentFreeOrintation == Tank::EOrientation::Idle)
			{
				currentFreeOrintation = getOrientationByAxisToPurpose(true, freedomCeil, probablyOrientation, rayPos, posEagle, lastFreeOrintation, 0);
			}
			if ( currentFreeOrintation == Tank::EOrientation::Idle)
			{
				currentFreeOrintation = getOrientationByAxisToPurpose(false, freedomCeil, probablyOrientation, rayPos, posEagle, lastFreeOrintation, 0);
			}
			if (currentFreeOrintation == Tank::EOrientation::Idle)
			{
				currentFreeOrintation = getOrientationByAxisToPurpose(true, freedomCeil, probablyOrientation, rayPos, posEagle, lastFreeOrintation, 1);
			}
			if (currentFreeOrintation == Tank::EOrientation::Idle)
			{
				currentFreeOrintation = getOrientationByAxisToPurpose(false, freedomCeil, probablyOrientation, rayPos, posEagle, lastFreeOrintation, 1);
			}

			if (currentFreeOrintation == lastFreeOrintation)
			{
				setNewPositionRay(lastFreeOrintation, rayPos);
				continue;
			}
			else if (currentFreeOrintation != Tank::EOrientation::Idle)
			{
				instuction.push_back(rayPos);
				setNewPositionRay(currentFreeOrintation, rayPos);
				lastFreeOrintation = currentFreeOrintation;
			}
			else
			{
				if (probablyOrientation.size() != 0)
				{
					currentFreeOrintation = probablyOrientation[Random::get<int>(0, int(probablyOrientation.size()) - 1)];
					instuction.push_back(rayPos);
					setNewPositionRay(currentFreeOrintation, rayPos);
					if (std::find(instuction.begin(), instuction.end(), rayPos) != instuction.end())
					{
						getEagle = false;
						break;
					}
					lastFreeOrintation = currentFreeOrintation;
					if (currentFreeOrintation == Tank::EOrientation::Left || currentFreeOrintation == Tank::EOrientation::Right)
					{
						if (rayPos.y - posEagle.y > 0)
						{
							lockedOrientation = Tank::EOrientation::Bottom;
						}
						else
						{
							lockedOrientation = Tank::EOrientation::Top;
						}
					}
					else if (currentFreeOrintation == Tank::EOrientation::Bottom || currentFreeOrintation == Tank::EOrientation::Top)
					{
						if (rayPos.x - posEagle.y > 0)
						{
							lockedOrientation = Tank::EOrientation::Left;
						}
						else
						{
							lockedOrientation = Tank::EOrientation::Right;
						}
					}
				}
				else
				{
					getEagle = false;
					break;
				}
			}



		}

		if ((instuction.size() < m_instructionNavigation.size() || m_instructionNavigation.size() == 0) && getEagle)
		{
			getEagle = false;
			while (!m_instructionNavigation.empty())
			{
				m_instructionNavigation.pop();
			}
			for (int i = 0; i < instuction.size(); i++)
			{
				m_instructionNavigation.push(instuction[i]);
			}
		}

	}

	//return m_instructionNavigation();

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
		if (m_checkerBoxs[i] != IGameObject::EObjectType::Empty && m_checkerBoxs[i] != IGameObject::EObjectType::Trees && m_checkerBoxs[i] != IGameObject::EObjectType::Ice && !m_isCollisionedByDirectionMove && i <4)
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

	glm::ivec2 positionTank = m_pParentTank->getCurrentPosition();
	//glm::vec2 windowScaleInPixels = m_pLevel->getWindowSizeInPixels();
	//glm::ivec2 correctedPositionTank = glm::vec2(std::clamp(std::round(positionTank.x - BLOCK_SIZE), 0.f, static_cast<float>(windowScaleInPixels.x)), std::clamp(std::round(windowScaleInPixels.y - positionTank.y) + BLOCK_SIZE / 2, 0.f, static_cast<float>(windowScaleInPixels.y)));

	

	int blockX = 0;
	int blockY = 0;
	IGameObject* object = nullptr;
	int BLOCK = Level::BLOCK_SIZE;
	//glm::vec2 correctedPosition;
	unsigned int index = 0;
	switch (eSubBoxChecker)
	{
	case AIComponent::ESubBoxChecker::Left:
		object = Physics::PhysicsEngine::getObjectByPos(positionTank - glm::ivec2(BLOCK, 0));
		index = Physics::PhysicsEngine::getObjectIndexByPos(positionTank - glm::ivec2(BLOCK, 0));
		break;
	case AIComponent::ESubBoxChecker::Top:
		object = Physics::PhysicsEngine::getObjectByPos(positionTank + glm::ivec2(0, BLOCK));
		index = Physics::PhysicsEngine::getObjectIndexByPos(positionTank + glm::ivec2(0, BLOCK));
		break;
	case AIComponent::ESubBoxChecker::Right:
		object = Physics::PhysicsEngine::getObjectByPos(positionTank + glm::ivec2(BLOCK, 0));
		index = Physics::PhysicsEngine::getObjectIndexByPos(positionTank + glm::ivec2(BLOCK, 0));
		break;
	case AIComponent::ESubBoxChecker::Bottom:
		object = Physics::PhysicsEngine::getObjectByPos(positionTank - glm::ivec2(0, BLOCK));
		index = Physics::PhysicsEngine::getObjectIndexByPos(positionTank - glm::ivec2(0, BLOCK));
		break;
	case AIComponent::ESubBoxChecker::LeftLeft:
		object = Physics::PhysicsEngine::getObjectByPos(positionTank - glm::ivec2(BLOCK * 2, 0));
		index = Physics::PhysicsEngine::getObjectIndexByPos(positionTank - glm::ivec2(BLOCK * 2, 0));
		break;
	case AIComponent::ESubBoxChecker::TopTop:
		object = Physics::PhysicsEngine::getObjectByPos(positionTank - glm::ivec2(BLOCK * 2, 0));
		index = Physics::PhysicsEngine::getObjectIndexByPos(positionTank - glm::ivec2(BLOCK * 2, 0));
		break;
	case AIComponent::ESubBoxChecker::RightRight:
		object = Physics::PhysicsEngine::getObjectByPos(positionTank - glm::ivec2(BLOCK * 2, 0));
		index = Physics::PhysicsEngine::getObjectIndexByPos(positionTank - glm::ivec2(BLOCK * 2, 0));
		break;
	case AIComponent::ESubBoxChecker::BottomBottom:
		object = Physics::PhysicsEngine::getObjectByPos(positionTank - glm::ivec2(BLOCK * 2, 0));
		index = Physics::PhysicsEngine::getObjectIndexByPos(positionTank - glm::ivec2(BLOCK * 2, 0));
		break;
	}
	if (!object)
	{
		return IGameObject::EObjectType::Empty;
		
	}
	else if (object->getObjectType() == IGameObject::EObjectType::Bullet)
	{
		return IGameObject::EObjectType::Empty;
	}

	if (!object->isActive())
	{
		m_pLevel->changeLevelObject(index,nullptr);
		return IGameObject::EObjectType::Empty;
	}
	if (object->getObjectType() == IGameObject::EObjectType::Trees)
	{
		std::cout << "";
	}
	return object->getObjectType();

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

std::vector<Tank::EOrientation> AIComponent::getFreedomTankOrientation( bool addOrientationByChance)
{
	using Random = effolkronium::random_static;

	std::vector<Tank::EOrientation> res = {};
	for (int i = 0; i < m_checkerBoxs.size(); i++)
	{
		checkCheckerBox(res ,static_cast<AIComponent::ESubBoxChecker>(i),m_checkerBoxs[i], addOrientationByChance);
	}
	if (res.size() == 0)
	{
		std::cout << "";
	}
	return res;
}


void AIComponent::checkCheckerBox( std::vector<Tank::EOrientation>& freedomCeils, AIComponent::ESubBoxChecker eSubBoxChecker, IGameObject::EObjectType objectType, bool addOrientationByChance)
{

	if (int(eSubBoxChecker) < 4)
	{
		if (addOrientationByChance)
		{
			using Random = effolkronium::random_static;
			if (objectType == IGameObject::EObjectType::Empty || objectType == IGameObject::EObjectType::Trees || objectType == IGameObject::EObjectType::Ice)
			{
				if (int(m_pParentTank->getOrintationTank()) == (int(eSubBoxChecker) + 2) % 4)
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
					freedomCeils.push_back(static_cast<Tank::EOrientation>(eSubBoxChecker));
				}
			}
		}
		else
		{
			if (objectType == IGameObject::EObjectType::Empty || objectType == IGameObject::EObjectType::Trees || objectType == IGameObject::EObjectType::Ice)
				freedomCeils.push_back(static_cast<Tank::EOrientation>(int(eSubBoxChecker) % 4));
		}
	}
}
