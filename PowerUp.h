#pragma once
#include "PhysicsObject.h"
#include "Player.h"

namespace Arena
{
	class PowerUp : public PhysicsObject
	{
	public:

		enum Type
		{
			None,
			AdditionalBarrel,
			Health
			//Speed,
			//Firerate,
			//Vortex
		};

		Type type = None;
		PowerUp()
		{
		}

		virtual ~PowerUp()
		{
		}

		virtual void Initialise(octet::vec3 position, octet::mesh *shape = nullptr, btCollisionShape* collisionShape = nullptr, octet::material* mat = nullptr) override
		{
			PhysicsObject::Initialise(position, shape, collisionShape, mat);
		}

		virtual void DestroyViaPool() = 0;
		virtual void ApplyPowerUp(Player& player) = 0;
		virtual void OnHit()
		{
			DestroyViaPool();
		}
	};
}

