#include "Ice.h"
#include "../../Renderer/sprite.h"
#include "../../Resources/ResourceManager.h"

Ice::Ice(const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer)
	:IGameObject(position, size, rotation, layer, EObjectType::Ice)
    , m_pSprite(Resources::ResourceManager::getSprite("ice"))
    , m_blockOffsets{ glm::vec2(0, m_size.y / 2.f),
        glm::vec2(m_size.x / 2.f, m_size.y / 2.f),
        glm::vec2(0, 0),
        glm::vec2(m_size.x / 2.f, 0) }
{
}

void Ice::renderBlock(const EBlockLocation eBlockLocation) const
{
    m_pSprite->render(m_position + m_blockOffsets[static_cast<size_t>(eBlockLocation)], m_size/2.0f, m_rotation, m_layer);
}

void Ice::render() const
{
    renderBlock(EBlockLocation::TopLeft);
    renderBlock(EBlockLocation::TopRight);
    renderBlock(EBlockLocation::BottomLeft);
    renderBlock(EBlockLocation::BottomRight);
}
