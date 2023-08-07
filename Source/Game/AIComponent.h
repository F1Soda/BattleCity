#pragma once

class Tank;

class AIComponent
{
public:
	AIComponent(Tank* pParrentTank);

	void update(const double delta);

private:
	Tank* m_pParentTank;
};