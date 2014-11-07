#pragma once
#include "ExplodeEnemy.h"
namespace Arena
{
	class SphereEnemy : public ExplodeEnemy
	{
	public:

		SphereEnemy()
		{
			Initialise();
		}

		virtual ~SphereEnemy()
		{
		}

		virtual void Initialise() override
		{
			octet::vec3 position = octet::vec3(0.0f, 0.0f, 0.0f);

			InitialiseExplodeShaderMaterial();

			octet::mesh *shape = new octet::mesh_sphere(octet::vec3(0.0f, 0.0f, 0.0f), 2.5f, 5);

			PhysicsObject::Initialise(position, shape, shape->get_bullet_shape(), mat);
			rigidBody->setActivationState(DISABLE_DEACTIVATION);

			Reset();
		}
	};
}

