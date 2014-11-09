#pragma once
#include "PhysicsObject.h"
#include "Player.h"

namespace Arena
{
	//Base powerup class which plays a sound on collection. Used to dervive other powerups.
	class PowerUp : public PhysicsObject
	{
	public:

		enum Type
		{
			None,
			AdditionalBarrel,
			Health
		};

		Type type = None;

		SoundManager& soundManager;
		PowerUp(SoundManager &sound) : soundManager(sound)
		{
		}

		virtual ~PowerUp()
		{
		}

		virtual void Initialise(octet::vec3 position, octet::mesh *shape = nullptr, btCollisionShape* collisionShape = nullptr, octet::material* mat = nullptr) override
		{
			collisionType = CollisionFlags::CollisionTypes::COL_POWERUP;
			collisionMask = CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_PLAYER | CollisionFlags::CollisionTypes::COL_PROJECTILES;
			PhysicsObject::Initialise(position, shape, collisionShape, mat);
		}

		virtual void DestroyViaPool() = 0;
		virtual void ApplyPowerUp(Player& player) = 0;
		virtual void OnHit()
		{
			soundManager.Play(soundManager.pickupSound);
			DestroyViaPool();
		}
	};
}

