#pragma once
#include "../../octet.h"
#include "IObjectPool.h"
#include "Enemy.h"
#include "Projectile.h"
#include "PowerUp.h"
#include "AdditionalBarrel.h"
#include "Health.h"

#include "VaryingColourEnemy.h"
#include "ExplodeEnemy.h"
#include "SphereEnemy.h"

namespace Arena
{
	///Creates PhysicsObjects and Textures and adds then to a pool available for dynamic ingame use.
	///This stops memory allocation in the main game loop (slow)
	class ObjectPool : public IObjectPool
	{
	private:
		octet::dynarray<Enemy*> activeEnemies;
		octet::dynarray<Enemy*> inactiveEnemies;

		octet::dynarray<Projectile*> activeProjectiles;
		octet::dynarray<Projectile*> inactiveProjectiles;

		octet::dynarray<PowerUps::Health*> activeHealth;
		octet::dynarray<PowerUps::Health*> inactiveHealth;

		octet::dynarray<PowerUps::AdditionalBarrel*> activeAdditionalBarrel;
		octet::dynarray<PowerUps::AdditionalBarrel*> inactiveAdditionalBarrel;

		octet::dictionary<octet::ref<octet::image>> textures;

		GameWorldContext* gameWorldContext;

		octet::dynarray<PhysicsObject*> physicsObjects;

		//Prevent copying
		ObjectPool(ObjectPool const&); 
		void operator=(ObjectPool const&);
	public:
		
		ObjectPool(){}

		virtual ~ObjectPool() override { }

		void Initialise(GameWorldContext& context, int enemyCapacity, int projectileCapacity, int healthCapacity, int additionalBarrelCapacity)
		{
			gameWorldContext = &context;
			
			InitialiseTextures();

			activeEnemies.reserve(enemyCapacity);
			inactiveEnemies.reserve(enemyCapacity);
			activeProjectiles.reserve(projectileCapacity);
			inactiveProjectiles.reserve(projectileCapacity);
			activeHealth.reserve(healthCapacity);
			inactiveHealth.reserve(healthCapacity);
			activeAdditionalBarrel.reserve(additionalBarrelCapacity);
			inactiveAdditionalBarrel.reserve(additionalBarrelCapacity);


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

			for (int i = 0; i < healthCapacity; i++)
			{
				PowerUps::Health* health = CreateNewHealthPowerUp();
				health->Disable();
				inactiveHealth.push_back(health);
			}

			for (int i = 0; i < additionalBarrelCapacity; i++)
			{
				PowerUps::AdditionalBarrel* addBarrel = CreateNewAdditionalBarrelPowerUp();
				addBarrel->Disable();
				inactiveAdditionalBarrel.push_back(addBarrel);
			}
		}

		void InitialiseTextures()
		{
			octet::ref<octet::image> sciFiCaution = new octet::image("src/examples/arena/assets/images/sci_fi_caution2.jpg");
			textures["SciFiCaution"] = sciFiCaution;

			octet::ref<octet::image> forerunner = new octet::image("src/examples/arena/assets/images/Forerunner_2.jpg");
			textures["Forerunner"] = forerunner;

			octet::ref<octet::image> darkWall = new octet::image("src/examples/arena/assets/images/sci_fi_dark_wall_2.jpg");
			textures["DarkWall"] = darkWall;

			octet::ref<octet::image> sciFiCube = new octet::image("src/examples/arena/assets/images/explode_crate.jpg");
			textures["SciFiCube"] = sciFiCube;

			octet::ref<octet::image> skyTexture = new octet::image("src/examples/arena/assets/images/skyTexture.jpg");
			textures["SkyTexture"] = skyTexture;
		}

		///Get texture based on predefined string
		octet::image* GetTexture(const char* name)
		{
			if (textures.contains(name))
			{
				return textures[name];
			}
			return NULL;
		}

		void AddPhysicsObject(PhysicsObject* physObj) override
		{
			physicsObjects.push_back(physObj);
		}

		///call 'Update' on all PhysicsObjects created through the ObjectPool
		void UpdatePhysicsObjects(GameWorldContext& context) override
		{
			for (unsigned int i = 0; i < physicsObjects.size(); i++)
					physicsObjects[i]->Update(context);
		}

		#pragma region Enemy
		///Gets an unused Enemy object from pool or creates a new one and adds to pool if none are available
		Enemy* GetEnemyObject()
		{
			Enemy *enemy = GetEnemyObject(octet::vec3(0.0f, 0.0f, 0.0f));
			enemy->Reset();

			return enemy;
		}

		///Gets an unused Enemy object from pool or creates a new one and adds to pool if none are available.
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
				enemy = CreateNewEnemy();
			}

			activeEnemies.push_back(enemy);
			enemy->Enable();
			return enemy;
		}
		
		///Creates a new Enemy and adds to pool
		Enemy* CreateNewEnemy()
		{
			ExplodeEnemy* enemy = new ExplodeEnemy(*gameWorldContext);
			enemy->addPhysicsObjectToWorld(*gameWorldContext);
			return enemy;
		}
		octet::dynarray<Enemy*>& GetActiveEnemies() override { return activeEnemies; }
		
		///Removes active enemy from the active list and puts on the inactive list and disables.
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
		unsigned int GetActiveEnemyCount() override { return activeEnemies.size(); }
		unsigned int GetInactiveEnemyCount(){ return inactiveEnemies.size(); }
		#pragma endregion

		#pragma region Projectile
		///Gets an unused Projectile object from pool or creates a new one and adds to pool if none are available.
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
		Projectile* CreateNewProjectile()
		{
			Projectile *projectile = new Projectile();
			projectile->addPhysicsObjectToWorld(*gameWorldContext);
			return projectile;
		}
		octet::dynarray<Projectile*>& GetActiveProjectiles() { return activeProjectiles; }

		///Removes active projectile from the active list and puts on the inactive list and disables.
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
		unsigned int GetActiveProjectileCount() { return activeProjectiles.size(); }
		unsigned int GetInactiveProjectileCount() { return inactiveProjectiles.size(); }
		#pragma endregion

		#pragma region AdditionalBarrel
		///Gets an unused AdditionalBarrel object from pool or creates a new one and adds to pool if none are available.
		PowerUps::AdditionalBarrel* GetAdditionalBarrelObject()
		{
			PowerUps::AdditionalBarrel* addBarrel = nullptr;
			if (inactiveAdditionalBarrel.size() > 0)
			{
				addBarrel = inactiveAdditionalBarrel[inactiveAdditionalBarrel.size() - 1];
				inactiveAdditionalBarrel.pop_back();
			}
			else
			{
				//allocate new
				addBarrel = CreateNewAdditionalBarrelPowerUp();
			}

			activeAdditionalBarrel.push_back(addBarrel);
			addBarrel->Enable();
			
			return addBarrel;
		}
		PowerUps::AdditionalBarrel* CreateNewAdditionalBarrelPowerUp()
		{
			PowerUps::AdditionalBarrel* barrel = new PowerUps::AdditionalBarrel(gameWorldContext->soundManager);
			barrel->addPhysicsObjectToWorld(*gameWorldContext);
			return barrel;
		}

		///Removes active AdditionalBarrel from the active list and puts on the inactive list and disables.
		void DestroyActiveAdditionalBarrelObject(PowerUps::AdditionalBarrel* additionalBarrel)
		{
			for (unsigned int i = 0; i < activeAdditionalBarrel.size(); i++)
			{
				if (activeAdditionalBarrel[i] == additionalBarrel)
				{
					additionalBarrel->Disable();
					inactiveAdditionalBarrel.push_back(activeAdditionalBarrel[i]);
					activeAdditionalBarrel.erase(i);
					return;
				}
			}
		}
		unsigned int GetActiveAdditionalBarrelCount() { return activeAdditionalBarrel.size(); }
		unsigned int GetInactiveAdditionalBarrelCount(){ return inactiveAdditionalBarrel.size(); }
		#pragma endregion

		#pragma region Health
		///Gets an unused Health object from pool or creates a new one and adds to pool if none are available.
		PowerUps::Health* GetHealthObject()
		{
			PowerUps::Health* health = nullptr;
			if (inactiveHealth.size() > 0)
			{
				health = inactiveHealth[inactiveHealth.size() - 1];
				inactiveHealth.pop_back();
			}
			else
			{
				//allocate new - try to avoid this
				health = CreateNewHealthPowerUp();
			}

			activeHealth.push_back(health);
			health->Enable();
			return health;
		}
		PowerUps::Health* CreateNewHealthPowerUp()
		{
			PowerUps::Health* health = new PowerUps::Health(gameWorldContext->soundManager);
			health->addPhysicsObjectToWorld(*gameWorldContext);
			return health;
		}

		///Removes active Health from the active list and puts on the inactive list and disables.
		void DestroyActiveHealthObject(PowerUps::Health* health)
		{
			for (unsigned int i = 0; i < activeHealth.size(); i++)
			{
				if (activeHealth[i] == health)
				{
					health->Disable();
					inactiveHealth.push_back(activeHealth[i]);
					activeHealth.erase(i);
					return;
				}
			}
		}
		unsigned int GetActiveHealthCount() { return activeHealth.size(); }
		unsigned int GetInactiveHealthCount(){ return inactiveHealth.size(); }
		#pragma endregion
		void KillAllActiveEnemys()
		{
			for (int i = activeEnemies.size()-1; i >= 0; i--)
				activeEnemies[i]->DestroyViaPool();
		}
	};
}

