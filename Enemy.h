#pragma once
#include "PhysicsObject.h"

namespace Arena
{
	//Base class to represent an Enemy. Handles basic chase AI and state 
	class Enemy : public PhysicsObject
	{
	public:

		enum AIMode
		{
			Idle,
			Chase
		};

	protected:
		int baseHealth = 100; ///initial health value before powerups
		int health = baseHealth; ///current health value
		int damage = 25; ///damage done to players on collision
		float speed; ///current health value
		float baseSpeed; ///initial speed value before powerups
		PhysicsObject *target = nullptr; ///current AI target
		AIMode mode = Idle;
		octet::vec4 originalColour;
		octet::vec4 damageColour;
		float deadTime = 0.0f; ///the running time the enemy last died
		float deathDelay = 5.0f; ///delay before destroying after dying (allows for death animations)

	public:
		enum state
		{
			Alive,
			Dead
		} state = Alive;
		static const char* referenceName;

		virtual const char* GetReferenceType()
		{
			return Enemy::referenceName;
		}

		Enemy()
		{
			collisionType = CollisionFlags::CollisionTypes::COL_ENEMY;
			collisionMask = CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_PLAYER |
				CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_PROJECTILES;

			damageColour = octet::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			originalColour = octet::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		}

		~Enemy()
		{

		}

		virtual void Initialise()
		{
			octet::vec3 position = octet::vec3(0.0f, 0.0f, 0.0f);
			octet::vec3 size = octet::vec3(2.5f);

			mat = new octet::material(originalColour);
			octet::mesh *shape = new octet::mesh_box(size);
			btBoxShape *collisionShape = new btBoxShape(get_btVector3(size));

			PhysicsObject::Initialise(position, shape, collisionShape, mat);
			rigidBody->setActivationState(DISABLE_DEACTIVATION);
			
			Reset();
		}

		///Reset internal values (used when pooling)
		virtual void Reset()
		{
			baseSpeed = 10;
			speed = baseSpeed;
			maxSpeed = 30.0f;
			state = Alive;
		}

		///Sets the Dead state and destroys via object pool
		virtual void Die(GameWorldContext& context)
		{
			state = Dead;
			deadTime = context.timer.GetRunningTime();
			DestroyViaPool();
		}

		///disables enemy via the object pool
		virtual void DestroyViaPool()
		{
			mat->set_diffuse(originalColour);
			objectPool->DestroyActiveEnemyObject(this);
			Disable();
		}

		virtual void Update(GameWorldContext& context) override
		{
			switch (mode)
			{
			case Idle:
				break;
			case Chase:
				if (target != nullptr)
				{
					octet::vec3 moveDir = target->GetPosition() - GetPosition();
					moveDir = moveDir.normalize();
					moveDir *= speed * 30;
					rigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
					rigidBody->applyImpulse(get_btVector3(moveDir), btVector3(0.0f, 0.0f, 0.0f));
				}
				break;
			}
			if (state == Alive)
				if (health <= 0)
					Die(context);

			PhysicsObject::Update(context);
		}

		virtual void TakeDamage(unsigned int damageValue)
		{
			health -= damageValue;
			mat->set_diffuse(damageColour);
		}

		virtual void Enable()
		{
			PhysicsObject::Enable();
			Reset();
		}

		#pragma region Getter functions
		int GetDamage() { return damage; }
		int GetHealth() { return health; }
		AIMode GetAIMode() { return mode; }
		PhysicsObject* GetTarget() { return target; }
		float GetSpeed() { return speed; }
		#pragma endregion

		#pragma region Setter functions
		void SetAIMode(AIMode mode) { this->mode = mode; }
		void SetTarget(PhysicsObject *target){ this->target = target; }
		void SetSpeed(float speedValue) { speed = speedValue; }
		#pragma endregion
	};
	const char *Enemy::referenceName = "Enemy";
}
