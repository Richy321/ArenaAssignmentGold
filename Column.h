#pragma once
#include "PhysicsObject.h"

namespace Arena
{
	class Column : public PhysicsObject
	{
	public:
		static const char* referenceName;
		virtual const char* GetReferenceType()
		{
			return referenceName;
		}

		Column(octet::mat4t modelToWorld, float radius, float halfExtents, GameWorldContext& context)
		{
			Initialise(modelToWorld, radius, halfExtents, context);
		}

		virtual ~Column()
		{
		}


		void Initialise(octet::mat4t modelToWorld, float radius, float halfExtents, GameWorldContext& context)
		{
			octet::mat4t transform;
			transform.rotateX90();

			transform = transform * modelToWorld; //rotate first

			collisionType = CollisionFlags::CollisionTypes::COL_WALL;

			collisionMask = CollisionFlags::CollisionTypes::COL_PLAYER |
				CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_PROJECTILES |
				CollisionFlags::CollisionTypes::COL_POWERUP;
			mass = 0;

			//mat = new octet::material(octet::vec4(0.0f, 0.25f, 1.0f, 1.0f));
			mat = new octet::material(context.objectPool.GetTexture("SciFiCaution"));
			octet::mesh *shape = new octet::mesh_cylinder(octet::math::zcylinder(octet::vec3(0.0f, 0.0f, 0.0f), radius, halfExtents));


			PhysicsObject::Initialise(octet::vec3(0.0f, 0.0f, 0.0f), shape, shape->get_bullet_shape(), mat);
			rigidBody->setRestitution(1.0f);

			this->SetWorldTransform(transform);
		}
	};
	const char * Column::referenceName = "Column";
}

