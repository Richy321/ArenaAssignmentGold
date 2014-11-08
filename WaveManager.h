#pragma once
#include "PhysicsObject.h"
#include "PowerUp.h"
#include "ArenaLayout.h"

#include "AdditionalBarrel.h"
#include "Health.h"
#include "GameMode.h"

namespace Arena
{
	class WaveManager
	{
	private:
		float lastPowerUpSpawnTime = 0.0f;
		const float waveSpawnDelay = 5.0f;
		float lastWaveFinishedTime = -waveSpawnDelay;
	public:
		enum { maxPowerUpsPerType = 5 };

		ArenaLayout &arena;
		int currentWave = 0;
		const float powerUpSpawnFrequency = 20.0f;
		const int maxEnemyCount = 1;
		const int minEnemyCount = 1;

		float remainingWaveDelayTime = 0.0f;

		enum state
		{
			Active,
			BetweenWaves,
			Inactive
		}

		state = Inactive;
		octet::random rnd;

		WaveManager(ArenaLayout& Arena, GameWorldContext& context) : arena(Arena)
		{
			Initialise(context);
		}

		virtual ~WaveManager()
		{
		}


		void Initialise(GameWorldContext& context)
		{
			rnd = octet::random(context.timer.GetTime());
		}
		void SpawnWave(GameWorldContext& context, Player* target, Player* target2, GameMode mode)
		{
			octet::mat4t modelToWorld;
			float multiplyer = 1 + currentWave * 0.2f;
			int enemyCount = octet::math::max(minEnemyCount, octet::math::min(maxEnemyCount, (int)(5 * multiplyer)));
			for (int i = 0; i < enemyCount; i++)
			{
				Enemy *enemy = context.objectPool.GetEnemyObject();
				octet::vec3 pos = arena.GetRandomSpawnLocation();
				modelToWorld.loadIdentity();
				modelToWorld.translate(pos.x(), pos.y(), pos.z());
				enemy->SetWorldTransform(modelToWorld);
				enemy->SetAIMode(Enemy::Chase);

				Player *curTarget = nullptr;
				if (mode == Solo)
					curTarget = target;
				else if (mode == Coop)
				{
					bool target1Alive = target->GetRemainingLives() >= 0;
					bool target2Alive = target2->GetRemainingLives() >= 0;
					if (target1Alive && target2Alive)
					{
						int rndInt = rnd.get(0, 2);
						curTarget = rndInt == 0 ? target : target2;
					}
					else if (target1Alive)
						curTarget = target;
					else if (target2Alive)
						curTarget = target2;
					else
						curTarget = nullptr;
				}
				enemy->SetTarget(curTarget);
			}
			SpawnRandomPowerUp(context);
		}

		void SpawnPhysicsExampleWave(GameWorldContext& context)
		{
			octet::mat4t modelToWorld;
			modelToWorld.translate(-4.5f, 10.0f, 0);

			for (int i = 0; i != 20; ++i)
			{
				Enemy *enemy = context.objectPool.GetEnemyObject();
				modelToWorld.translate(3, 0, 0);
				modelToWorld.rotateZ(360 / 20);
				enemy->SetWorldTransform(modelToWorld);
			}

			SpawnPowerUp(octet::vec3(15.0f, 1.0f, 15.0f), PowerUp::Type::AdditionalBarrel, context);
			SpawnPowerUp(octet::vec3(-15.0f, 1.0f, -15.0f), PowerUp::Type::Health, context);
		}

		//Random function to spawn powerups.
		void SpawnRandomPowerUp(GameWorldContext context)
		{
			lastPowerUpSpawnTime = context.timer.GetRunningTime();
			int powerUpNum = rnd.get(PowerUp::Type::AdditionalBarrel, PowerUp::Type::Health +1);
			SpawnPowerUp(arena.GetRandomSpawnLocation(), (PowerUp::Type)powerUpNum, context);
		}

		//Specific spawn of powerups
		void SpawnPowerUp(octet::vec3 position, PowerUp::Type powerupType, GameWorldContext& context)
		{
			PowerUp* powerup = nullptr;
			switch (powerupType)
			{
			case PowerUp::Type::AdditionalBarrel:
				powerup = (PowerUp*)context.objectPool.GetAdditionalBarrelObject();
				break;
			case PowerUp::Type::Health:
				powerup = (PowerUp*)context.objectPool.GetHealthObject();
				break;
			}

			if (powerup != nullptr)
			{
				powerup->Translate(position);
			}
		}

		void Update(GameWorldContext& context, Player* target, Player* target2, GameMode mode)
		{
			if (state == Active)
			{
				remainingWaveDelayTime = 0.0f;
				if (context.objectPool.GetActiveEnemyCount() == 0)
				{
					state = BetweenWaves;
					lastWaveFinishedTime = context.timer.GetRunningTime();
					currentWave++;
				}
			}
			else if (state == BetweenWaves)
			{
				float curRunTime = context.timer.GetRunningTime();
				if (curRunTime > lastWaveFinishedTime + waveSpawnDelay)
				{
					SpawnWave(context, target, target2, mode);
					state = Active;
				}
				else
					remainingWaveDelayTime = lastWaveFinishedTime + waveSpawnDelay - curRunTime;
			}
		}
	};
}

