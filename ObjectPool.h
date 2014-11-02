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
		octet::dynarray<Enemy*> activeEnemies;
		octet::dynarray<Enemy*> inactiveEnemies;

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
			activeEnemies.reserve(enemyCapacity);
			inactiveEnemies.reserve(enemyCapacity);
			activeProjectiles.reserve(projectileCapacity);
			inactiveProjectiles.reserve(projectileCapacity);

			for (int i = 0; i < projectileCapacity; i++)
			{
				Projectile* proj = CreateNewProjectile();
				proj->Disable();
				inactiveProjectiles.push_back(proj);
			}
			
			for (int i = 0; i < enemyCapacity; i++)
			{
				Enemy* enemy = CreateNewEnemy();
				enemy->Disable();
				inactiveEnemies.push_back(enemy);
			}
		}

		Enemy* GetEnemyObject()
		{
			return GetEnemyObject(octet::vec3(0.0f, 0.0f, 0.0f));
		}

		Enemy* GetEnemyObject(octet::vec3 position)
		{
			Enemy* enemy = nullptr;
			if (inactiveEnemies.size() > 0)
			{
				enemy = inactiveEnemies[inactiveEnemies.size() - 1];
				enemy->Translate(get_btVector3(position));
				inactiveEnemies.pop_back();
			}
			else
			{
				//allocate new - try to avoid this
				enemy = new Enemy();
				enemy->addPhysicsObjectToWorld(*gameWorldContext);
			}

			activeEnemies.push_back(enemy);
			enemy->Enable();
			return enemy;
		}

		Enemy* CreateNewEnemy()
		{
			Enemy* enemy = new Enemy();
			enemy->addPhysicsObjectToWorld(*gameWorldContext);
			return enemy;
		}

		Projectile* CreateNewProjectile()
		{
			Projectile *projectile = new Projectile();
			projectile->addPhysicsObjectToWorld(*gameWorldContext);
			return projectile;
		}

		Projectile* GetProjectileObject()
		{
			Projectile *projectile = nullptr;
			if (inactiveProjectiles.size() > 0)
			{
				projectile = inactiveProjectiles[inactiveProjectiles.size() - 1];
				inactiveProjectiles.pop_back();
			}
			else
			{
				//allocate new - try to avoid this
				projectile = CreateNewProjectile();
			}

			activeProjectiles.push_back(projectile);
			projectile->Enable();
			return projectile;
		}

		void DestroyActiveEnemyObject(Enemy* enemy)
		{
			for (unsigned int i = 0; i < activeEnemies.size(); i++)
			{
				if (activeEnemies[i] == enemy)
				{
					enemy->Disable();
					inactiveEnemies.push_back(activeEnemies[i]);
					activeEnemies.erase(i);
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

		unsigned int GetActiveProjectileCount()
		{
			return activeProjectiles.size();
		}

		unsigned int GetInactiveProjectileCount()
		{
			return inactiveProjectiles.size();
		}

		unsigned int GetActiveEnemyCount()
		{
			return activeEnemies.size();
		}

		unsigned int GetInactiveEnemyCount()
		{
			return inactiveEnemies.size();
		}
	};
}

