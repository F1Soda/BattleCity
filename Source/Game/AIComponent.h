#pragma once

#include <array>
#include "GameStates/Level.h"
class Tank;

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

	std::string getStringObjectInCheckerBox(unsigned int index);
	std::vector<Tank::EOrientation> getFreedomTankOrientation();

	void update(const double delta);

	static EEnemyState m_eEnemyState;

private:

	Tank* m_pParentTank;
	Level* m_pLevel;
	double durationBetweenStepAI;
	std::array<IGameObject::EObjectType, 8> m_checkerBoxs;

	std::array<IGameObject::EObjectType, 8>& updateObjectInCheckerBox();
	IGameObject::EObjectType getObjectInBox(AIComponent::ESubBoxChecker eSubBoxChecker);

	// Setting chances
	const double chanceChangeDirection = 0.0;
	const double chanceFire = 0.0;
	
};