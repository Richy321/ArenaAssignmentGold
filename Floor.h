#pragma once
#include "../../octet.h"
#include "PhysicsObject.h"
#include "CollisionFlags.h"


namespace Arena
{
	class Floor : public PhysicsObject
	{
	private:
	public:

		Floor()
		{
			Initialise(octet::vec3(0.0f, 0.0f, 0.0f), octet::vec3(200.0f, 0.5f, 200.0f));
		}

		static const char* referenceName;

		virtual const char* GetReferenceType()
		{
			return referenceName;
		}

		virtual void Initialise(octet::vec3 position, octet::vec3 size)
		{
			isDynamic = false;
			collisionType = CollisionFlags::CollisionTypes::COL_WALL;
			collisionMask = CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_PLAYER | CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_PROJECTILES;
			
			mat = new octet::material(octet::vec4(0.0f, 1.0f, 1.0f, 1.0f));
			octet::mesh *shape = new octet::mesh_box(size);
			btBoxShape *collisionShape = new btBoxShape(get_btVector3(size));

			PhysicsObject::Initialise(position, shape, collisionShape, mat);

			rigidBody->setFriction(0.5f);
		}

		~Floor()
		{
		}
	};
	const char * Floor::referenceName = "Floor";
}

