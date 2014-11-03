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
		octet::vec3 barrelOffset = octet::vec3(0.0f, 0.0f, -1.1f);

	public:
		static const char* referenceName;
		
		octet::dynarray<TurretBarrel*> barrels;
		PhysicsObject *owner;
		GameWorldContext* context;

		Turret(GameWorldContext& context, PhysicsObject *owner)
		{
			Initialise(octet::vec3(0.0f, 0.0f, 0.0f), owner);
		}

		virtual ~Turret()
		{
		}


		void Initialise(octet::vec3 position, PhysicsObject *owner)
		{
			mass = 0.01f;
			octet::mesh *baseShape = new octet::mesh_sphere(octet::vec3(0.0f, 0.0f, 0.0f), 1.5f);
			mat = new octet::material(octet::vec4(1.0f, 1.0f, 0.33f, 1.0f));
			this->owner = owner;

			PhysicsObject::Initialise(position, baseShape, baseShape->get_bullet_shape());
		}

		void addPhysicsObjectToWorld(GameWorldContext& context) override
		{
			this->context = &context;
			PhysicsObject::addPhysicsObjectToWorld(context);

			octet::material* mat = new octet::material(octet::vec4(1.0f, 0.54f, 0.2f, 0.1f));
			TurretBarrel *barrel = new TurretBarrel(context, node, mat, barrelOffset, owner);

			barrels.push_back(barrel);
		}

		void FireProjectile(GameWorldContext& context)
		{
			for (unsigned int i = 0; i < barrels.size(); i++)
				barrels[i]->Fire(context);
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

		void AddBarrel()
		{
			if (barrels.size() == 1)
			{
				octet::vec3 newOffset = barrelOffset;
				newOffset.x() += 0.5f;
				TurretBarrel *barrel = new TurretBarrel(*context, node, mat, newOffset, owner);
				barrels.push_back(barrel);
			}
			else if (barrels.size() == 2)
			{
				octet::vec3 newOffset = barrelOffset;
				newOffset.x() -= 0.5f;
				TurretBarrel *barrel = new TurretBarrel(*context, node, mat, newOffset, owner);
				barrels.push_back(barrel);
			}
		}
	};
	const char * Turret::referenceName = "Turret";
}

