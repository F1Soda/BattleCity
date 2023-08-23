#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include "IGameObject.h"
#include "../../Renderer/spriteAnimator.h"
#include "../../System/Timer.h"
#include <string>


namespace RenderEngine
{
	class Sprite;
}

class Bullet;
class AIComponent;
class GameManager;

class Tank : public IGameObject
{
public:

    enum class ETankType : uint8_t
    {
        Player1Yellow_type1,
        Player1Yellow_type2,
        Player1Yellow_type3,
        Player1Yellow_type4,

        Player2Green_type1,
        Player2Green_type2,
        Player2Green_type3,
        Player2Green_type4,

        EnemyWhite_type1,
        EnemyWhite_type2,
        EnemyWhite_type3,
        EnemyWhite_type4,

        EnemyGreen_type1,
        EnemyGreen_type2,
        EnemyGreen_type3,
        EnemyGreen_type4,

        EnemyRed_type1,
        EnemyRed_type2,
        EnemyRed_type3,
        EnemyRed_type4
    };


    inline static std::string TankTypeToSpriteString[] =
    {
        "player1_yellow_tank_type1_sprite",
        "player1_yellow_tank_type2_sprite",
        "player1_yellow_tank_type3_sprite",
        "player1_yellow_tank_type4_sprite",

        "player2_green_tank_type1_sprite",
        "player2_green_tank_type2_sprite",
        "player2_green_tank_type3_sprite",
        "player2_green_tank_type4_sprite",

        "enemy_white_tank_type1_sprite",
        "enemy_white_tank_type2_sprite",
        "enemy_white_tank_type3_sprite",
        "enemy_white_tank_type4_sprite",

        "enemy_green_tank_type1_sprite",
        "enemy_green_tank_type2_sprite",
        "enemy_green_tank_type3_sprite",
        "enemy_green_tank_type4_sprite",

        "enemy_red_tank_type1_sprite",
        "enemy_red_tank_type2_sprite",
        "enemy_red_tank_type3_sprite",
        "enemy_red_tank_type4_sprite"
    };

	enum class EOrientation
	{
        Left = 0,
		Top=1,
        Right = 2,
		Bottom= 3,
		Idle=4
	};

    Tank(const Tank::ETankType eType,
        Level* pLevel,
        const bool bHasAI,
        const bool bShieldOnSpawn,
        const EOrientation m_eOrientation,
        const double maxVelocityTank,
        const double maxVelocityBullet,
        const glm::vec2& position,
        const glm::vec2& size,
        const float layer,
        GameManager* gameManager,
        const bool isBonusTank,
        int beginingLifes = 3);

	void render() const override;
	void setOrientation(const EOrientation eOrientation);
	void update(const double delta) override;
	double getMaxVelocity() const { return m_maxVelocityTank; }
	void setVelocity(const double velocity) override;
	void fire();
    //void setHeadOnCollision(bool val);
    Tank::EOrientation getOrintationTank() const { return m_eOrientation; }
    void onCollision(IGameObject& object) override;

    bool canDrive() const { return m_canDrive; }

    void destroy();

    std::shared_ptr<Bullet> getCurrentBullet() const { return m_pCurrentBullet1; }

    Tank::ETankType getTankType() const { return m_type; }
    int getCountLeftLifes() const { return m_lifes; }
    void reduceCountLifes();
    void addLife() { m_lifes++; }
    const bool m_isEnemyTank;
    bool canDestroyBetton() const { return m_canDestroyBetton; }

    void activateShield(double duration);
    void levelUp();
    void respawn();
    void setTankType(ETankType type);
    bool isDestroyed() const { return m_isDestroyed; }
    bool isSpawning() const { return m_isSpawning; }
private:

	EOrientation m_eOrientation;

	std::shared_ptr<Bullet> m_pCurrentBullet1;
    std::shared_ptr<Bullet> m_pCurrentBullet2;
    Level* m_pLevel;
    GameManager* m_pGameManager;

    Tank::ETankType m_type;

	std::shared_ptr<RenderEngine::Sprite> m_pSprite_top;
	std::shared_ptr<RenderEngine::Sprite> m_pSprite_bottom;
	std::shared_ptr<RenderEngine::Sprite> m_pSprite_left;
	std::shared_ptr<RenderEngine::Sprite> m_pSprite_right;
	RenderEngine::SpriteAnimator m_spriteAnimator_top;
	RenderEngine::SpriteAnimator m_spriteAnimator_bottom;
	RenderEngine::SpriteAnimator m_spriteAnimator_left;
	RenderEngine::SpriteAnimator m_spriteAnimator_right;

	std::shared_ptr<RenderEngine::Sprite> m_pSprite_respawn;
	RenderEngine::SpriteAnimator m_spriteAnimator_respawn;

	std::shared_ptr<RenderEngine::Sprite> m_pSprite_shield;
	RenderEngine::SpriteAnimator m_spriteAnimator_shield;

    std::shared_ptr<RenderEngine::Sprite> m_pSprite_bigExplosionLeftTop;
    std::shared_ptr<RenderEngine::Sprite> m_pSprite_bigExplosionRightTop;
    std::shared_ptr<RenderEngine::Sprite> m_pSprite_bigExplosionLeftBottom;
    std::shared_ptr<RenderEngine::Sprite> m_pSprite_bigExplosionRightBottom;
    RenderEngine::SpriteAnimator m_spriteAnimator_bigExplosionLeftTop;
    RenderEngine::SpriteAnimator m_spriteAnimator_bigExplosionRightTop;
    RenderEngine::SpriteAnimator m_spriteAnimator_bigExplosionLeftBottom;
    RenderEngine::SpriteAnimator m_spriteAnimator_bigExplosionRightBottom;

	Timer m_respawnTimer;
	Timer m_shieldTimer;
    Timer m_explosionTimer;
    Timer m_iceDriceTimer;
    Timer m_twiceShootTimer;
    mutable Timer m_changeColorTimer;

    double m_timerBetweenTwiceShoot = 300;
    double m_timeBetweenChangingColor = 100;

	double m_maxVelocityTank;
    double m_maxVelocityBullet;
	bool m_isSpawning;
	bool m_hasShield;
    bool m_bShieldOnSpawn;
    bool m_canDrive;
    bool m_canFire;
    bool m_canDestroyBetton;
    bool m_isBonusTank;
    mutable bool m_changeColorToRed;
    mutable  bool m_changeColorToGreen;

    std::vector<std::shared_ptr<RenderEngine::Sprite>> m_redSprites;
    std::vector<std::shared_ptr<RenderEngine::Sprite>> m_greenSprites;

    bool m_isDestroyed;

    std::unique_ptr<AIComponent> m_pAIComponent;

    int m_lifes;

    static const std::string& getTankSpriteFromType(const ETankType eTankType);
 
};