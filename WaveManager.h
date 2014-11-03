#pragma once
#include "PhysicsObject.h"
#include "PowerUp.h"

#include "AdditionalBarrel.h"
#include "Health.h"

namespace Arena
{
	class WaveManager
	{
	public:

		GameWorldContext &worldContext;
		int currentWave = 1;
		float arenaHeight;
		float arenaWidth;

		float maxObjectWidth = 5.0f;
		float maxObjectHeight = 5.0f;

		octet::random rnd = octet::random(33245532U);

		WaveManager(GameWorldContext& WorldContext, float ArenaHeight, float ArenaWidth) : worldContext(WorldContext), arenaHeight(ArenaHeight), arenaWidth(ArenaWidth)
		{
		
		}

		virtual ~WaveManager()
		{
		}

		void SpawnWave()
		{
			
		}

		void SpawnPhysicsExampleWave()
		{
			octet::mat4t modelToWorld;
			modelToWorld.translate(-4.5f, 10.0f, 0);
			octet::material *mat = new octet::material(octet::vec4(0, 1.0f, 0.5f, 1.0f));
			for (int i = 0; i != 20; ++i)
			{
				Enemy *enemy = worldContext.objectPool.GetEnemyObject();
				modelToWorld.translate(3, 0, 0);
				modelToWorld.rotateZ(360 / 20);
				enemy->SetWorldTransform(modelToWorld);
			}

			SpawnPowerUp(octet::vec3(15.0f, 1.0f, 15.0f), PowerUp::Type::AdditionalBarrel);
			SpawnPowerUp(octet::vec3(-15.0f, 1.0f, -15.0f), PowerUp::Type::Health);
		 }

		octet::vec3 GetRandomSpawnLocation()
		{
			rnd.get(arenaWidth - maxObjectWidth, arenaHeight - maxObjectWidth);
		}

		///Random function to spawn powerups.
		void SpawnPowerUp()
		{
			int powerUpNum = rnd.get(PowerUp::Type::AdditionalBarrel, PowerUp::Type::Health);
			SpawnPowerUp(GetRandomSpawnLocation(), (PowerUp::Type)powerUpNum);
		}

		///Specific spawn of powerups
		void SpawnPowerUp(octet::vec3 position, PowerUp::Type powerupType)
		{
			PowerUp* powerup = nullptr;
			switch (powerupType)
			{
			case PowerUp::Type::AdditionalBarrel:
				powerup = (PowerUp*)worldContext.objectPool.GetAdditionalBarrelObject();
				break;
			case PowerUp::Type::Health:
				powerup = (PowerUp*)worldContext.objectPool.GetHealthObject();
				break;
			}

			if (powerup != nullptr)
			{
				powerup->Translate(position);
			}
		}
	};
}

