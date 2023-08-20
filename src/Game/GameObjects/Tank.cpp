#include "Tank.h"
#include "../../Renderer/sprite.h"
#include "../../Resources/ResourceManager.h"
#include "Bullet.h"
#include "../Components/AIComponent.h"
#include <iostream>
#include "../Game.h"
#include "../GameManager.h"

Tank::Tank(const Tank::ETankType eType,
    Level* pLevel,
    const bool bHasAI,
    const bool bShieldOnSpawn,
    const EOrientation m_eOrientation,
    const double maxVelocity,
    const glm::vec2& position,
    const glm::vec2& size,
    const float layer,
    GameManager* gameManager     
    )
    : IGameObject(position, size, 0.f, layer, EObjectType::Tank)
    , m_isEnemyTank(bHasAI)
    , m_eOrientation(m_eOrientation)
    , m_pCurrentBullet(std::make_shared<Bullet>(0.1, m_position + m_size / 4.f, m_size / 2.f, m_size, layer))
    , m_pLevel(pLevel)
    , m_pGameManager(gameManager)
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
    , m_maxVelocity(maxVelocity)
    , m_isSpawning(true)
    , m_hasShield(false)
    , m_bShieldOnSpawn(bShieldOnSpawn)
    , m_health(1)
    , m_isDestroyed(false)
    , m_type(eType)
{
    setOrientation(m_eOrientation);
    m_isActive = false;
    m_respawnTimer.setCallback([&]()
        {
            m_isSpawning = false;
            m_isActive = true;
            if (m_pAIComponent)
            {
                m_velocity = m_maxVelocity;
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
            }
        }
    );

    m_pCurrentBullet->setOwner(this);

    auto onCollisionCallback = [&](const IGameObject& object, const Physics::ECollisionDirection direction)
        {
            if (object.getCurrentPosition() == getCurrentPosition())
            {
                std::cerr << "Tank on static object, call Tank::destroy()\n";
                destroy();
                
            }
        };

    m_colliders.emplace_back(glm::vec2(0), m_size, onCollisionCallback);
    Physics::PhysicsEngine::addDynamicGameObject(m_pCurrentBullet);
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
        m_lifes = 3;
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
        m_pSprite_bigExplosionLeftTop->render(glm::vec2(m_position.x - Level::BLOCK_SIZE/2, m_position.y + Level::BLOCK_SIZE / 2), m_size, m_rotation, m_layer + 0.1f, m_spriteAnimator_bigExplosionLeftTop.getCurrentFrame());
        m_pSprite_bigExplosionRightTop->render(m_position + glm::vec2(Level::BLOCK_SIZE / 2), m_size, m_rotation, static_cast<float>(m_layer + 0.1f), m_spriteAnimator_bigExplosionRightTop.getCurrentFrame());
        m_pSprite_bigExplosionLeftBottom->render(m_position - glm::vec2(Level::BLOCK_SIZE / 2), m_size, m_rotation, m_layer + 0.1f, m_spriteAnimator_bigExplosionLeftBottom.getCurrentFrame());
        m_pSprite_bigExplosionRightBottom->render(glm::vec2(m_position.x + Level::BLOCK_SIZE / 2, m_position.y - Level::BLOCK_SIZE / 2), m_size, m_rotation, m_layer + 0.1f, m_spriteAnimator_bigExplosionRightBottom.getCurrentFrame());
    }
    else
    {
        switch (m_eOrientation)
        {
        case Tank::EOrientation::Top:
            m_pSprite_top->render(m_position, m_size, m_rotation, m_layer, m_spriteAnimator_top.getCurrentFrame());
            break;
        case Tank::EOrientation::Bottom:
            m_pSprite_bottom->render(m_position, m_size, m_rotation, m_layer, m_spriteAnimator_bottom.getCurrentFrame());
            break;
        case Tank::EOrientation::Left:
            m_pSprite_left->render(m_position, m_size, m_rotation, m_layer, m_spriteAnimator_left.getCurrentFrame());
            break;
        case Tank::EOrientation::Right:
            m_pSprite_right->render(m_position, m_size, m_rotation, m_layer, m_spriteAnimator_right.getCurrentFrame());
            break;
        default:
            break;
        }

        if (m_hasShield)
        {
            m_pSprite_shield->render(glm::vec2(m_position.x - 0.5f, m_position.y - 1), m_size, m_rotation, m_layer + 0.1f, m_spriteAnimator_shield.getCurrentFrame());
        }

    }

    if (m_pCurrentBullet->isActive())
    {
        m_pCurrentBullet->render();
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

    if ((m_position.x<0 || m_position.x > m_pLevel->getWindowSizeInPixels().x || m_position.y<0 || m_position.y > m_pLevel->getWindowSizeInPixels().y) && !m_isDestroyed)
    {
        std::cout << "Tank out of bounds level. Call Tank::destroy()\n";
        destroy();
    }

    if (m_pCurrentBullet->isActive())
    {
        m_pCurrentBullet->update(delta);
        if (m_pCurrentBullet->getCurrentVelocity() != 0)
        {
            bool b1 = m_pCurrentBullet->getCurrentPosition().y < 0;
            bool b2 = m_pCurrentBullet->getCurrentPosition().y > m_pLevel->getWindowSizeInPixels().y;
            bool b3 = m_pCurrentBullet->getCurrentPosition().x < 0;
            bool b4 = m_pCurrentBullet->getCurrentPosition().x > m_pLevel->getWindowSizeInPixels().x + 10;
            auto a1 = m_pCurrentBullet->getCurrentPosition();
            auto a2 = m_pLevel->getWindowSizeInPixels();
            if (m_pCurrentBullet->getCurrentPosition().y < 0 || m_pCurrentBullet->getCurrentPosition().y > m_pLevel->getWindowSizeInPixels().y
                || m_pCurrentBullet->getCurrentPosition().x < 0 || m_pCurrentBullet->getCurrentPosition().x > m_pLevel->getWindowSizeInPixels().x + 10 && !m_pCurrentBullet->isExplosioning())
            {
                std::cout << "Bullet out of bounds level. Call Bullet::onCOllisionCallback()\n";
                m_pCurrentBullet->getColliders()[0].onCollisionCallback(*m_pCurrentBullet.get(), Physics::ECollisionDirection::Bottom);
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
        if (m_pAIComponent)
        {
            m_pAIComponent->update(delta);
            //setVelocity(0);
        }

        if (m_hasShield)
        {
            m_spriteAnimator_shield.update(delta);
            m_shieldTimer.update(delta);
        }

        if (m_velocity > 0)
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
    if (!m_isSpawning  && !m_pCurrentBullet->isActive())
    {
        Physics::PhysicsEngine::addDynamicGameObject(m_pCurrentBullet);
        m_pCurrentBullet->fire(m_position + m_size / 4.f + m_size * m_direction/4.f, m_direction);
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
}

void Tank::respawn()
{
    m_isDestroyed = false;
    m_respawnTimer.start(1500);
    m_isSpawning = true;
}