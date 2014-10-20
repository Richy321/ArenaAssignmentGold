#pragma once
#include "PhysicsObject.h"
#include "../../octet.h"

namespace octet
{
	class Enemy : public PhysicsObject
	{
	private:

	public:
		vec3 up = vec3(0, 1, 0);
		vec3 right = vec3(0, 1, 0);
		vec3 forward = vec3(0, 0, 1);
		float speed;
		
		Enemy(vec3 position)
		{
			Initialise(position, vec3(0.5f));
		}

		~Enemy()
		{

		}

		void Initialise(vec3 position, vec3 size)
		{
			PhysicsObject::Initialise(position, size);
			speed = 25.0f;
		}
	};
}
