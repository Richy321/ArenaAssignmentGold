#pragma once
#include "PhysicsObject.h"
#include "PowerUp.h"
#include "ArenaLayout.h"

#include "AdditionalBarrel.h"
#include "Health.h"
#include "GameMode.h"

namespace Arena
{
	///Manages enemy and powerup spawning, enemy target switching and difficulty
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
		const int maxEnemyCount = 20;
		const int minEnemyCount = 1;

		float remainingWaveDelayTime = 0.0f;

		enum state
		{
			Active,
			BetweenWaves,
			Inactive
		}
		state = Inactive;

		WaveManager(ArenaLayout& Arena, GameWorldContext& context) : arena(Arena)
		{
			Initialise(context);
		}

		virtual ~WaveManager()
		{
		}

		void Initialise(GameWorldContext& context)
		{
			context.rnd = octet::random(context.timer.GetTime());
		}

		///Spawns a new wave with random enemy positions and scaled difficulty
		void SpawnWave(GameWorldContext& context, Player* target, Player* target2, GameMode mode)
		{
			octet::mat4t modelToWorld;
			float multiplyer = 1 + currentWave * 0.2f;
			int enemyCount = octet::math::max(minEnemyCount, octet::math::min(maxEnemyCount, (int)(5 * multiplyer)));

			int fastEnemies = currentWave / 5;

			for (int i = 0; i < enemyCount; i++)
			{
				Enemy *enemy = context.objectPool.GetEnemyObject(); 
				octet::vec3 pos = arena.GetRandomSpawnLocation();
				modelToWorld.loadIdentity();
				modelToWorld.translate(pos.x(), pos.y(), pos.z());
				enemy->SetWorldTransform(modelToWorld);
				enemy->SetAIMode(Enemy::Chase);
				if (fastEnemies > 0)
				{
					enemy->SetSpeed(1 + enemy->GetSpeed() * 0.2f);
					fastEnemies--;
				}

				Player *curTarget = nullptr;
				if (mode == Solo)
					curTarget = target;
				else if (mode == Coop)
				{
					bool target1Alive = target->GetRemainingLives() >= 0;
					bool target2Alive = target2->GetRemainingLives() >= 0;
					if (target1Alive && target2Alive)
					{
						int rndInt = context.rnd.get(0, 2);
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

		///Random function to spawn powerups.
		void SpawnRandomPowerUp(GameWorldContext context)
		{
			lastPowerUpSpawnTime = context.timer.GetRunningTime();
			int powerUpNum = context.rnd.get(PowerUp::Type::AdditionalBarrel, PowerUp::Type::Health +1);
			SpawnPowerUp(arena.GetRandomSpawnLocation(), (PowerUp::Type)powerUpNum, context);
		}

		///Specific spawn of powerups
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
				octet::mat4t transform;
				transform.translate(position.x(), position.y(), position.z());
				powerup->SetWorldTransform(transform);
			}
		}

		///Checks to see if a new wave needs to be spawned and re-arranges enemy targets based on player/s state.
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

				for (unsigned int i = 0; i < context.objectPool.GetActiveEnemyCount(); i++)
				{
					bool target1Alive = target != nullptr && target->curState != Player::State::Dead;
					bool target2Alive = target2 != nullptr && target2->curState != Player::State::Dead;

					//Check if enemy is targeting dead player 1, set to player 2 if possible
					if (target != nullptr && target == context.objectPool.GetActiveEnemies()[i]->GetTarget() && !target1Alive)
					{
						if (target2Alive)
							context.objectPool.GetActiveEnemies()[i]->SetTarget(target2);
						else
							context.objectPool.GetActiveEnemies()[i]->SetTarget(nullptr);
					}

					//Check if enemy is targeting dead player 2, set to player 1 if possible
					if (target2 != nullptr && target2 == context.objectPool.GetActiveEnemies()[i]->GetTarget() && !target2Alive)
					{
						if (target1Alive)
							context.objectPool.GetActiveEnemies()[i]->SetTarget(target);
						else
							context.objectPool.GetActiveEnemies()[i]->SetTarget(nullptr);
					}

					//check if any enemies without targets and apply
					if (context.objectPool.GetActiveEnemies()[i]->GetTarget() == nullptr)
					{
						if (target1Alive)
							context.objectPool.GetActiveEnemies()[i]->SetTarget(target);
						else if(target2Alive)
							context.objectPool.GetActiveEnemies()[i]->SetTarget(target2);
					}
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

