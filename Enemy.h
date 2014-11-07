#pragma once
#include "PhysicsObject.h"

namespace Arena
{
	class Enemy : public PhysicsObject
	{
	public:

		enum AIMode
		{
			Idle,
			DumbChase,
			Chase
			//Pathfind
			//Proximity
		};

	protected:
		int health = 100;
		int damage = 5;
		int points = 1;
		float speed;
		float baseSpeed;
		PhysicsObject *target = nullptr;
		AIMode mode = Idle;
		octet::vec4 originalColour;
		octet::vec4 damageColour;

	public:
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

			Initialise();
		}

		~Enemy()
		{

		}

		virtual void Initialise()
		{
			octet::vec3 position = octet::vec3(0.0f, 0.0f, 0.0f);
			octet::vec3 size = octet::vec3(2.5f);

			damageColour = octet::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			originalColour = octet::vec4(0.0f, 1.0f, 0.0f, 1.0f);

			mat = new octet::material(originalColour);
			octet::mesh *shape = new octet::mesh_box(size);
			btBoxShape *collisionShape = new btBoxShape(get_btVector3(size));

			PhysicsObject::Initialise(position, shape, collisionShape, mat);
			rigidBody->setActivationState(DISABLE_DEACTIVATION);

			
			baseSpeed = 10;
			speed = baseSpeed;
			maxSpeed = 30.0f;
		}

		void DestroyViaPool()
		{
			mat->set_diffuse(originalColour);
			objectPool->DestroyActiveEnemyObject(this);
			Disable();
		}

		void Update(GameWorldContext& context) override
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
					moveDir *= speed * 30;
					rigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
					rigidBody->applyImpulse(get_btVector3(moveDir), btVector3(0.0f, 0.0f, 0.0f));
				}
				break;
			}
			PhysicsObject::Update(context);
		}

		void TakeDamage(unsigned int damageValue)
		{
			health -= damageValue;
			mat->set_diffuse(damageColour);

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
