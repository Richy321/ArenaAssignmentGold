#pragma once

namespace Arena
{
	class Wall : public PhysicsObject
	{
	public:

		const float thickness = 0.5f;

		Wall(octet::mat4t modelToWorld)
		{
			Initialise(modelToWorld);
		}

		virtual ~Wall()
		{
		
		}

		void Initialise(octet::mat4t modelToWorld)
		{
			mass = 0;


			InitialiseRigidBody(modelToWorld, collisionShape);



		}
	};
}

