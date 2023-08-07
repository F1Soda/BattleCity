#include "GameObjects/Tank.h"
#include "AIComponent.h"

AIComponent::EEnemyState AIComponent::m_eEnemyState;





AIComponent::AIComponent(Tank* pParrentTank)
	: m_pParentTank(pParrentTank)
	, durationBetweenStepAI(1000)
{

}

void AIComponent::update(const double delta)
{
	static double currentFrameTime = 0;

	if (currentFrameTime <= durationBetweenButtonsClicking)
		currentFrameTime += delta;
	else
	{
		switch (m_eEnemyState)
		{
		case AIComponent::EEnemyState::Ñhaotic:

			break;
		case AIComponent::EEnemyState::PursuePlayers:
			break;
		case AIComponent::EEnemyState::PursueEagle:
			break;
		}
	}
	

	m_pParentTank->fire();
}