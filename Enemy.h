#pragma once
#include "PhysicsObject.h"

namespace Arena
{
	class Enemy : public PhysicsObject
	{
	private:
		unsigned int health = 100;
		int damage = 5;
	public:
		octet::vec3 up = octet::vec3(0, 1, 0);
		octet::vec3 right = octet::vec3(0, 1, 0);
		octet::vec3 forward = octet::vec3(0, 0, 1);
		float speed;

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
		}

		void DestroyViaPool()
		{
			objectPool->DestroyActiveEnemyObject(this);
			Disable();
		}

		int getDamage() { return damage; }
		int getHealth() { return health; }
	};
	const char * Enemy::referenceName = "Enemy";
}
