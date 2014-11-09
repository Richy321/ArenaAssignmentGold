#pragma once
#include "PhysicsObject.h"
#include "Timer.h"

namespace Arena
{
	/// Represents a basic projectile. Will destroy via object pool after its lifespan is up.
	class Projectile : public PhysicsObject
	{
	private:
		unsigned int damage = 50; //damage applied on collision
		float createdTime = -1.0f;
		const float lifespan = 1.65f; //lifespan in seconds
		
	public:
		static const char* referenceName;
		PhysicsObject *owner = nullptr;

		virtual const char* GetReferenceType() override
		{
			return Projectile::referenceName;
		}
		
		unsigned int GetDamage() { return damage; }

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
			collisionMask = CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_PLAYER;

			float radius = 0.5f;
			mat = new octet::material(octet::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			octet::mesh *shape = new octet::mesh_sphere(octet::vec3(0.0f, 0.0f, 0.0f), radius, 1);
			btCollisionShape *collisionShape = new btSphereShape(btScalar(radius));
			shape->set_mode(GL_LINES);
			PhysicsObject::Initialise(position, shape, collisionShape, mat);

			rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() |
				btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

			rigidBody->setRestitution(1.0f);
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

		void Update(GameWorldContext& context) override
		{
			PhysicsObject::Update(context);
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

