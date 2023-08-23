#include "BetonWall.h"
#include "../../Renderer/sprite.h"
#include "../../Resources/ResourceManager.h"
#include "Bullet.h"
#include <iostream>

BetonWall::BetonWall(const EBetonWallType eBlockWallType, const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer)
	:IGameObject(position, size, rotation, layer, EObjectType::BettonWall)
	, m_eCurrentBlockState{EBlockState::Destroyed, 
                           EBlockState::Destroyed,
                           EBlockState::Destroyed, 
                           EBlockState::Destroyed, }
, m_pSprite(Resources::ResourceManager::getSprite("betonWall"))
    , m_blockOffsets{ glm::vec2(0, m_size.y / 2.f),
        glm::vec2(m_size.x / 2.f, m_size.y / 2.f),
        glm::vec2(0, 0),
        glm::vec2(m_size.x / 2.f, 0) }
    
{
    auto onCollisionCallbackTopLeft = [&](const IGameObject& object, const Physics::ECollisionDirection direction)
        {
            onCollisionCallback(EBlockLocation::TopLeft, object, direction);
        };
    auto onCollisionCallbackTopRight = [&](const IGameObject& object, const Physics::ECollisionDirection direction)
        {
            onCollisionCallback(EBlockLocation::TopRight, object, direction);
        };
    auto onCollisionCallbackBottomLeft = [&](const IGameObject& object, const Physics::ECollisionDirection direction)
        {
            onCollisionCallback(EBlockLocation::BottomLeft, object, direction);
        };
    auto onCollisionCallbackBottomRight = [&](const IGameObject& object, const Physics::ECollisionDirection direction)
        {
            onCollisionCallback(EBlockLocation::BottomRight, object, direction);
        };
    m_blockLocationToColliderMap.fill(nullptr);
    m_colliders.reserve(4);

    switch (eBlockWallType)
    {
    case EBetonWallType::All:
        m_eCurrentBlockState.fill(EBlockState::Enabled);

        m_blockLocationToColliderMap[int(EBlockLocation::TopLeft)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::TopLeft, m_size), onCollisionCallbackTopLeft);
        m_blockLocationToColliderMap[int(EBlockLocation::TopRight)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::TopRight, m_size), onCollisionCallbackTopRight);
        m_blockLocationToColliderMap[int(EBlockLocation::BottomLeft)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::BottomLeft, m_size), onCollisionCallbackBottomLeft);
        m_blockLocationToColliderMap[int(EBlockLocation::BottomRight)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::BottomRight, m_size), onCollisionCallbackBottomRight);
        break;
    case EBetonWallType::Top:
        m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::TopLeft)] = EBlockState::Enabled;
        m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::TopRight)] = EBlockState::Enabled;

        m_blockLocationToColliderMap[int(EBlockLocation::TopLeft)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::TopLeft, m_size), onCollisionCallbackTopLeft);
        m_blockLocationToColliderMap[int(EBlockLocation::TopRight)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::TopRight, m_size), onCollisionCallbackTopRight);

        break;
    case EBetonWallType::Bottom:
        m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::BottomLeft)] = EBlockState::Enabled;
        m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::BottomRight)] = EBlockState::Enabled;

        m_blockLocationToColliderMap[int(EBlockLocation::BottomLeft)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::BottomLeft, m_size), onCollisionCallbackBottomLeft);
        m_blockLocationToColliderMap[int(EBlockLocation::BottomRight)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::BottomRight, m_size), onCollisionCallbackBottomRight);

        break;
    case EBetonWallType::Left:
        m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::TopLeft)] = EBlockState::Enabled;
        m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::BottomLeft)] = EBlockState::Enabled;

        m_blockLocationToColliderMap[int(EBlockLocation::TopLeft)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::TopLeft, m_size), onCollisionCallbackTopLeft);
        m_blockLocationToColliderMap[int(EBlockLocation::BottomLeft)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::BottomLeft, m_size), onCollisionCallbackBottomLeft);
        break;
    case EBetonWallType::Right:
        m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::TopRight)] = EBlockState::Enabled;
        m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::BottomRight)] = EBlockState::Enabled;

        m_blockLocationToColliderMap[int(EBlockLocation::TopRight)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::TopRight, m_size), onCollisionCallbackTopRight);
        m_blockLocationToColliderMap[int(EBlockLocation::BottomRight)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::BottomRight, m_size), onCollisionCallbackBottomRight);
        break;
    case EBetonWallType::TopLeft:
        m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::TopLeft)] = EBlockState::Enabled;

        m_blockLocationToColliderMap[int(EBlockLocation::TopLeft)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::TopLeft, m_size), onCollisionCallbackTopLeft);
        break;
    case EBetonWallType::TopRight:
        m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::TopRight)] = EBlockState::Enabled;

        m_blockLocationToColliderMap[int(EBlockLocation::TopRight)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::TopRight, m_size), onCollisionCallbackTopRight);
        break;
    case EBetonWallType::BottomLeft:
        m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::BottomLeft)] = EBlockState::Enabled;

        m_blockLocationToColliderMap[int(EBlockLocation::BottomLeft)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::BottomLeft, m_size), onCollisionCallbackBottomLeft);
        break;
    case EBetonWallType::BottomRight:
        m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::BottomRight)] = EBlockState::Enabled;

        m_blockLocationToColliderMap[int(EBlockLocation::BottomRight)] = &m_colliders.emplace_back(getAABBForBlockState(EBlockLocation::BottomRight, m_size), onCollisionCallbackBottomRight);
        break;
    }

}

void BetonWall::renderBlock(const EBlockLocation eBlockLocation) const
{
    
    const EBlockState state = m_eCurrentBlockState[static_cast<size_t>(eBlockLocation)];

    if (state != EBlockState::Destroyed)
    {
        m_pSprite->render(m_position + m_blockOffsets[static_cast<size_t>(eBlockLocation)], m_size/2.0f, m_rotation, m_layer);
    }
}

void BetonWall::render() const
{
    renderBlock(EBlockLocation::TopLeft);
    renderBlock(EBlockLocation::TopRight);
    renderBlock(EBlockLocation::BottomLeft);
    renderBlock(EBlockLocation::BottomRight);
}

void BetonWall::update(const double delta)
{
}


void BetonWall::onCollisionCallback(const EBlockLocation location,const IGameObject& object, const Physics::ECollisionDirection direction)
{
    static int counterBreaking = 0; // когда достигнет 4, то блок точно будет сломан и можно будет его убрать из вектора level::m_levelObjects(нужно для работы AIComponent)
    if (object.getObjectType() != IGameObject::EObjectType::Bullet) return;

    IGameObject& objectWithNoConst = const_cast<IGameObject&>(object);
    Bullet* bullet = dynamic_cast<Bullet*>(&objectWithNoConst);
    if (!bullet->canDestroyBetton()) return;

    counterBreaking++;
    m_eCurrentBlockState[int(location)] = EBlockState::Destroyed;
    Physics::Collider* a = m_blockLocationToColliderMap[static_cast<size_t>(location)];
    a->isActive = false;
    if (counterBreaking >= 4)
    {
        counterBreaking = 0;
        m_isActive = false;
        
    }
}


Physics::AABB BetonWall::getAABBForBlockState(const EBlockLocation location, const glm::vec2& size)
{
    glm::vec2 bottomLeft(0);
    glm::vec2 topRight(size.x/2, size.y/2);
    switch (location)
    {
    case EBlockLocation::BottomLeft:
        //bottomLeft = glm::vec2(0);
        //topRight = size /2.f;
        break;
    case EBlockLocation::BottomRight:
        bottomLeft.x += size.x / 2;
        topRight.x += size.x / 2;
        break;
    case EBlockLocation::TopLeft:
        topRight.y += size.y / 2;
        bottomLeft.y += size.y / 2;
        break;
    case EBlockLocation::TopRight:
        topRight += size / 2.f;
        bottomLeft += size / 2.f;
        break;
    }
    return { bottomLeft, topRight };

}

//void BetonWall::onCollision(IGameObject&)
//{
//    std::cout << "Called Beton::onCollision()\n";
//}