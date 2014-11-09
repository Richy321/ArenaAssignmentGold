#pragma once
#include "../../octet.h"
#include "PhysicsObject.h"
#include "CollisionFlags.h"

namespace Arena
{
	///Basic static physic object representing the arena floor
	class Floor : public PhysicsObject
	{
	private:
	public:
		const float thickness = 0.5f; ///floor thickness to prevent fall-through between physics steps

		Floor(float width, float height, GameWorldContext& context)
		{
			Initialise(octet::vec3(0.0f, 0.0f, 0.0f), octet::vec3(width, thickness, height), context);
		}

		static const char* referenceName;

		virtual const char* GetReferenceType()
		{
			return referenceName;
		}

		virtual void Initialise(octet::vec3 position, octet::vec3 size, GameWorldContext& context)
		{
			mass = 0.0f;
			collisionType = CollisionFlags::CollisionTypes::COL_WALL;
			collisionMask = CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_PLAYER | 
				CollisionFlags::CollisionTypes::COL_PROJECTILES | CollisionFlags::CollisionTypes::COL_POWERUP;
			
			mat = new octet::material(context.objectPool.GetTexture("Forerunner"));
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

