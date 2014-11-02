#pragma once
#include "PhysicsObject.h"

namespace Arena
{
	class Enemy : public PhysicsObject
	{
	public:
		octet::vec3 up = octet::vec3(0, 1, 0);
		octet::vec3 right = octet::vec3(0, 1, 0);
		octet::vec3 forward = octet::vec3(0, 0, 1);
		float speed;
		
		enum AIMode
		{
			Idle,
			Chase
		};

	private:
		unsigned int health = 100;
		int damage = 5;
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

			speed = 25.0f;
			maxSpeed = 100.0f;
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
			case Chase:
				if (target != nullptr)
				{
					octet::vec3 moveDir = target->GetPosition() - GetPosition();
					moveDir = moveDir.normalize();
					moveDir *= speed;
					rigidBody->applyForce(get_btVector3(moveDir), btVector3(0.0f, 0.0f, 0.0f));
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
	};
	const char * Enemy::referenceName = "Enemy";
}
