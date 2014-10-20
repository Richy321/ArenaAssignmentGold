#pragma once
#include "PhysicsObject.h"
#include "../../octet.h"

namespace Arena
{
	class Player : public PhysicsObject
	{
	private:
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

		virtual void Initialise(octet::vec3 position, octet::vec3 size)
		{
			
			collisionType = CollisionFlags::CollisionTypes::COL_PLAYER;
			collisionMask = CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_POWERUP;

			PhysicsObject::Initialise(position, size);
			rigidBody->setActivationState(DISABLE_DEACTIVATION);
			speed = 25.0f;
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

		void LookAt(octet::vec3 target)
		{

		}
	};
}