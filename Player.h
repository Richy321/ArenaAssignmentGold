#pragma once
#define OCTET_BULLET 1
#include "../../octet.h"
#include "PhysicsObject.h"
#include "CollisionFlags.h"

namespace Arena
{
	class Player : public PhysicsObject
	{
	private:
		unsigned int health;
		
	public:
		Player()
		{
			Initialise(octet::vec3(0.0f, 10.0f, -10.0f), octet::vec3(2.0f, 2.0f, 2.0f));
		}
		~Player() {}

		octet::vec3 right = octet::vec3(1, 0, 0);
		octet::vec3 up = octet::vec3(0, 1, 0);
		octet::vec3 forward = octet::vec3(0, 0, 1);

		float speed;

		static const char* referenceName;

		virtual const char* GetReferenceType() override
		{
			return Player::referenceName;
		}

		virtual void Initialise(octet::vec3 position, octet::vec3 size)
		{
			collisionType = CollisionFlags::CollisionTypes::COL_PLAYER;
			collisionMask = CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_POWERUP;
			health = 100;
			PhysicsObject::Initialise(position, size);
			rigidBody->setActivationState(DISABLE_DEACTIVATION);
			speed = 25.0f;

			rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() |
				btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		}

		void Update()
		{
			PhysicsObject::Update();
		}

		void Move(octet::vec3 moveVec)
		{
			moveVec.normalize();
			btVector3 force = btVector3(moveVec.x() * speed, moveVec.y() * speed, moveVec.z() * speed);
			rigidBody->applyForce(force, btVector3(0, 0, 0));
		}

		octet::vec3 GetPosition()
		{
			return node->access_nodeToParent()[3].xyz();
		}
		
		unsigned int GetHealth()
		{
			return health;
		}

		void takeDamage(unsigned int damageValue)
		{
			health -= damageValue;
			mat->set_diffuse(octet::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		}
		
		void LookAt(octet::vec3 target)
		{

		}
		void FireProjectile()
		{

		}
	};
}

const char * Arena::Player::referenceName = "Player";