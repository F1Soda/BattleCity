#include "PhysicsEngine.h"
#include "../Game/GameObjects/IGameObject.h"
#include "../Game/UI/GameStates/Level.h"
#include "../Game/GameObjects/Tank.h"
#include <iostream>
#include <algorithm>
#include "../Game/GameObjects/Bullet.h"
//#include "../Game/GameManager.h" // nothing including 

namespace Physics
{
	bool isSetNewLevel = false;

	std::vector<std::shared_ptr<IGameObject>> PhysicsEngine::m_dynamicObjects;
	std::stack<std::shared_ptr<IGameObject>> PhysicsEngine::m_objectToAddInNextFrame;
	std::shared_ptr<Level> PhysicsEngine::m_pCurrentLevel;
	GameManager* PhysicsEngine::m_pGameManager;
	void PhysicsEngine::init(GameManager* pGameManager)
	{
		m_pGameManager = pGameManager;
	}
	void PhysicsEngine::terminate()
	{
		m_dynamicObjects.clear();
		m_pCurrentLevel.reset();
	}

	void PhysicsEngine::update(const double delta)
	{
	
		bool bonusWasUsed = false;

		//m_dynamicObjects.erase(std::remove(m_dynamicObjects.begin(), m_dynamicObjects.end(), nullptr), m_dynamicObjects.end());
		calculateTargetPositions(m_dynamicObjects, delta);
		//std::vector < std::shared_ptr<IGameObject>>::iterator it;
		//check collisions
		for (auto it1 = m_dynamicObjects.begin(); it1 != m_dynamicObjects.end();)
		{
			auto pObject1 = *it1;
			for (auto it2 = ++it1; it2 != m_dynamicObjects.end(); ++it2)
			{
				auto pObject2 = *it2;

				if (pObject1 == nullptr || pObject2 == nullptr)
				{
					continue;
				}

				if (it1 == m_dynamicObjects.end() || it2 == m_dynamicObjects.end())
				{
					std::cerr << "";
				}

				
				if (pObject1->getCurrentVelocity() == 0 && pObject1->getObjectType() == IGameObject::EObjectType::Bullet)
					continue;
				if (pObject2->getCurrentVelocity() == 0 && pObject2->getObjectType() == IGameObject::EObjectType::Bullet)
					continue;

				if (pObject1->getOwner() == pObject2.get() || pObject2->getOwner() == pObject1.get())
 					continue;

				if ( ! (hasPositionIntersection(pObject1, pObject1->getTargetPosition(), pObject2, pObject2->getTargetPosition())))
				{
					continue;
				}

				if (pObject1->getObjectType() == IGameObject::EObjectType::Bonus )
				{
					if (pObject2->getObjectType() == IGameObject::EObjectType::Tank && hasPositionIntersection(pObject2, pObject2->getTargetPosition(), pObject1, pObject1->getCurrentPosition(), true) && !dynamic_cast<Tank*>(pObject2.get())->m_isEnemyTank && !bonusWasUsed)
					{
						pObject1->onCollision(*pObject2);
					}

					bonusWasUsed = true;

					continue;
					//goto Exit;
				}
				else if (pObject2->getObjectType() == IGameObject::EObjectType::Bonus)
				{
					if (pObject1->getObjectType() == IGameObject::EObjectType::Tank && hasPositionIntersection(pObject1, pObject1->getTargetPosition(), pObject2, pObject2->getCurrentPosition(), true) && !dynamic_cast<Tank*>(pObject1.get())->m_isEnemyTank && !bonusWasUsed)
					{
						pObject2->onCollision(*pObject1);
					}
					bonusWasUsed = true;
					continue;
					//goto Exit;
				}


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
			}
		}
		//std::vector < std::shared_ptr<IGameObject>>::iterator;
		//t = ;
		//if (it != m_dynamicObjects.end())
		//{
			//std::cout << "";
		//}
		m_dynamicObjects.erase(std::remove(m_dynamicObjects.begin(), m_dynamicObjects.end(), nullptr), m_dynamicObjects.end());

		updatePosition(m_dynamicObjects); // CurrentPosition = TargetPosition;

		while (!m_objectToAddInNextFrame.empty())
		{
			m_dynamicObjects.push_back(m_objectToAddInNextFrame.top());
			m_objectToAddInNextFrame.pop();
		}

		

		Exit:
			std::cout << "";
	}

	void PhysicsEngine::addDynamicGameObject(std::shared_ptr<IGameObject> pGameObject)
	{
		m_dynamicObjects.push_back(std::move(pGameObject));
	}

	void PhysicsEngine::removeDynamicGameObject(std::shared_ptr<IGameObject> pGameObject)
	{
		for (auto it = m_dynamicObjects.begin(); it != m_dynamicObjects.end(); it++)
		{
			if ((*it).get() == pGameObject.get())
			{
				m_dynamicObjects.erase(it);
				break;
			}
		}

		
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

	void PhysicsEngine::calculateTargetPositions(std::vector<std::shared_ptr<IGameObject>>& dynamicObjects, const double delta)
	{

		for (auto& currentDynamicObject : dynamicObjects)
		{

			if (currentDynamicObject.get() && currentDynamicObject->getCurrentVelocity() > 0 && currentDynamicObject->isActive())
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
				std::vector<std::shared_ptr<IGameObject>> objectsToCheck = std::move(m_pCurrentLevel->getObjectsInArea(newPosition, newPosition + currentDynamicObject->getSize()));

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

						if (currentObjectToCheck->getObjectType() == IGameObject::EObjectType::Ice && currentDynamicObject->getObjectType() == IGameObject::EObjectType::Tank)
						{
							//std::cout << "Ices!\n";
							Tank* tank = dynamic_cast<Tank*>(currentDynamicObject.get());
							IGameObject* objectUnderTank = getObjectUnderTank(*tank);
							if (objectUnderTank && objectUnderTank->getObjectType() == IGameObject::EObjectType::Ice)
							{
								currentDynamicObject->getColliders()[0].onCollisionCallback(*currentObjectToCheck, dynamicObjectCollisionDirection);
							}
						}
						bool debBool = currentObjectToCheck->getObjectType() == IGameObject::EObjectType::BettonWall;
  						const auto& collidersToCheck = currentObjectToCheck->getColliders();
						bool debBool2 = currentDynamicObject->getObjectType() == IGameObject::EObjectType::Bullet;
						if (currentObjectToCheck->collides(currentDynamicObject->getObjectType()) && !collidersToCheck.empty())
						{
							for (const auto& currentObjectCollider : currentObjectToCheck->getColliders())
							{

								Bullet* bullet;
								if (debBool2)
								{
									IGameObject* obj = const_cast<IGameObject*>(currentDynamicObject.get());
									bullet = dynamic_cast<Bullet*>(obj);
								}
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
					if (!currentDynamicObject.get()) continue;

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

	void PhysicsEngine::updatePosition(std::vector<std::shared_ptr<IGameObject>>& dynamicObjects)
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
	}

  	IGameObject* PhysicsEngine::getObjectUnderTank(Tank& tank)
	{

		if (m_pGameManager == nullptr)
		{
			std::cerr << "ERROR: Attempt to get level info with null pointer on GameManager: PhysicsEngine::getObjectUnderTank() \n";
			std::abort();
		}

		glm::vec2 positionTank = tank.getCurrentPosition();
		glm::vec2 windowScaleInPixels = m_pGameManager->getScaleScreenInPixels();
		glm::ivec2 correctedPositionTank = glm::vec2(std::clamp(std::round(positionTank.x - Level::BLOCK_SIZE), 0.f, static_cast<float>(windowScaleInPixels.x)), std::clamp(std::round(windowScaleInPixels.y - positionTank.y) + Level::BLOCK_SIZE / 2, 0.f, static_cast<float>(windowScaleInPixels.y)));

		int blockX = static_cast<int>(std::round(float(correctedPositionTank.x) / 16.f));
		int blockY = static_cast<int>(std::round(float(correctedPositionTank.y) / 16.f));

		
		IGameObject* res = m_pGameManager->getObjectByIndex(static_cast<unsigned int>((blockY - 1) * m_pGameManager->getScaleScreenInBlocks().x + blockX));

		return res;
	}


	void PhysicsEngine::nullifyDyanmicObject(IGameObject* pGameObject)
	{
		for (auto it = m_dynamicObjects.begin(); it != m_dynamicObjects.end(); it++)
		{
			if ((*it).get() == pGameObject)
			{
 				(*it) = nullptr;
				break;
			}
		}
	}

	void PhysicsEngine::addDynamicObjectInNextFrame(std::shared_ptr<IGameObject> object)
	{
		m_objectToAddInNextFrame.push(object);
	}


	IGameObject* PhysicsEngine::getObjectByPos(glm::vec2 pos)
	{
		glm::vec2 windowScaleInPixels = m_pGameManager->getScaleScreenInPixels();
		glm::ivec2 correctedPositionTank = glm::vec2(std::clamp(std::round(pos.x - Level::BLOCK_SIZE), -16.f, static_cast<float>(windowScaleInPixels.x)+16.f), std::clamp(std::round(windowScaleInPixels.y - pos.y) + Level::BLOCK_SIZE / 2, -16.f, static_cast<float>(windowScaleInPixels.y) + 16.f));

		int blockX = static_cast<int>(std::round(float(correctedPositionTank.x) / 16.f));
		int blockY = static_cast<int>(std::round(float(correctedPositionTank.y) / 16.f) - 1);

		glm::vec2 scaleScreenInBlock = m_pGameManager->getScaleScreenInBlocks();
		if (blockX < 0)
		{
			return  m_pGameManager->getObjectByIndex(int(scaleScreenInBlock.x * scaleScreenInBlock.y));
		}
		if (blockY < 0)
		{
			return  m_pGameManager->getObjectByIndex(int(scaleScreenInBlock.x * scaleScreenInBlock.y + 1));
		}
		if (blockX >= scaleScreenInBlock.x)
		{
			return  m_pGameManager->getObjectByIndex(int(scaleScreenInBlock.x * scaleScreenInBlock.y + 2));
		}
		if (blockY >= scaleScreenInBlock.y)
		{
			return  m_pGameManager->getObjectByIndex(int(scaleScreenInBlock.x * scaleScreenInBlock.y + 3));
		}


		IGameObject* res = m_pGameManager->getObjectByIndex(static_cast<unsigned int>((blockY) * m_pGameManager->getScaleScreenInBlocks().x + blockX));

		return res;
	}

	int PhysicsEngine::getObjectIndexByPos(glm::vec2 pos)
	{
		glm::vec2 windowScaleInPixels = m_pGameManager->getScaleScreenInPixels();
		glm::ivec2 correctedPositionTank = glm::vec2(std::clamp(std::round(pos.x - Level::BLOCK_SIZE), 0.f, static_cast<float>(windowScaleInPixels.x)), std::clamp(std::round(windowScaleInPixels.y - pos.y) + Level::BLOCK_SIZE / 2, 0.f, static_cast<float>(windowScaleInPixels.y)));

		int blockX = static_cast<int>(std::round(float(correctedPositionTank.x) / 16.f));
		int blockY = static_cast<int>(std::round(float(correctedPositionTank.y) / 16.f));

		return static_cast<int>(blockX + (blockY - 1) * m_pCurrentLevel->getSizeLevelInBlocks().x);
			
	}


	glm::vec2 PhysicsEngine::alignPositionByPixels(IGameObject* object, float pixelsAlign)
	{
		/*if (object->getCurrentDirection().x != 0.f)
		{
			return glm::vec2(object->getTargetPosition().x, static_cast<unsigned int>(std::round(object->getTargetPosition().y / pixelsAlign) * pixelsAlign));
		}
		else if (object->getCurrentDirection().y != 0.f)
		{
			return glm::vec2(static_cast<unsigned int>(std::round(object->getTargetPosition().x / pixelsAlign) * pixelsAlign), object->getTargetPosition().y);
		}*/
		return glm::vec2(static_cast<unsigned int>(std::round(object->getTargetPosition().x / pixelsAlign) * pixelsAlign), static_cast<unsigned int>(std::round(object->getTargetPosition().y / pixelsAlign) * pixelsAlign));
	}

}