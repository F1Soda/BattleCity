#include "Eagle.h"
#include "../../Renderer/sprite.h"
#include "../../Resources/ResourceManager.h"
#include "../GameManager.h"

Eagle::Eagle(const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer, GameManager* pGameManager)
	:IGameObject(position, size, rotation, layer, EObjectType::Eagle)
    , m_sprites{ Resources::ResourceManager::getSprite("eagle") ,Resources::ResourceManager::getSprite("eagle_dead") }
    , m_eCurrentState(EEagleState::Alive)
    , m_pGameManager(pGameManager)
{
    auto onCollisionCallback = [&](const IGameObject& object, const Physics::ECollisionDirection direction)
        {
            if (object.getObjectType() == IGameObject::EObjectType::Bullet && dynamic_cast<Tank*>(object.getOwner())->m_isEnemyTank)
            {
                m_eCurrentState = EEagleState::Dead;
                gameOverTimer.start(2000);
            }
        };

    m_colliders.emplace_back(glm::vec2(0), m_size, onCollisionCallback);

    gameOverTimer.setCallback([&]() { m_pGameManager->gameOver(); });

}

void Eagle::update(const double delta)
{
    gameOverTimer.update(delta);
}

void Eagle::render() const
{
    m_sprites[static_cast<size_t>(m_eCurrentState)]->render(m_position, m_size, m_rotation, m_layer);
}

void Eagle::onCollision(IGameObject&object)
{
    if (object.getObjectType() == IGameObject::EObjectType::Bullet && dynamic_cast<Tank*>(object.getOwner())->m_isEnemyTank)
    {
        m_eCurrentState = EEagleState::Dead;
        gameOverTimer.start(2000);
    }
}
