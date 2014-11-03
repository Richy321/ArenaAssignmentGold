#pragma once
#include "PowerUp.h"

namespace Arena
{
namespace PowerUps
{
	class AdditionalBarrel : public PowerUp
	{
	public:
		AdditionalBarrel()
		{
			Initialise(octet::vec3(0.0f, 0.5f, 0.0f));
		}

		virtual ~AdditionalBarrel()
		{
		}

		virtual void Initialise(octet::vec3 position)
		{
			collisionType = CollisionFlags::CollisionTypes::COL_POWERUP;
			collisionMask = CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_PLAYER | CollisionFlags::CollisionTypes::COL_PROJECTILES;

			float radius = 0.5f;
			float halfExtents = 2.25f;
			octet::mesh *barrelShape = new octet::mesh_cylinder(octet::math::zcylinder(octet::vec3(0.0f, 0.0f, 0.0f), radius, halfExtents));
			btSphereShape *collisionShape = new btSphereShape(halfExtents);

			PowerUp::Initialise(position, barrelShape, collisionShape, mat);
		}

		virtual const char* GetReferenceType()
		{
			return AdditionalBarrel::referenceName;
		}

		virtual void ApplyPowerUp(Player& player)
		{
			player.AddBarrel();
		}

		virtual void DestroyViaPool()
		{
			objectPool->DestroyActiveAdditionalBarrelObject(this);
			Disable();
		}
	};
}
}

