#include "Tank.h"
#include "../../Renderer/sprite.h"
#include "../../Resources/ResourceManager.h"
#include "Bullet.h"
#include "../Components/AIComponent.h"
#include <iostream>
#include "../Game.h"
#include "../GameManager.h"


static const double timeCantDrivingOnIce = 500;

Tank::Tank(const Tank::ETankType eType,
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
    int beginingLifes
)
    : IGameObject(position, size, 0.f, layer, EObjectType::Tank)
    , m_isEnemyTank(bHasAI)
    , m_eOrientation(m_eOrientation)
    , m_pGameManager(gameManager)
    , m_pLevel(pLevel)
    , m_pSprite_top(Resources::ResourceManager::getSprite(getTankSpriteFromType(eType) + "_top"))
    , m_pSprite_bottom(Resources::ResourceManager::getSprite(getTankSpriteFromType(eType) + "_bottom"))
    , m_pSprite_left(Resources::ResourceManager::getSprite(getTankSpriteFromType(eType) + "_left"))
    , m_pSprite_right(Resources::ResourceManager::getSprite(getTankSpriteFromType(eType) + "_right"))
    , m_spriteAnimator_top(m_pSprite_top)
    , m_spriteAnimator_bottom(m_pSprite_bottom)
    , m_spriteAnimator_left(m_pSprite_left)
    , m_spriteAnimator_right(m_pSprite_right)
    , m_pSprite_respawn(Resources::ResourceManager::getSprite("respawn"))
    , m_spriteAnimator_respawn(m_pSprite_respawn)
    , m_pSprite_shield(Resources::ResourceManager::getSprite("shield"))
    , m_pSprite_bigExplosionLeftTop(Resources::ResourceManager::getSprite("bigExplosion_top_left"))
    , m_pSprite_bigExplosionRightTop(Resources::ResourceManager::getSprite("bigExplosion_top_right"))
    , m_pSprite_bigExplosionLeftBottom(Resources::ResourceManager::getSprite("bigExplosion_bottom_left"))
    , m_pSprite_bigExplosionRightBottom(Resources::ResourceManager::getSprite("bigExplosion_bottom_right"))
    , m_spriteAnimator_bigExplosionLeftTop(m_pSprite_bigExplosionLeftTop)
    , m_spriteAnimator_bigExplosionRightTop(m_pSprite_bigExplosionRightTop)
    , m_spriteAnimator_bigExplosionLeftBottom(m_pSprite_bigExplosionLeftBottom)
    , m_spriteAnimator_bigExplosionRightBottom(m_pSprite_bigExplosionRightBottom)
    , m_spriteAnimator_shield(m_pSprite_shield)
    , m_maxVelocityTank(maxVelocityTank)
    , m_maxVelocityBullet(maxVelocityBullet)
    , m_isSpawning(true)
    , m_hasShield(false)
    , m_bShieldOnSpawn(bShieldOnSpawn)
    , m_isDestroyed(false)
    , m_type(eType)
    , m_canDrive(true)
    , m_canFire(true)
    , m_canDestroyBetton(false)
    , m_isBonusTank(isBonusTank)
    , m_changeColorToGreen(false)
    , m_changeColorToRed(false)
    , m_greenColor(false)
    , m_redColor(false)
    , m_isIceUnderTank(false)
{
    m_pCurrentBullet1 = std::make_shared<Bullet>(m_maxVelocityBullet, m_position + m_size / 4.f, m_size / 2.f, m_size, layer, m_pGameManager);

    if (eType == Tank::ETankType::Player1Yellow_type3 || eType == Tank::ETankType::Player2Green_type3 ||  eType == Tank::ETankType::Player1Yellow_type4 || eType == Tank::ETankType::Player2Green_type4)
    {
        m_pCurrentBullet2 = std::make_shared<Bullet>(m_maxVelocityBullet, m_position + m_size / 4.f, m_size / 2.f, m_size, layer, m_pGameManager);
        m_pCurrentBullet2->setOwner(this);
        //Physics::PhysicsEngine::addDynamicGameObject(m_pCurrentBullet2);
    }
    if (eType == Tank::ETankType::Player1Yellow_type4 || eType == Tank::ETankType::Player2Green_type4)
    {
        m_canDestroyBetton = true;
        m_pCurrentBullet1->setCanDestroyBetton(true);
        m_pCurrentBullet2->setCanDestroyBetton(true);
    }

    m_twiceShootTimer.setCallback([&]() { m_canFire = true; });


   

    m_iceDriveTimer.setCallback([&]() {
        m_canDrive = true;
        m_velocity = 0;
        if (m_isEnemyTank)
        {
            m_velocity = m_maxVelocityTank;
        }
        });

    setOrientation(m_eOrientation);
    m_isActive = false;
    m_respawnTimer.setCallback([&]()
        {
            m_isSpawning = false;
            m_isActive = true;
            if (m_pAIComponent)
            {
                m_velocity = m_maxVelocityTank;
            }
            if (m_bShieldOnSpawn)
            {
                m_hasShield = true;
                m_shieldTimer.start(2000);
            }
        });

    m_respawnTimer.start(Level::DURATION_RESPAWNING);

    m_shieldTimer.setCallback([&]()
    {
        m_hasShield = false;
    });

    m_explosionTimer.setCallback([&]()
        {
            if (m_isEnemyTank)
            {
                m_pGameManager->reduceCountTankEnemy(this);
            }
            else
            {
                m_pGameManager->reduceCountTankPlayers(this);
                m_isDestroyed = false;
                m_isActive = false;
            }
            
        }
    );

    m_pCurrentBullet1->setOwner(this);

    auto onCollisionCallback = [&](const IGameObject& object, const Physics::ECollisionDirection direction)
        {
            if (object.getCurrentPosition() == getCurrentPosition())
            {
                std::cerr << "Tank on static object, call Tank::destroy()\n";
                destroy();
                
            }

            else if (object.getObjectType() == IGameObject::EObjectType::Ice && canDrive())
            {
                m_canDrive = false;
                m_iceDriveTimer.start(timeCantDrivingOnIce);
                m_isIceUnderTank = true;
                if (m_isEnemyTank)
                {
                    m_velocity = m_maxVelocityTank / 2;
                }
            }
            else
            {
                m_isIceUnderTank = false;
                
            }

        };

    m_colliders.emplace_back(glm::vec2(0), m_size, onCollisionCallback);
    //Physics::PhysicsEngine::addDynamicGameObject(m_pCurrentBullet1);
    if (bHasAI)
    {
        m_pAIComponent = std::make_unique<AIComponent>(this, m_pLevel);
    }

    if (m_isEnemyTank)
    {
        if (eType == Tank::ETankType::EnemyGreen_type4 || eType == Tank::ETankType::EnemyRed_type4 || eType == Tank::ETankType::EnemyWhite_type4)
        {
            m_lifes = 4;
        }
        else
        {
            m_lifes = 1;
        }
    }
    else
    {
        if (beginingLifes < 3)
        {
            //std::cerr << "ERROR: begining life player less then 3! beginingLife = " << beginingLifes << std::endl;
            //std::abort();
            m_lifes = 3;
        }
        else
        {
            m_lifes = beginingLifes;
        }
        std::cout << "Players1 lifes = " << m_lifes << std::endl;
    }

    


    if (m_isBonusTank)
    {
        m_redSprites.reserve(4);
        switch (eType)
        {
        case Tank::ETankType::EnemyWhite_type1:
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type1_sprite_left"));
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type1_sprite_top"));
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type1_sprite_right"));
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type1_sprite_bottom"));
            
            break;
        case Tank::ETankType::EnemyWhite_type2:
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type2_sprite_left"));
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type2_sprite_top"));
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type2_sprite_right"));
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type2_sprite_bottom"));
            break;
        case Tank::ETankType::EnemyWhite_type3:
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type3_sprite_left"));
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type3_sprite_top"));
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type3_sprite_right"));
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type3_sprite_bottom"));
            break;
        case Tank::ETankType::EnemyWhite_type4:
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type4_sprite_left"));
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type4_sprite_top"));
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type4_sprite_right"));
            m_redSprites.push_back(Resources::ResourceManager::getSprite("enemy_red_tank_type4_sprite_bottom"));
            break;
        default:
            std::cerr << "Wrong tnakType! (Tank::Tank())\n";
            std::abort();
            break;
        }

        m_changeColorToRed = true;
        m_changeColorToGreen = false;

        if (!m_greenSprites.empty()) m_greenSprites.clear();

        m_changeColorTimer.setCallback([&]() {m_redColor = !m_redColor; m_changeColorTimer.start(m_timeBetweenChangingColor); });
        m_changeColorTimer.start(m_timeBetweenChangingColor);


    }

    if (m_lifes > 1 && m_isEnemyTank && !m_changeColorToRed)
    {
        m_greenSprites.reserve(4);
        m_greenSprites.push_back(Resources::ResourceManager::getSprite("enemy_green_tank_type4_sprite_left"));
        m_greenSprites.push_back(Resources::ResourceManager::getSprite("enemy_green_tank_type4_sprite_top"));
        m_greenSprites.push_back(Resources::ResourceManager::getSprite("enemy_green_tank_type4_sprite_right"));
        m_greenSprites.push_back(Resources::ResourceManager::getSprite("enemy_green_tank_type4_sprite_bottom"));

        m_changeColorToGreen = true;
        m_changeColorTimer.setCallback([&]() {m_greenColor = !m_greenColor; m_changeColorTimer.start(m_timeBetweenChangingColor); });
        m_changeColorTimer.start(m_timeBetweenChangingColor);
    }

    if (m_pLevel->isFreeze())
    {
        m_velocity = 0;
    }


}

void Tank::render() const
{
    if (m_isSpawning)
    {
        m_pSprite_respawn->render(m_position, m_size, m_rotation, m_layer, m_spriteAnimator_respawn.getCurrentFrame());
    }
    else if (m_isDestroyed)
    {
        m_pSprite_bigExplosionLeftTop->render(glm::vec2(m_position.x - Level::BLOCK_SIZE/2, m_position.y + Level::BLOCK_SIZE / 2), m_size, m_rotation, m_layer + 1.1f, m_spriteAnimator_bigExplosionLeftTop.getCurrentFrame());
        m_pSprite_bigExplosionRightTop->render(m_position + glm::vec2(Level::BLOCK_SIZE / 2), m_size, m_rotation, static_cast<float>(m_layer + 1.1f), m_spriteAnimator_bigExplosionRightTop.getCurrentFrame());
        m_pSprite_bigExplosionLeftBottom->render(m_position - glm::vec2(Level::BLOCK_SIZE / 2), m_size, m_rotation, m_layer + 1.1f, m_spriteAnimator_bigExplosionLeftBottom.getCurrentFrame());
        m_pSprite_bigExplosionRightBottom->render(glm::vec2(m_position.x + Level::BLOCK_SIZE / 2, m_position.y - Level::BLOCK_SIZE / 2), m_size, m_rotation, m_layer + 1.1f, m_spriteAnimator_bigExplosionRightBottom.getCurrentFrame());
    }
    else if (m_isActive)
    {
        switch (m_eOrientation)
        {
        case Tank::EOrientation::Top:
            if (m_isBonusTank && m_redColor)
            {
                //m_changeColorToRed = false;
                m_redSprites.at(int(Tank::EOrientation::Top))->render(m_position, m_size, m_rotation, m_layer,
                    m_spriteAnimator_top.getCurrentFrame());
            }
            else if (m_greenColor)
            {
                m_greenSprites.at(int(Tank::EOrientation::Top))->render(m_position, m_size, m_rotation, m_layer,
                    m_spriteAnimator_top.getCurrentFrame());
            }

            else
            {
                m_pSprite_top->render(m_position, m_size, m_rotation, m_layer, m_spriteAnimator_top.getCurrentFrame());
            }
            break;
        case Tank::EOrientation::Bottom:
            if (m_isBonusTank && m_redColor)
            {
                //m_changeColorTimer.start(m_timeBetweenChangingColor);
                //m_changeColorToRed = false;
                m_redSprites.at(int(Tank::EOrientation::Bottom))->render(m_position, m_size, m_rotation, m_layer,
                    m_spriteAnimator_bottom.getCurrentFrame());
            }
            else if (m_greenColor)
            {
                m_greenSprites[int(Tank::EOrientation::Bottom)]->render(m_position, m_size, m_rotation, m_layer,
                    m_spriteAnimator_bottom.getCurrentFrame());
            }
            else
            {
                m_pSprite_bottom->render(m_position, m_size, m_rotation, m_layer, m_spriteAnimator_bottom.getCurrentFrame());
            }
            break;
        case Tank::EOrientation::Left:
            if (m_isBonusTank && m_redColor)
            {
                //m_changeColorTimer.start(m_timeBetweenChangingColor);
                //m_changeColorToRed = false;
                m_redSprites.at(int(Tank::EOrientation::Left))->render(m_position, m_size, m_rotation, m_layer,
                    m_spriteAnimator_left.getCurrentFrame());
            }
            else if (m_greenColor)
            {
                m_greenSprites[int(Tank::EOrientation::Left)]->render(m_position, m_size, m_rotation, m_layer,
                    m_spriteAnimator_left.getCurrentFrame());
            }
            else
            {
                m_pSprite_left->render(m_position, m_size, m_rotation, m_layer, m_spriteAnimator_left.getCurrentFrame());
            }
            break;
        case Tank::EOrientation::Right:
            if (m_isBonusTank && m_redColor)
            {
                //m_changeColorTimer.start(m_timeBetweenChangingColor);
                //m_changeColorToRed = false;
                m_redSprites.at(int(Tank::EOrientation::Right))->render(m_position, m_size, m_rotation, m_layer,
                    m_spriteAnimator_right.getCurrentFrame());
            }
            else if (m_greenColor)
            {
                m_greenSprites[int(Tank::EOrientation::Right)]->render(m_position, m_size, m_rotation, m_layer,
                    m_spriteAnimator_right.getCurrentFrame());
            }
            else
            {
                m_pSprite_right->render(m_position, m_size, m_rotation, m_layer, m_spriteAnimator_right.getCurrentFrame());
            }
            break;
        }

        if (m_hasShield)
        {
            m_pSprite_shield->render(glm::vec2(m_position.x - 0.5f, m_position.y - 1), m_size, m_rotation, m_layer + 0.1f, m_spriteAnimator_shield.getCurrentFrame());
        }

    }

    if (m_pCurrentBullet1->isActive())
    {
        m_pCurrentBullet1->render();
    }
    if (m_pCurrentBullet2 && m_pCurrentBullet2->isActive())
    {
        m_pCurrentBullet2->render();
    }

}

void Tank::setOrientation(const EOrientation eOrientation)
{

    m_eOrientation = eOrientation;
    switch (m_eOrientation)
    {
    case Tank::EOrientation::Top:
        m_direction.x = 0.f;
        m_direction.y = 1.f;
        break;
    case Tank::EOrientation::Bottom:
        m_direction.x = 0.f;
        m_direction.y = -1.f;
        break;
    case Tank::EOrientation::Left:
        m_direction.x = -1.f;
        m_direction.y = 0.f;
        break;
    case Tank::EOrientation::Right:
        m_direction.x = 1.f;
        m_direction.y = 0.f;
        break;
    default:
        break;
    }
}

void Tank::update(const double delta)
{
    bool isFreeze = m_pLevel->isFreeze();

    if (isFreeze && m_isEnemyTank)
    {
        m_velocity = 0;
    }
    m_shieldTimer.update(delta);
    m_iceDriveTimer.update(delta);

    m_twiceShootTimer.update(delta);

    m_changeColorTimer.update(delta);

    glm::ivec2 windowSize = m_pLevel->getWindowSizeInPixels();

    if ((m_position.x<0 || m_position.x > windowSize.x || m_position.y<0 || m_position.y > windowSize.y) && !m_isDestroyed && m_isActive)
    {
        std::cout << "Tank out of bounds level. Call Tank::destroy()\n";
        m_isBonusTank = false;
        destroy();
    }

    if (m_pCurrentBullet1->isActive() && (  (!m_isEnemyTank) || (m_isEnemyTank && !isFreeze) ) )
    {
        m_pCurrentBullet1->update(delta);
        if (m_pCurrentBullet1->getCurrentVelocity() != 0)
        {
            glm::vec2 positionBullet = m_pCurrentBullet1->getCurrentPosition();
            if (positionBullet.y < 0 || positionBullet.y > windowSize.y
                || positionBullet.x < 0 || positionBullet.x > windowSize.x + 10 && !m_pCurrentBullet1->isExplosioning())
            {
                std::cout << "Bullet out of bounds level. Call Bullet::onCOllisionCallback()\n";
                m_pCurrentBullet1->getColliders()[0].onCollisionCallback(*m_pCurrentBullet1.get(), Physics::ECollisionDirection::Bottom);
            }
                
        }

    }

   

    if (m_pCurrentBullet2 && m_pCurrentBullet2->isActive())
    {
        m_pCurrentBullet2->update(delta);
        if (m_pCurrentBullet2->getCurrentVelocity() != 0)
        {
            glm::vec2 positionBullet = m_pCurrentBullet2->getCurrentPosition();
            if (positionBullet.y < 0 || positionBullet.y > windowSize.y
                || positionBullet.x < 0 || positionBullet.x >windowSize.x + 10 && !m_pCurrentBullet2->isExplosioning())
            {
                std::cout << "Bullet out of bounds level. Call Bullet::onCOllisionCallback()\n";
                m_pCurrentBullet2->getColliders()[0].onCollisionCallback(*m_pCurrentBullet2.get(), Physics::ECollisionDirection::Bottom);
            }

        }

    }

    if (m_isSpawning)
    {
        m_spriteAnimator_respawn.update(delta);
        m_respawnTimer.update(delta);
    }
   
    else if (m_isDestroyed)
    {
        
        m_spriteAnimator_bigExplosionLeftTop.update(delta);
        m_spriteAnimator_bigExplosionLeftBottom.update(delta);
        m_spriteAnimator_bigExplosionRightTop.update(delta);
        m_spriteAnimator_bigExplosionRightBottom.update(delta);
        m_explosionTimer.update(delta);
    }

    else
    {
        if (m_pAIComponent && !isFreeze)
        {
            m_pAIComponent->update(delta);
        }

        if (m_hasShield)
        {
            m_spriteAnimator_shield.update(delta);
            
        }

        if (m_velocity > 0 && ((!m_isEnemyTank) || (m_isEnemyTank && !isFreeze)))
        {

            switch (m_eOrientation)
            {
            case Tank::EOrientation::Top:
                m_spriteAnimator_top.update(delta);
                break;
            case Tank::EOrientation::Bottom:
                m_spriteAnimator_bottom.update(delta);
                break;
            case Tank::EOrientation::Left:
                m_spriteAnimator_left.update(delta);
                break;
            case Tank::EOrientation::Right:
                m_spriteAnimator_right.update(delta);
                break;
            default:
                break;
            }
        }
    }
}

void Tank::setVelocity(const double velocity)
{
    if (!m_isSpawning)
    {
        m_velocity = velocity;
    }
}

void Tank::fire()
{
    if (m_isSpawning || !m_canFire) return;

    if (!m_pCurrentBullet1->isActive())
    {
        m_canFire = false;
        m_twiceShootTimer.start(m_timerBetweenTwiceShoot);
        m_pGameManager->playSound(AudioManager::EAudioType::Shoot);
        Physics::PhysicsEngine::addDynamicGameObject(m_pCurrentBullet1);
        m_pCurrentBullet1->fire(m_position + m_size / 4.f + m_size * m_direction/4.f, m_direction);
    }
    else if (m_pCurrentBullet2 && !m_pCurrentBullet2->isActive())
    {
        m_canFire = false;
        m_twiceShootTimer.start(m_timerBetweenTwiceShoot);
        m_pGameManager->playSound(AudioManager::EAudioType::Shoot);
        Physics::PhysicsEngine::addDynamicGameObject(m_pCurrentBullet2);
        m_pCurrentBullet2->fire(m_position + m_size / 4.f + m_size * m_direction / 4.f, m_direction);
    }
}

const std::string& Tank::getTankSpriteFromType(const ETankType eTankType)
{
    return TankTypeToSpriteString[static_cast<size_t>(eTankType)];
}


void Tank::onCollision(IGameObject& object)
{
    if (object.getObjectType() == IGameObject::EObjectType::Bullet )
    {
        
        object.getColliders()[0].onCollisionCallback(*this, Physics::ECollisionDirection::Bottom);


        Tank* tankOwner = dynamic_cast<Tank*>(object.getOwner());
        if (tankOwner->m_isEnemyTank && this->m_isEnemyTank)
            return;
        if (!tankOwner->m_isEnemyTank && !this->m_isEnemyTank)
            return;

        

        if (!m_isSpawning && !m_hasShield)
        {
            if (m_isEnemyTank)
            {
                reduceCountLifes();
                if (m_lifes == 1 && !m_changeColorToRed )
                {
                    m_changeColorToGreen = false;
                    m_changeColorTimer.stop();
                    m_greenColor = false;
                }
                    

                if (m_lifes > 0)
                    return;
                else
                    destroy();
            }
            else
            {
                destroy();
            }
        }
       
    }
}

void Tank::destroy()
{
    m_velocity = 0;
    m_isActive = false;
    m_isDestroyed = true;
    m_explosionTimer.start(m_spriteAnimator_bigExplosionLeftBottom.getTotalDuration());
    m_pGameManager->playSound(AudioManager::EAudioType::Explosion);
    if (m_isBonusTank)
    {
        m_pGameManager->addBonus();
    }
}

void Tank::respawn()
{
    m_isDestroyed = false;
    m_isActive = false;
    m_respawnTimer.start(1500);
    m_isSpawning = true;
}

void Tank::activateShield(double duration)
{
    m_hasShield = true;
    m_shieldTimer.start(duration);
}

void Tank::levelUp()
{
    switch (m_type)
    {
    case Tank::ETankType::Player1Yellow_type1:
        setTankType(Tank::ETankType::Player1Yellow_type2);
        m_type = Tank::ETankType::Player1Yellow_type2;
        break;
    case Tank::ETankType::Player1Yellow_type2:
        setTankType(Tank::ETankType::Player1Yellow_type3);
        m_type = Tank::ETankType::Player1Yellow_type3;
        break;
    case Tank::ETankType::Player1Yellow_type3:
        setTankType(Tank::ETankType::Player1Yellow_type4);
        m_type = Tank::ETankType::Player1Yellow_type4;
        break;
    case Tank::ETankType::Player1Yellow_type4:
        break;
    case Tank::ETankType::Player2Green_type1:
        setTankType(Tank::ETankType::Player2Green_type2);
        m_type = Tank::ETankType::Player2Green_type2;
        break;
    case Tank::ETankType::Player2Green_type2:
        setTankType(Tank::ETankType::Player2Green_type3);
        m_type = Tank::ETankType::Player2Green_type3;
        break;
    case Tank::ETankType::Player2Green_type3:
        setTankType(Tank::ETankType::Player2Green_type4);
        m_type = Tank::ETankType::Player2Green_type4;
        break;
    case Tank::ETankType::Player2Green_type4:
        break;
    }
}


void Tank::levelDown()
{
    switch (m_type)
    {
    case Tank::ETankType::Player1Yellow_type1:
        break;
    case Tank::ETankType::Player1Yellow_type2:
        setTankType(Tank::ETankType::Player1Yellow_type1);
        m_type = Tank::ETankType::Player1Yellow_type1;
        break;
    case Tank::ETankType::Player1Yellow_type3:
        setTankType(Tank::ETankType::Player1Yellow_type2);
        m_type = Tank::ETankType::Player1Yellow_type2;
        break;
    case Tank::ETankType::Player1Yellow_type4:
        setTankType(Tank::ETankType::Player1Yellow_type3);
        m_type = Tank::ETankType::Player1Yellow_type3;
        break;
    case Tank::ETankType::Player2Green_type1:
        break;
    case Tank::ETankType::Player2Green_type2:
        setTankType(Tank::ETankType::Player2Green_type1);
        m_type = Tank::ETankType::Player2Green_type1;
        break;
    case Tank::ETankType::Player2Green_type3:
        setTankType(Tank::ETankType::Player2Green_type2);
        m_type = Tank::ETankType::Player2Green_type2;
        break;
    case Tank::ETankType::Player2Green_type4:
        setTankType(Tank::ETankType::Player2Green_type3);
        m_type = Tank::ETankType::Player2Green_type3;
        break;
    }
}


void Tank::setTankType(Tank::ETankType type)
{
    Physics::PhysicsEngine::nullifyDyanmicObject(m_pCurrentBullet2.get());
    m_pCurrentBullet2 = nullptr;
    m_pCurrentBullet1->setCanDestroyBetton(false);
    m_maxVelocityBullet = Spawner::MAX_VELOCITY_PLAYER_BULLET_TYPE1;
    m_maxVelocityTank = Spawner::MAX_VELOCITY_PLAYER_TANK_TYPE1;
    setVelocity(m_maxVelocityTank);
    //m_pCurrentBullet1->setVelocity(0);
    switch (type)
    {
    case Tank::ETankType::Player1Yellow_type1:
        m_pSprite_left = Resources::ResourceManager::getSprite("player1_yellow_tank_type1_sprite_left");
        m_pSprite_top = Resources::ResourceManager::getSprite("player1_yellow_tank_type1_sprite_top");
        m_pSprite_right = Resources::ResourceManager::getSprite("player1_yellow_tank_type1_sprite_right");
        m_pSprite_bottom = Resources::ResourceManager::getSprite("player1_yellow_tank_type1_sprite_bottom");
        m_type = Tank::ETankType::Player1Yellow_type1;
        break;
    case Tank::ETankType::Player1Yellow_type2:
        m_maxVelocityBullet = Spawner::MAX_VELOCITY_PLAYER_BULLET_TYPE2;
        m_maxVelocityTank = Spawner::MAX_VELOCITY_PLAYER_TANK_TYPE2;
        m_pSprite_left = Resources::ResourceManager::getSprite("player1_yellow_tank_type2_sprite_left");
        m_pSprite_top = Resources::ResourceManager::getSprite("player1_yellow_tank_type2_sprite_top");
        m_pSprite_right = Resources::ResourceManager::getSprite("player1_yellow_tank_type2_sprite_right");
        m_pSprite_bottom = Resources::ResourceManager::getSprite("player1_yellow_tank_type2_sprite_bottom");
        m_type = Tank::ETankType::Player1Yellow_type2;
        break;
    case Tank::ETankType::Player1Yellow_type3:
        m_maxVelocityBullet = Spawner::MAX_VELOCITY_PLAYER_BULLET_TYPE3;
        m_maxVelocityTank = Spawner::MAX_VELOCITY_PLAYER_TANK_TYPE3;
        m_pCurrentBullet2 = std::make_shared<Bullet>(m_maxVelocityBullet, m_position + m_size / 4.f, m_size / 2.f, m_size, m_layer, m_pGameManager);
        m_pCurrentBullet2->setOwner(this);
        m_pSprite_left = Resources::ResourceManager::getSprite("player1_yellow_tank_type3_sprite_left");
        m_pSprite_top = Resources::ResourceManager::getSprite("player1_yellow_tank_type3_sprite_top");
        m_pSprite_right = Resources::ResourceManager::getSprite("player1_yellow_tank_type3_sprite_right");
        m_pSprite_bottom = Resources::ResourceManager::getSprite("player1_yellow_tank_type3_sprite_bottom");
        m_type = Tank::ETankType::Player1Yellow_type3;
        break;
    case Tank::ETankType::Player1Yellow_type4:
        m_maxVelocityBullet = Spawner::MAX_VELOCITY_PLAYER_BULLET_TYPE4;
        m_maxVelocityTank = Spawner::MAX_VELOCITY_PLAYER_TANK_TYPE4;
        m_pCurrentBullet2 = std::make_shared<Bullet>(m_maxVelocityBullet, m_position + m_size / 4.f, m_size / 2.f, m_size, m_layer, m_pGameManager);
        m_pCurrentBullet2->setOwner(this);
        m_canDestroyBetton = true;
        m_pCurrentBullet1->setCanDestroyBetton(true);
        m_pCurrentBullet2->setCanDestroyBetton(true);
        m_pSprite_left = Resources::ResourceManager::getSprite("player1_yellow_tank_type4_sprite_left");
        m_pSprite_top = Resources::ResourceManager::getSprite("player1_yellow_tank_type4_sprite_top");
        m_pSprite_right = Resources::ResourceManager::getSprite("player1_yellow_tank_type4_sprite_right");
        m_pSprite_bottom = Resources::ResourceManager::getSprite("player1_yellow_tank_type4_sprite_bottom");
        m_type = Tank::ETankType::Player1Yellow_type4;
        break;
    case Tank::ETankType::Player2Green_type1:
        m_maxVelocityBullet = Spawner::MAX_VELOCITY_PLAYER_BULLET_TYPE1;
        m_maxVelocityTank = Spawner::MAX_VELOCITY_PLAYER_TANK_TYPE1;
        m_pSprite_left = Resources::ResourceManager::getSprite("player2_green_tank_type1_sprite_left");
        m_pSprite_top = Resources::ResourceManager::getSprite("player2_green_tank_type1_sprite_top");
        m_pSprite_right = Resources::ResourceManager::getSprite("player2_green_tank_type1_sprite_right");
        m_pSprite_bottom = Resources::ResourceManager::getSprite("player2_green_tank_type1_sprite_bottom");
        m_type = Tank::ETankType::Player2Green_type1;
        break;
    case Tank::ETankType::Player2Green_type2:
        m_maxVelocityBullet = Spawner::MAX_VELOCITY_PLAYER_BULLET_TYPE2;
        m_maxVelocityTank = Spawner::MAX_VELOCITY_PLAYER_TANK_TYPE2;
        m_pSprite_left = Resources::ResourceManager::getSprite("player2_green_tank_type2_sprite_left");
        m_pSprite_top = Resources::ResourceManager::getSprite("player2_green_tank_type2_sprite_top");
        m_pSprite_right = Resources::ResourceManager::getSprite("player2_green_tank_type2_sprite_right");
        m_pSprite_bottom = Resources::ResourceManager::getSprite("player2_green_tank_type2_sprite_bottom");
        m_type = Tank::ETankType::Player2Green_type2;
        break;
    case Tank::ETankType::Player2Green_type3:
        m_maxVelocityBullet = Spawner::MAX_VELOCITY_PLAYER_BULLET_TYPE3;
        m_maxVelocityTank = Spawner::MAX_VELOCITY_PLAYER_TANK_TYPE3;
        m_pCurrentBullet2 = std::make_shared<Bullet>(m_maxVelocityBullet, m_position + m_size / 4.f, m_size / 2.f, m_size, m_layer, m_pGameManager);
        m_pCurrentBullet2->setOwner(this);
        m_pSprite_left = Resources::ResourceManager::getSprite("player2_green_tank_type3_sprite_left");
        m_pSprite_top = Resources::ResourceManager::getSprite("player2_green_tank_type3_sprite_top");
        m_pSprite_right = Resources::ResourceManager::getSprite("player2_green_tank_type3_sprite_right");
        m_pSprite_bottom = Resources::ResourceManager::getSprite("player2_green_tank_type3_sprite_bottom");
        m_type = Tank::ETankType::Player2Green_type3;
        break;
    case Tank::ETankType::Player2Green_type4:
        m_maxVelocityBullet = Spawner::MAX_VELOCITY_PLAYER_BULLET_TYPE4;
        m_maxVelocityTank = Spawner::MAX_VELOCITY_PLAYER_TANK_TYPE4;
        m_pCurrentBullet2 = std::make_shared<Bullet>(m_maxVelocityBullet, m_position + m_size / 4.f, m_size / 2.f, m_size, m_layer, m_pGameManager);
        m_pCurrentBullet2->setOwner(this);
        m_canDestroyBetton = true;
        m_pCurrentBullet1->setCanDestroyBetton(true);
        m_pCurrentBullet2->setCanDestroyBetton(true);
        m_pSprite_left = Resources::ResourceManager::getSprite("player2_green_tank_type4_sprite_left");
        m_pSprite_top = Resources::ResourceManager::getSprite("player2_green_tank_type4_sprite_top");
        m_pSprite_right = Resources::ResourceManager::getSprite("player2_green_tank_type4_sprite_right");
        m_pSprite_bottom = Resources::ResourceManager::getSprite("player2_green_tank_type4_sprite_bottom");
        m_type = Tank::ETankType::Player2Green_type4;
        break;
    }
    setVelocity(m_maxVelocityTank);


}


void Tank::reduceCountLifes()
{
    m_lifes--; 
}