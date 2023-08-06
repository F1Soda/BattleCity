#pragma once

#include "IGameObject.h"
#include "../../Renderer/spriteAnimator.h"
#include <memory>
#include <array>

namespace RenderEngine
{
	class Sprite;
	class SpriteAnimator;
}

class Water : public IGameObject
{
public: 

	enum class EBlockLocation
	{
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight
	};

	Water(const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer);
	virtual void render() const override;
	virtual void update(const double delta) override;
	virtual bool Water::collides(const EObjectType objectType) override;

private:
	void renderBlock(const EBlockLocation eBlockLocation) const;

	
	std::shared_ptr<RenderEngine::Sprite> m_pSprite;
	RenderEngine::SpriteAnimator m_spriteAnimator;
	const std::array<glm::vec2, 4> m_blockOffsets;
};
