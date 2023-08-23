#pragma once 

#include "../../System/Timer.h"
#include <memory>
#include "IGameObject.h"

namespace RenderEngine
{
	class Sprite;
}
class GameManager;

class Bonus : public IGameObject
{
public:

	enum class ETypeBonus
	{
		Armor,
		TimeFreeze,
		Shovel,
		Star,
		Granade, 
		DoubleTank
	};

	Bonus(ETypeBonus type, glm::vec2 pos, float layer, GameManager* pGameManager);
	void destroy();
	void update(double duration) override;
	void render() const override;
	void onCollision(IGameObject& object) override;

private:

	ETypeBonus m_typeBonus;

	std::shared_ptr<RenderEngine::Sprite> m_pSprite;

	Timer m_bonusExistingTimer;
	Timer m_flashingTimer;
	
	double m_timeBetweenFlashing = 500;
	double m_durationExistion = 10000;
	double m_durationShield = 10000;
	double m_durationFreeze = 5000;
	double m_durationExistingBetonFord = 10000;

	bool m_renderBonus;

	GameManager* m_pGameManager;
};
