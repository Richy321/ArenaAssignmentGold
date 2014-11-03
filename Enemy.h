#pragma once
#include "PhysicsObject.h"

namespace Arena
{
	class Enemy : public PhysicsObject
	{
	public:
		float speed;
		
		enum AIMode
		{
			Idle,
			DumbChase,
			Chase
			//Pathfind
			//Proximity
		};

	private:
		unsigned int health = 100;
		int damage = 5;
		int points = 1;
		PhysicsObject *target = nullptr;
		AIMode mode = Idle;

	public:
		static const char* referenceName;

		virtual const char* GetReferenceType()
		{
			return Enemy::referenceName;
		}

		Enemy()
		{
			Initialise(octet::vec3(0.0f, 0.0f, 0.0f), octet::vec3(2.5f));
		}

		~Enemy()
		{

		}

		void Initialise(octet::vec3 position, octet::vec3 size)
		{
			collisionType = CollisionFlags::CollisionTypes::COL_ENEMY;
			collisionMask = CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_PLAYER |
				CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_PROJECTILES;

			mat = new octet::material(octet::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			octet::mesh *shape = new octet::mesh_box(size);
			btBoxShape *collisionShape = new btBoxShape(get_btVector3(size));

			PhysicsObject::Initialise(position, shape, collisionShape, mat);
			rigidBody->setActivationState(DISABLE_DEACTIVATION);

			speed = 20;
			maxSpeed = 30.0f;
		}

		void DestroyViaPool()
		{
			objectPool->DestroyActiveEnemyObject(this);
			Disable();
		}

		void Update() override
		{
			switch (mode)
			{
			case Idle:
				break;
			case DumbChase:
				if (target != nullptr)
				{
					octet::vec3 moveDir = target->GetPosition() - GetPosition();
					moveDir = moveDir.normalize();
					moveDir *= speed;
					rigidBody->applyForce(get_btVector3(moveDir), btVector3(0.0f, 0.0f, 0.0f));
				}
				break;
			case Chase:
				if (target != nullptr)
				{
					octet::vec3 moveDir = target->GetPosition() - GetPosition();
					moveDir = moveDir.normalize();
					moveDir *= speed * 50;
					rigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
					rigidBody->applyImpulse(get_btVector3(moveDir), btVector3(0.0f, 0.0f, 0.0f));
				}
				break;
			}
			PhysicsObject::Update();
		}

		void TakeDamage(unsigned int damageValue)
		{
			health -= damageValue;
			mat->set_diffuse(octet::vec4(1.0f, 0.0f, 0.0f, 1.0f));

			if (health <= 0)
				DestroyViaPool();
		}

		int GetDamage() { return damage; }
		int GetHealth() { return health; }
		int GetPoints() { return points;  }
		AIMode GetAIMode() { return mode; }
		PhysicsObject* GetTarget() { return target; }

		void SetAIMode(AIMode mode) { this->mode = mode; }
		void SetTarget(PhysicsObject *target){ this->target = target; }
	};
	const char *Enemy::referenceName = "Enemy";
}
