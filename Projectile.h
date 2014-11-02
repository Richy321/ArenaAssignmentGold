#pragma once
#include "PhysicsObject.h"
#include "Timer.h"

namespace Arena
{
	class Projectile : public PhysicsObject
	{
	private:
		unsigned int damage = 50;
		float createdTime = -1.0f;
		const float lifespan = 1.0f; //seconds
		static const char* referenceName;

	public:
		PhysicsObject *owner = nullptr;

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

		void Enable() override
		{
			PhysicsObject::Enable();
			createdTime = timer->GetRunningTime();
		}

		void DestroyViaPool()
		{
			objectPool->DestroyActiveProjectileObject(this);
			createdTime = -1.0f;
			owner = nullptr;
		}

		void Update() override
		{
			PhysicsObject::Update();
			if (createdTime >= 0.0f)
			{
				float runningTime = timer->GetRunningTime();
				if (runningTime > createdTime + lifespan)
					DestroyViaPool();
			}
		}
	};

	const char * Projectile::referenceName = "Projectile";
}

