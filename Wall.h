#pragma once
#include "PhysicsObject.h"

namespace Arena
{
	///Basic static physic object representing the arena wall, fully collidable
	class Wall : public PhysicsObject
	{
	private:
	public:
		static const char* referenceName;
		virtual const char* GetReferenceType()
		{
			return referenceName;
		}

		Wall(octet::mat4t modelToWorld, octet::vec3 size, GameWorldContext& context)
		{
			Initialise(modelToWorld, size, context);
		}

		virtual ~Wall()
		{
		
		}

		void Initialise(octet::mat4t modelToWorld, octet::vec3 size, GameWorldContext& context)
		{
			collisionType = CollisionFlags::CollisionTypes::COL_WALL;

			collisionMask = CollisionFlags::CollisionTypes::COL_PLAYER |
				CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_PROJECTILES |
				CollisionFlags::CollisionTypes::COL_POWERUP;

			mass = 0;
			mat = new octet::material(context.objectPool.GetTexture("DarkWall"));
			octet::mesh *shape = new octet::mesh_box(size);
			btBoxShape *collisionShape = new btBoxShape(get_btVector3(size));

			PhysicsObject::Initialise(octet::vec3(0.0f, 0.0f, 0.0f), shape, shape->get_static_bullet_shape(), mat);
			rigidBody->setRestitution(1.0f);

			this->SetWorldTransform(modelToWorld);
		}
	};
	const char * Wall::referenceName = "Wall";
}

