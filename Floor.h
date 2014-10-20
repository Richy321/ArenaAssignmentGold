#pragma once
#include "PhysicsObject.h"
#include "../../octet.h"

namespace Arena
{
	class Floor : public PhysicsObject
	{
	private:
	public:

		Floor()
		{
			Initialise(octet::vec3(0.0f, 0.0f, 0.0f), octet::vec3(200.0f, 0.5f, 200.0f));
		}

		virtual void Initialise(octet::vec3 position, octet::vec3 size)
		{
			isDynamic = false;
			mat = new octet::material(octet::vec4(0.0f, 1.0f, 1.0f, 1.0f));
			PhysicsObject::Initialise(position, size);
			rigidBody->setFriction(0.7f);
		}

		~Floor()
		{
		}
	};
}

