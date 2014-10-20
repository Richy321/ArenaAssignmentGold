#pragma once
#include "PhysicsObject.h"
#include "../../octet.h"

namespace octet
{
	class Player : public PhysicsObject
	{
	private:
	public:
		Player()
		{
			Initialise(vec3(0.0f, 10.0f, -10.0f), vec3(2.0f, 2.0f, 2.0f));
		}
		~Player() {}

		vec3 right = vec3(1, 0, 0);
		vec3 up = vec3(0, 1, 0);
		vec3 forward = vec3(0, 0, 1);

		float speed;

		virtual void Initialise(vec3 position, vec3 size)
		{
			PhysicsObject::Initialise(position, size);
			speed = 25.0f;
		}

		void Update()
		{
			PhysicsObject::Update();
		}

		void Move(vec3 moveVec)
		{
			moveVec.normalize();
			btVector3 force = btVector3(moveVec.x() * speed, moveVec.y() * speed, moveVec.z() * speed);

			rigidBody->applyForce(force, btVector3(0, 0, 0));
		}

		vec3 GetPosition()
		{
			return node->access_nodeToParent()[3].xyz();
		}

		void LookAt(vec3 target)
		{

		}
	};
}