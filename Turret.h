#pragma once
#define OCTET_BULLET 1
#include "../../octet.h"
#include "PhysicsObject.h"
#include "TurretBarrel.h"

namespace Arena
{
	class Turret : public PhysicsObject
	{
	private:
		octet::vec3 barrelOffset = octet::vec3(0.0f, 0.0f, -1.0f);

	public:
		static const char* referenceName;
		TurretBarrel* barrel;

		Turret(GameWorldContext& context)
		{
			Initialise(octet::vec3(0.0f, 0.0f, 0.0f));
		}

		virtual ~Turret()
		{
		}


		void Initialise(octet::vec3 position)
		{
			mass = 0.01f;
			octet::mesh *baseShape = new octet::mesh_sphere(octet::vec3(0.0f, 0.0f, 0.0f), 1.5f);
			mat = new octet::material(octet::vec4(1.0f, 1.0f, 0.33f, 1.0f));

			PhysicsObject::Initialise(position, baseShape, baseShape->get_bullet_shape());
		}

		void addPhysicsObjectToWorld(GameWorldContext& context) override
		{
			PhysicsObject::addPhysicsObjectToWorld(context);

			octet::material* mat = new octet::material(octet::vec4(1.0f, 0.54f, 0.2f, 0.1f));
			barrel = new TurretBarrel(context, node, mat, barrelOffset);
		}

		void FireProjectile(GameWorldContext& context)
		{
			barrel->Fire(context);
		}

		void Rotate(float amount)
		{
			SetDampening(0.0f);
			rigidBody->applyTorque(btVector3(0.0f, amount, 0.0f));
		}

		void SetDampening(float dampening)
		{
			rigidBody->setDamping(0.0f, dampening);
		}
	};
	const char * Turret::referenceName = "Turret";
}

