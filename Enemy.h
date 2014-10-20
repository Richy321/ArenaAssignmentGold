#pragma once
#include "PhysicsObject.h"
#include "../../octet.h"

namespace Arena
{
	class Enemy : public PhysicsObject
	{
	private:

	public:
		octet::vec3 up = octet::vec3(0, 1, 0);
		octet::vec3 right = octet::vec3(0, 1, 0);
		octet::vec3 forward = octet::vec3(0, 0, 1);
		float speed;
		
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
	};
}
