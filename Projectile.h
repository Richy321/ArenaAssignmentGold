#pragma once
#include "PhysicsObject.h"

namespace Arena
{
	class Projectile : public PhysicsObject
	{
	private:
		unsigned int damage = 50;
		static const char* referenceName;
	public:

		virtual const char* GetReferenceType() override
		{
			return Projectile::referenceName;
		}

		Projectile()
		{
			Initialise(octet::vec3(0.0f, 0.0f, 0.0f));
		}

		virtual ~Projectile()
		{

		}

		virtual void Initialise(octet::vec3 position)
		{
			collisionType = CollisionFlags::CollisionTypes::COL_PROJECTILES;
			collisionMask = CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_ENEMY;

			float radius = 0.5f;
			mat = new octet::material(octet::vec4(0.0f, 0.75f, 0.0f, 1.0f));
			octet::mesh *shape = new octet::mesh_sphere(octet::vec3(0.0f, 0.0f, 0.0f), radius);
			btCollisionShape *collisionShape = new btSphereShape(btScalar(radius));

			PhysicsObject::Initialise(position, shape, collisionShape, mat);
		}

		void DestroyViaPool()
		{
			objectPool->DestroyActiveProjectileObject(this);
			Disable();
		}
	};

	const char * Projectile::referenceName = "Projectile";
}

