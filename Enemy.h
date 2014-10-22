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

		Enemy(octet::vec3 position)
		{
			Initialise(position, octet::vec3(0.5f));
		}

		~Enemy()
		{

		}

		void Initialise(octet::vec3 position, octet::vec3 size)
		{
			collisionType = CollisionFlags::CollisionTypes::COL_ENEMY;
			collisionMask = CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_PLAYER |
				CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_PROJECTILES;

			PhysicsObject::Initialise(position, size);
			speed = 25.0f;
		}

		void Enable()
		{
			rigidBody->setActivationState(ACTIVE_TAG);
		}

		void Disable()
		{
			rigidBody->setActivationState(DISABLE_SIMULATION);
			rigidBody->translate(btVector3(1000, 1000, -1000));
		}

		int getDamage() { return damage; }
		int getHalth() { return health; }
	};
}
const char * Arena::Enemy::referenceName = "Enemy";