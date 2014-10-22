#pragma once
#include "../../octet.h"
#include "IObjectPool.h"
#include "Enemy.h"

namespace Arena
{
	class ObjectPool : IObjectPool
	{
	private:
		octet::dynarray<Enemy*> activeObjects;
		octet::dynarray<Enemy*> inactiveObjects;
	public:
		octet::dynarray<PhysicsObject*> physicsObjects;

		static ObjectPool& getInstance()
		{
			static ObjectPool instance;
			return instance;
		}

		void AddPhysicsObject(PhysicsObject* physObj) override
		{
			physicsObjects.push_back(physObj);
		}

		void UpdatePhysicsObjects() override
		{
			for (unsigned int i = 0; i < physicsObjects.size(); i++)
				physicsObjects[i]->Update();
		}

		ObjectPool(){}
		ObjectPool(ObjectPool const&);
		void operator=(ObjectPool const&);

		virtual ~ObjectPool() override { }

		void initialise(int enemyCapacity)
		{
			activeObjects.reserve(enemyCapacity);
			inactiveObjects.reserve(enemyCapacity);
		}

		Enemy* GetEnemyObject()
		{
			return GetEnemyObject(octet::vec3(0.0f, 0.0f, 0.0f));
		}

		Enemy* GetEnemyObject(octet::vec3 position)
		{
			Enemy* enemy = nullptr;
			if (inactiveObjects.size() > 0)
			{
				enemy = inactiveObjects[inactiveObjects.size() - 1];
				inactiveObjects.pop_back();
				return enemy;
			}
			else
			{
				//allocate new - try to avoid this
				enemy = new Enemy(position);
			}

			activeObjects.push_back(enemy);
			enemy->Enable();
			return enemy;
		}

		void DestroyActiveEnemyObject(Enemy* enemy)
		{
			for (unsigned int i = 0; i < activeObjects.size(); i++)
			{
				if (activeObjects[i] == enemy)
				{
					enemy->Disable();
					inactiveObjects.push_back(activeObjects[i]);
					activeObjects.erase(i);
					return;
				}
			}
		}
	};
}

