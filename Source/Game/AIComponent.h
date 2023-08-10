#pragma once

#include <array>
#include "GameStates/Level.h"
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
	std::vector<Tank::EOrientation> getFreedomTankOrientation();

	void update(const double delta);

	static EEnemyState m_eEnemyState;

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
	void AIComponent::checkCheckerBox(IGameObject::EObjectType objectType, Tank::EOrientation tankOrientation, AIComponent::ESubBoxChecker eSubBoxChecker, std::vector<Tank::EOrientation>& freedomCeils);
	Timer m_changeDirectionTimer;
	double timeBetweenChangiingDirection;

	bool isDirectionOnWall;

	// Setting chances
	double chanceChangeDirection = 0.01;
	const double chanceFire = 0.5;
	double changeToReturnBack = 0.05;
	double chanceToTurnOnBrickWall = 0.1;

};