#pragma once

#include <unordered_set>
#include <memory>
#include <glm/vec2.hpp>
#include <functional>

class IGameObject;
class Level;
class Tank;
class GameManager;

namespace Physics
{

	enum class ECollisionDirection : uint8_t
	{
		Top,
		Bottom,
		Left, 
		Right
	};

	struct AABB // Axis Aligned Bounding Box
	{
		AABB(const glm::vec2 _bottomLeft, const glm::vec2 _topRight)
			: bottomLeft(_bottomLeft)
			, topRight(_topRight)
		{
		}
		glm::vec2 bottomLeft;
		glm::vec2 topRight;
	};

	struct Collider {
		Collider(const glm::vec2& _bottomLeft, const glm::vec2 _topRight, std::function<void(const IGameObject&, const ECollisionDirection)> _onCollisionCallback = {})
			: boundingBox(_bottomLeft, _topRight)
			, isActive(true)
			, onCollisionCallback(_onCollisionCallback)
		{}

		Collider(const AABB& _boundingBox, std::function<void(const IGameObject&, const ECollisionDirection)> _onCollisionCallback = {})
			: boundingBox(_boundingBox)
			, isActive(true)
			, onCollisionCallback(_onCollisionCallback)
		{}

		AABB boundingBox;
		bool isActive;
		std::function<void(const IGameObject&, const ECollisionDirection)> onCollisionCallback;
	};

	class PhysicsEngine
	{
	public:
		PhysicsEngine() = delete;
		~PhysicsEngine() = delete;
		PhysicsEngine(const PhysicsEngine&) = delete;
		PhysicsEngine(const PhysicsEngine&&) = delete;
		PhysicsEngine& operator=(const PhysicsEngine&) = delete;
		PhysicsEngine& operator=(PhysicsEngine&&) = delete;
		PhysicsEngine(PhysicsEngine&&) = delete;

		static void init(GameManager* pGameManager);
		static void terminate();

		static void update(const double delta);
		static void addDynamicGameObject(std::shared_ptr<IGameObject> pGameObject);
		static void removeDynamicGameObject(std::shared_ptr<IGameObject> pGameObject);
		static void setCurrentLevel(std::shared_ptr<Level> pLevel);

	private:
		static std::unordered_set<std::shared_ptr<IGameObject>> m_dynamicObjects;
		static std::shared_ptr<Level> m_pCurrentLevel;

		static bool PhysicsEngine::hasCollidersIntersection(const Collider& collider1, const glm::vec2 position1, const Collider& collider2, const glm::vec2 position2, bool forTanks = false);
		static bool PhysicsEngine::hasPositionIntersection(const std::shared_ptr<IGameObject>& pObject1, const glm::vec2 position1, const std::shared_ptr<IGameObject>& pObject2, const glm::vec2 position2, bool forTanks = false);

		static void calculateTargetPositions(std::unordered_set<std::shared_ptr<IGameObject>> dynamicObjects, const double delta);
		static void updatePosition(std::unordered_set<std::shared_ptr<IGameObject>> dynamicObjects);

		static IGameObject* getObjectUnderTank(Tank& tank);

		static void checkIfTankInTank(Tank* t1, Tank* t2);

		 static GameManager* m_pGameManager;

	};
}