#pragma once

#include "IGameObject.h"
#include <memory>
#include <array>

namespace RenderEngine
{
	class Sprite;
}

class BetonWall : public IGameObject
{
public: 

	enum class EBetonWallType : uint8_t
	{
		All,
		Top,
		Bottom,
		Left,
		Right,
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight
	};

	enum class EBlockState : uint8_t
	{
		Enabled, 
		Destroyed
	};

	enum class EBlockLocation : uint8_t
	{
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight
	};

	BetonWall(const EBetonWallType eBlockWallType , const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer);
	virtual void render() const override;
	virtual void update(const double delta) override;
	void onCollisionCallback(const EBlockLocation location,const IGameObject& object, const Physics::ECollisionDirection direction);
	static Physics::AABB getAABBForBlockState(const EBlockLocation location,  const glm::vec2& size);
	//virtual void onCollision(IGameObject&) override;

private:
	void renderBlock(const EBlockLocation eBlockLocation) const;

	std::array<EBlockState, 4> m_eCurrentBlockState;
	std::array<Physics::Collider*, 4> m_blockLocationToColliderMap;
	std::shared_ptr<RenderEngine::Sprite> m_pSprite;
	const std::array<glm::vec2, 4> m_blockOffsets;
};
