#pragma once
#include "Enemy.h"

namespace Arena
{
	class SpotEnemy : public Enemy
	{
	public:

		SpotEnemy()
		{
		}

		virtual ~SpotEnemy()
		{
		}

		void Initialise() override
		{
			octet::vec3 position = octet::vec3(0.0f, 0.0f, 0.0f);
			octet::vec3 size = octet::vec3(2.5f);

			damageColour = octet::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			originalColour = octet::vec4(0.0f, 1.0f, 0.0f, 1.0f);

			mat = new octet::material(originalColour);
			octet::mesh *shape = new octet::mesh_box(size);
			btBoxShape *collisionShape = new btBoxShape(get_btVector3(size));

			PhysicsObject::Initialise(position, shape, collisionShape, mat);
			rigidBody->setActivationState(DISABLE_DEACTIVATION);


			baseSpeed = 10;
			speed = baseSpeed;
			maxSpeed = 30.0f;

		}
	};
}

