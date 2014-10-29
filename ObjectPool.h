#pragma once
#include "../../octet.h"
#include "IObjectPool.h"
#include "Enemy.h"
#include "Projectile.h"

namespace Arena
{
	class ObjectPool : public IObjectPool
	{
	private:
		octet::dynarray<Enemy*> activeObjects;
		octet::dynarray<Enemy*> inactiveObjects;

		octet::dynarray<Projectile*> activeProjectiles;
		octet::dynarray<Projectile*> inactiveProjectiles;

		GameWorldContext* gameWorldContext;

	public:
		octet::dynarray<PhysicsObject*> physicsObjects;
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

		void Initialise(GameWorldContext& context, int enemyCapacity, int projectileCapacity)
		{
			gameWorldContext = &context;
			activeObjects.reserve(enemyCapacity);
			inactiveObjects.reserve(enemyCapacity);
			activeProjectiles.reserve(projectileCapacity);
			inactiveProjectiles.reserve(projectileCapacity);

			for (int i = 0; i < projectileCapacity; i++)
			{
				Projectile* proj = GetProjectileObject();
				proj->DestroyViaPool();
			}
			
			for (int i = 0; i < enemyCapacity; i++)
			{
				Enemy* enemy = GetEnemyObject();
				enemy->DestroyViaPool();
			}
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
				enemy->Translate(get_btVector3(position));
				inactiveObjects.pop_back();
			}
			else
			{
				//allocate new - try to avoid this
				enemy = new Enemy();
				enemy->addPhysicsObjectToWorld(*gameWorldContext);
			}

			activeObjects.push_back(enemy);
			enemy->Enable();
			return enemy;
		}

		Projectile* GetProjectileObject()
		{
			return GetProjectileObject(octet::vec3(0.0f, 0.0f, 0.0f));
		}

		Projectile* GetProjectileObject(octet::vec3 position)
		{
			Projectile *projectile = nullptr;
			if (inactiveProjectiles.size() > 0)
			{
				projectile = inactiveProjectiles[inactiveProjectiles.size() - 1];
				projectile->Translate(get_btVector3(position));
				inactiveProjectiles.pop_back();
			}
			else
			{
				//allocate new - try to avoid this
				projectile = new Projectile();
				projectile->Translate(get_btVector3(position));
				projectile->addPhysicsObjectToWorld(*gameWorldContext);
			}

			activeProjectiles.push_back(projectile);
			projectile->Enable();
			return projectile;
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

		void DestroyActiveProjectileObject(Projectile* projectile)
		{
			for (unsigned int i = 0; i < activeProjectiles.size(); i++)
			{
				if (activeProjectiles[i] == projectile)
				{
					projectile->Disable();
					inactiveProjectiles.push_back(activeProjectiles[i]);
					activeProjectiles.erase(i);
					return;
				}
			}
		}
	};
}

