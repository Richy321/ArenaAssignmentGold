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
		const float thickness = 0.5f;

		Floor(float width, float height)
		{
			Initialise(octet::vec3(0.0f, 0.0f, 0.0f), octet::vec3(width, thickness, height));
		}

		static const char* referenceName;

		virtual const char* GetReferenceType()
		{
			return referenceName;
		}

		virtual void Initialise(octet::vec3 position, octet::vec3 size)
		{
			mass = 0.0f;
			collisionType = CollisionFlags::CollisionTypes::COL_WALL;
			collisionMask = CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_PLAYER | 
				CollisionFlags::CollisionTypes::COL_PROJECTILES | CollisionFlags::CollisionTypes::COL_POWERUP;
			
			octet::image *bitmap = new octet::image("src/examples/arena/assets/asphaltSeemless_alt.jpg");
			mat = new octet::material(bitmap);
			octet::mesh *shape = new octet::mesh_box(size);
			btBoxShape *collisionShape = new btBoxShape(get_btVector3(size));

			PhysicsObject::Initialise(position, shape, shape->get_static_bullet_shape(), mat);

			rigidBody->setFriction(0.25f);
		}

		~Floor()
		{
		}
	};
	const char * Floor::referenceName = "Floor";
}

