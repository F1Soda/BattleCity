#include "PhysicsEngine.h"
#include "../Game/GameObjects/IGameObject.h"
#include "../Game/GameStates/Level.h"
#include "../Game/GameObjects/Tank.h"
#include <iostream>

namespace Physics
{
	bool isSetNewLevel = false;

	std::unordered_set<std::shared_ptr<IGameObject>> PhysicsEngine::m_dynamicObjects;
	std::shared_ptr<Level> PhysicsEngine::m_pCurrentLevel;
	void PhysicsEngine::init()
	{

	}
	void PhysicsEngine::terminate()
	{
		m_dynamicObjects.clear();
		m_pCurrentLevel.reset();
	}

	void PhysicsEngine::update(const double delta)
	{

		calculateTargetPositions(m_dynamicObjects, delta);
		
		//check collisions

		for (auto it1 = m_dynamicObjects.begin(); it1 != m_dynamicObjects.end();)
		{
			auto pObject1 = *it1;
			for (auto it2 = ++it1; it2 != m_dynamicObjects.end(); ++it2)
			{

				


				auto pObject2 = *it2;
				if (pObject1->getOwner() == pObject2.get() || pObject2->getOwner() == pObject1.get())
					continue;

				if (pObject1->getCurrentVelocity() == 0 && pObject1->getObjectType() == IGameObject::EObjectType::Bullet)
					continue;
				if (pObject2->getCurrentVelocity() == 0 && pObject2->getObjectType() == IGameObject::EObjectType::Bullet)
					continue;

				

				if ( ! (hasPositionIntersection(pObject1, pObject1->getTargetPosition(), pObject2, pObject2->getTargetPosition())))
				{
					//pObject1->getTargetPosition() = pObject1->getCurrentPosition();
					//pObject2->getTargetPosition() = pObject2->getCurrentPosition();
					continue;
				}

				/*if (hasPositionIntersection(pObject1, pObject1->getTargetPosition(), pObject2, pObject2->getTargetPosition(), true) && pObject2->isActive() && pObject1->isActive())
				{
					if (pObject1->getObjectType() == IGameObject::EObjectType::Tank && pObject2->getObjectType() == IGameObject::EObjectType::Tank)
					{
						Tank* pT1 = dynamic_cast<Tank*>(pObject1.get());
						Tank* pT2 = dynamic_cast<Tank*>(pObject2.get());
						if ((pT1->m_isEnemyTank && !pT2->m_isEnemyTank) || (pT2->m_isEnemyTank && !pT1->m_isEnemyTank))
						{
							pObject1->getTargetPosition() = pObject1->getCurrentPosition();
							pObject2->getTargetPosition() = pObject2->getCurrentPosition();
							continue;
						}
					}
				}*/

				if (hasPositionIntersection(pObject1, pObject1->getTargetPosition(), pObject2, pObject2->getCurrentPosition(), true) && pObject2->isActive() && pObject1->isActive())
				{
					pObject1->getTargetPosition() = pObject1->getCurrentPosition();
					pObject1->onCollision(*pObject2);
					pObject2->onCollision(*pObject1);
					if (pObject1->getObjectType() == IGameObject::EObjectType::Tank && pObject2->getObjectType() == IGameObject::EObjectType::Tank)
					{
						Tank* tank1 = dynamic_cast<Tank*>(pObject1.get());
							Tank* tank2 = dynamic_cast<Tank*>(pObject2.get());
							if (tank1->m_isEnemyTank && tank2->m_isEnemyTank)
							{
 								tank1->setOrientation(Tank::EOrientation((int(tank1->getOrintationTank()) + 2) % 4));
								//tank2->setOrientation(Tank::EOrientation((int(tank2->getOrintationTank()) + 2) % 4));
								checkIfTankInTank(tank1, tank2);
							}

					}
				}
				if (hasPositionIntersection(pObject2, pObject2->getTargetPosition(), pObject1, pObject1->getCurrentPosition(), true) && pObject2->isActive() && pObject1->isActive())
				{
					pObject2->getTargetPosition() = pObject2->getCurrentPosition();
					pObject1->onCollision(*pObject2);
					pObject2->onCollision(*pObject1);
					if (pObject1->getObjectType() == IGameObject::EObjectType::Tank && pObject2->getObjectType() == IGameObject::EObjectType::Tank)
					{
						Tank* tank1 = dynamic_cast<Tank*>(pObject1.get());
						Tank* tank2 = dynamic_cast<Tank*>(pObject2.get());
						if (tank1->m_isEnemyTank && tank2->m_isEnemyTank)
						{
							//tank1->setOrientation(Tank::EOrientation((int(tank1->getOrintationTank()) + 2) % 4));
							tank2->setOrientation(Tank::EOrientation((int(tank2->getOrintationTank()) + 2) % 4));
							checkIfTankInTank(tank1, tank2);
						}

					}
				}

				if (isSetNewLevel)
				{
					isSetNewLevel = false;
					goto Exit;
				}

				/*if ( && pObject1->isActive()&& pObject2->isActive())
				{
				 	pObject2->getTargetPosition() = pObject2->getCurrentPosition();
					pObject2->onCollision(*pObject1);
					pObject1->onCollision(*pObject2);
				}*/
			}
		}
		updatePosition(m_dynamicObjects); // CurrentPosition = TargetPosition;

		Exit:
			std::cout << "";
	}

	void PhysicsEngine::addDynamicGameObject(std::shared_ptr<IGameObject> pGameObject)
	{
		m_dynamicObjects.insert(std::move(pGameObject));
	}

	void PhysicsEngine::removeDynamicGameObject(std::shared_ptr<IGameObject> pGameObject)
	{
		m_dynamicObjects.erase(pGameObject);
	}

	void PhysicsEngine::setCurrentLevel(std::shared_ptr<Level> pLevel)
	{
		isSetNewLevel = true;
		m_pCurrentLevel.swap(pLevel);
		m_dynamicObjects.clear();
		m_pCurrentLevel->initLevel();

	}

	bool PhysicsEngine::hasCollidersIntersection(const Collider& collider1, const glm::vec2 position1, const Collider& collider2, const glm::vec2 position2, bool forTanks)
	{

		const int acceptableOccurrencePixels = forTanks ? 0 : 0;

		const glm::vec2 collider1_bottomLeft_world = collider1.boundingBox.bottomLeft + position1;
		const glm::vec2 collider1_topRight_world = collider1.boundingBox.topRight + position1;

		const glm::vec2 collider2_bottomLeft_world = collider2.boundingBox.bottomLeft + position2;
		const glm::vec2 collider2_topRight_world = collider2.boundingBox.topRight + position2;

		if (collider1_bottomLeft_world.x >= collider2_topRight_world.x + acceptableOccurrencePixels)
			return false;
		if (collider1_topRight_world.x <= collider2_bottomLeft_world.x - acceptableOccurrencePixels)
			return false;
		if (collider1_bottomLeft_world.y >= collider2_topRight_world.y + acceptableOccurrencePixels)
			return false;
		if (collider1_topRight_world.y <= collider2_bottomLeft_world.y - acceptableOccurrencePixels)
			return false;
		return true;
	}

	void PhysicsEngine::calculateTargetPositions(std::unordered_set<std::shared_ptr<IGameObject>> dynamicObjects, const double delta)
	{

		for (auto& currentDynamicObject : dynamicObjects)
		{

			if (currentDynamicObject->getCurrentVelocity() > 0)
			{
				// align position to multiple of 4 pixels
				if (currentDynamicObject->getCurrentDirection().x != 0.f)
				{
					currentDynamicObject->getTargetPosition() = glm::vec2(currentDynamicObject->getCurrentPosition().x, static_cast<unsigned int>(currentDynamicObject->getCurrentPosition().y / 4.f + 0.5f) * 4.f);
				}
				else if (currentDynamicObject->getCurrentDirection().y != 0.f)
				{
					currentDynamicObject->getTargetPosition() = glm::vec2(static_cast<unsigned int>(currentDynamicObject->getCurrentPosition().x / 4.f + 0.5f) * 4.f, currentDynamicObject->getCurrentPosition().y);
				}

				const auto newPosition = currentDynamicObject->getTargetPosition() + static_cast<float>(currentDynamicObject->getCurrentVelocity() * delta) * currentDynamicObject->getCurrentDirection();
				std::vector<std::shared_ptr<IGameObject>> objectsToCheck = m_pCurrentLevel->getObjectsInArea(newPosition, newPosition + currentDynamicObject->getSize());

				const auto& colliders = currentDynamicObject->getColliders();
				bool hasCollision = false;

				ECollisionDirection dynamicObjectCollisionDirection = ECollisionDirection::Right;
				if (currentDynamicObject->getCurrentDirection().x < 0) dynamicObjectCollisionDirection = ECollisionDirection::Left;
				else if (currentDynamicObject->getCurrentDirection().y > 0) dynamicObjectCollisionDirection = ECollisionDirection::Top;
				else if (currentDynamicObject->getCurrentDirection().y < 0) dynamicObjectCollisionDirection = ECollisionDirection::Bottom;

				ECollisionDirection objectCollisionDirection = ECollisionDirection::Left;
				if (currentDynamicObject->getCurrentDirection().x < 0) objectCollisionDirection = ECollisionDirection::Right;
				else if (currentDynamicObject->getCurrentDirection().y > 0) objectCollisionDirection = ECollisionDirection::Bottom;
				else if (currentDynamicObject->getCurrentDirection().y < 0) objectCollisionDirection = ECollisionDirection::Top;


				for (const auto& currentDynamicObjectCollider : colliders)
				{
					for (const auto& currentObjectToCheck : objectsToCheck)
					{

						const auto& collidersToCheck = currentObjectToCheck->getColliders();
						if (currentObjectToCheck->collides(currentDynamicObject->getObjectType()) && !collidersToCheck.empty())
						{
							for (const auto& currentObjectCollider : currentObjectToCheck->getColliders())
							{
								if (currentObjectCollider.isActive && hasCollidersIntersection(currentDynamicObjectCollider, newPosition, currentObjectCollider, currentObjectToCheck->getCurrentPosition()))
								{
									hasCollision = true;
									if (currentObjectCollider.onCollisionCallback)
									{
										currentObjectCollider.onCollisionCallback(*currentDynamicObject, objectCollisionDirection);
									}
									if (currentDynamicObjectCollider.onCollisionCallback)
									{
										currentDynamicObjectCollider.onCollisionCallback(*currentObjectToCheck, dynamicObjectCollisionDirection);
									}
								}
							}
						}
					}

				}

				if (!hasCollision)
					currentDynamicObject->getTargetPosition() = newPosition;
				else
				{
					// align position to multiple of 4 pixels
					if (currentDynamicObject->getCurrentDirection().x != 0.f)
					{
						currentDynamicObject->getTargetPosition() = glm::vec2(currentDynamicObject->getTargetPosition().x, static_cast<unsigned int>(currentDynamicObject->getTargetPosition().y / 4.f + 0.5f) * 4.f);
					}
					else if (currentDynamicObject->getCurrentDirection().y != 0.f)
					{
						currentDynamicObject->getTargetPosition() = glm::vec2(static_cast<unsigned int>(currentDynamicObject->getTargetPosition().x / 4.f + 0.5f) * 4.f, currentDynamicObject->getTargetPosition().y);
					}
					//currentDynamicObject->onCollision();
				}


			}
		}
	}

	void PhysicsEngine::updatePosition(std::unordered_set<std::shared_ptr<IGameObject>> dynamicObjects)
	{
		for (auto& currentDynamicObject : dynamicObjects)
		{
			currentDynamicObject->getCurrentPosition() = currentDynamicObject->getTargetPosition();
		}
	}

	bool PhysicsEngine::hasPositionIntersection(const std::shared_ptr<IGameObject>& pObject1, const glm::vec2 position1, const std::shared_ptr<IGameObject>& pObject2, const glm::vec2 position2, bool forTanks)
	{
		const auto& currentObjectColliders = pObject1->getColliders();
		const auto& otherObjectColliders = pObject2->getColliders();

		for (const auto& currentObjectCollider : currentObjectColliders)
		{
			for (const auto& otherObjectCollider : otherObjectColliders)
			{
				if (hasCollidersIntersection(currentObjectCollider, position1, otherObjectCollider, position2, forTanks))
				{
					return true;
				}
			}
		}
		return false;

	}

	
	void PhysicsEngine::checkIfTankInTank(Tank* t1, Tank* t2)
	{
		int admissiblePixelsEnterSprite = 4;
		if (true)
		{
			if (t1->getCurrentPosition() != t2->getCurrentPosition())
			{
				Tank* pRT = t1->getTargetPosition().x > t2->getTargetPosition().x ? t1 : t2;
				Tank* pLT = t1->getTargetPosition().x > t2->getTargetPosition().x ? t2 : t1;
				Tank* pTT = t1->getTargetPosition().y > t2->getTargetPosition().y ? t1 : t2;
				Tank* pBT = t1->getTargetPosition().y > t2->getTargetPosition().y ? t2 : t1;
				glm::vec2& posRT = pRT->getTargetPosition();
				glm::vec2& posLT = pLT->getTargetPosition();
				//auto a1 = pRT->getTargetPosition().x;
				//auto a2 = pLT->getTargetPosition().x;
				//auto res1 = pRT->getTargetPosition().x - pLT->getTargetPosition().x;
				//auto res2 = pLT->getTargetPosition().x - pLT->getTargetPosition().x;
				if (pRT->getTargetPosition().x - pLT->getTargetPosition().x < Level::BLOCK_SIZE - admissiblePixelsEnterSprite && pTT->getTargetPosition().y - pBT->getTargetPosition().y < Level::BLOCK_SIZE - admissiblePixelsEnterSprite)
				{
					if (pRT->getCurrentVelocity() != 0)
					{
						switch (pRT->getOrintationTank())
						{	
						case Tank::EOrientation::Right:
							posRT = glm::vec2(posRT.x + Level::BLOCK_SIZE, posRT.y);
							break;
						case Tank::EOrientation::Left:
							posRT = glm::vec2(posRT.x + Level::BLOCK_SIZE, posRT.y);
							break;
						case Tank::EOrientation::Bottom:
							if (pRT == pTT)
							{
								posRT = glm::vec2(posRT.x, posLT.y + Level::BLOCK_SIZE);
							}
							else
							{
								posRT = glm::vec2(posRT.x, posLT.y - Level::BLOCK_SIZE);
							}
							break;
						case Tank::EOrientation::Top:
							if (pRT == pTT)
							{
								posRT = glm::vec2(posRT.x, posLT.y + Level::BLOCK_SIZE);
							}
							else
							{
								posRT = glm::vec2(posRT.x, posLT.y - Level::BLOCK_SIZE);
							}
							break;
						}
					}
					else
					{
						switch (pLT->getOrintationTank())
						{
						case Tank::EOrientation::Right:
							posLT = glm::vec2(posRT.x - Level::BLOCK_SIZE, posLT.y);
							break;
						case Tank::EOrientation::Left:
							posLT = glm::vec2(posRT.x - Level::BLOCK_SIZE, posLT.y);
							break;
						case Tank::EOrientation::Bottom:
							if (pLT == pTT)
							{
								posLT = glm::vec2(posLT.x, posRT.y + Level::BLOCK_SIZE);
							}
							else
							{
								posLT = glm::vec2(posLT.x, posRT.y - Level::BLOCK_SIZE);
							}
							break;
						case Tank::EOrientation::Top:
							if (pLT == pTT)
							{
								posLT = glm::vec2(posLT.x, posRT.y + Level::BLOCK_SIZE);
							}
							else
							{
								posLT = glm::vec2(posLT.x, posRT.y - Level::BLOCK_SIZE);
							}
							break;
						}
					}
				}
			}
		}
		else
		{
			if (t1->getCurrentVelocity() != 0)
			{
				switch (t1->getOrintationTank())
				{
				case Tank::EOrientation::Right:
					t1->getTargetPosition() -= glm::vec2(Level::BLOCK_SIZE, 0);
					break;
				case Tank::EOrientation::Left:
					t1->getTargetPosition() += glm::vec2(Level::BLOCK_SIZE, 0);
					break;
				case Tank::EOrientation::Bottom:
					t1->getTargetPosition() += glm::vec2(0, Level::BLOCK_SIZE);
					break;
				case Tank::EOrientation::Top:
					t1->getTargetPosition() -= glm::vec2(0, Level::BLOCK_SIZE);
					break;
				}
			}
			else
			{
				switch (t2->getOrintationTank())
				{
				case Tank::EOrientation::Right:
					t2->getTargetPosition() -= glm::vec2(Level::BLOCK_SIZE, 0);
					break;
				case Tank::EOrientation::Left:
					t2->getTargetPosition() += glm::vec2(Level::BLOCK_SIZE, 0);
					break;
				case Tank::EOrientation::Bottom:
					t2->getTargetPosition() += glm::vec2(0, Level::BLOCK_SIZE);
					break;
				case Tank::EOrientation::Top:
					t2->getTargetPosition() -= glm::vec2(0, Level::BLOCK_SIZE);
					break;
				}
			}
		}
	}
}