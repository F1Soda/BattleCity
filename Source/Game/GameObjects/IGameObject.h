#pragma once

#include <glm/vec2.hpp>
#include "../../Physics/PhysicsEngine.h"
#include <vector>

class IGameObject
{
public:

	enum class EObjectType
	{
		BettonWall,
		Border,
		BrickWall,
		Bullet,
		Eagle,
		Ice,
		Tank,
		Trees,
		Water,

		Empty,
		Unknown
	};

	IGameObject(const glm::vec2& position, const glm::vec2& size, const float rotation, const float layer, const EObjectType objectType);
	void setOwner(IGameObject* pOwner);
	IGameObject* getOwner() const { return m_pOwner; }
	virtual void render() const = 0;
	virtual void update(const double delta) {};
	virtual ~IGameObject();

	virtual glm::vec2& getCurrentPosition() { return m_position; }
	virtual glm::vec2& getTargetPosition() { return m_targetPosition; }
	virtual glm::vec2& getCurrentDirection() { return m_direction; }
	virtual double getCurrentVelocity() { return m_velocity; }
	virtual void setVelocity(const double velocity);

	const glm::vec2& getSize() const { return m_size; }
	const std::vector<Physics::Collider>& getColliders() const { return m_colliders; }
	const bool isActive() const { return m_isActive; }

	EObjectType getObjectType() const { return m_objectType; }
	virtual bool collides(const EObjectType objectType) { return true; }
	virtual void onCollision() {}

protected:
	IGameObject* m_pOwner;
	glm::vec2 m_position;
	glm::vec2 m_targetPosition;
	glm::vec2 m_size;
	float m_rotation;
	float m_layer;
	EObjectType m_objectType;

	glm::vec2 m_direction;
	double m_velocity;
	std::vector<Physics::Collider> m_colliders;
	bool m_isActive;
};