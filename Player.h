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
			Initialise();
		}
		~Player() {}

		vec3 right = vec3(1, 0, 0);
		vec3 up = vec3(0, 1, 0);
		vec3 forward = vec3(0, 0, 1);

		float speed;

		void Initialise()
		{
			PhysicsObject::Initialise();

			speed = 25.0f;
		}

		void Update()
		{
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