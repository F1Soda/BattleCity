#pragma once

#include <array>
#include "../UI/GameStates/Level.h"
#include <queue>
class Tank;
class Timer;

class AIComponent
{
public:

	enum class EEnemyState
	{
		Ñhaotic,
		PursuePlayers,
		PursueEagle
	};

	enum class ESubBoxChecker
	{
		Left = 0,
		Top,
		Right,
		Bottom,
		LeftLeft, 
		TopTop,
		RightRight,
		BottomBottom
	};

	AIComponent(Tank* pParrentTank, Level* pLevel);
	bool m_isCollisionedByDirectionMove;
	bool m_canChangeDirection;

	std::string getStringObjectInCheckerBox(unsigned int index);
	std::vector<Tank::EOrientation> AIComponent::getFreedomTankOrientation(bool addOrientationByChance);

	void update(const double delta);

	EEnemyState m_eEnemyState;

private:

	Tank* m_pParentTank;
	glm::ivec2 m_lastFramePositionTank;
	double m_durationStandingSteal;

	Level* m_pLevel;
	double durationBetweenStepAI;
	std::array<IGameObject::EObjectType, 8> m_checkerBoxs;

	std::array<IGameObject::EObjectType, 8>& updateObjectInCheckerBox();
	IGameObject::EObjectType getObjectInBox(AIComponent::ESubBoxChecker eSubBoxChecker);
	std::vector<Tank::EOrientation> AIComponent::RandomChageDirection(std::vector<Tank::EOrientation>& freedomCeil, Tank::EOrientation orientationTank);
	void AIComponent::checkCheckerBox(std::vector<Tank::EOrientation>& freedomCeils, AIComponent::ESubBoxChecker eSubBoxChecker, IGameObject::EObjectType objectType, bool addOrientationByChance);
	Timer m_changeDirectionTimer;
	Timer m_findWayTimer;
	Timer m_changeEnemyStateTimer;

	double m_timeChangingEnemyState = 35000;
	double m_durationBetweenFindingWay = 5000;
	double timeBetweenChangiingDirection;

	bool isDirectionOnWall;

	std::queue<glm::ivec2> m_instructionNavigation;
	void AIComponent::getInstructionToGetPosition(glm::vec2& posWhere, glm::vec2& currentPos);

	// Setting chances
	double chanceChangeDirection = 0.01;
	const double chanceFire = 0.5;
	double changeToReturnBack = 0.05;
	double chanceToTurnOnBrickWall = 0.05;
	bool m_mustFindWay;
};