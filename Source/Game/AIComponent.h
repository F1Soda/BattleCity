#pragma once

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

	AIComponent(Tank* pParrentTank);

	void update(const double delta);

	static EEnemyState m_eEnemyState;
private:


	Tank* m_pParentTank;
	double durationBetweenStepAI;
	
};