#include "Border.h"
#include "../../Renderer/sprite.h"
#include "../../Resources/ResourceManager.h"

Border::Border(const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer)
	:IGameObject(position, size, rotation, layer, EObjectType::Border)
    , m_pSprite(ResourceManager::getSprite("border"))
{
    m_colliders.emplace_back(glm::vec2(0), m_size);
}



void Border::render() const
{
    m_pSprite->render(m_position, m_size, m_rotation, m_layer);
}
