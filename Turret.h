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
			//barrel->addPhysicsObjectToWorld(context);

//			btVector3 axis = btVector3(0.0f, 0.0f, 1.0f);
//			btVector3 connectionPointBase = btVector3(0.0f, 0.0f, 0.0f);
//			btVector3 connectionPointBarrel = btVector3(0.0f, 0.0f, barrel->halfExtents);
//			btHingeConstraint *barrelBaseConstraint = new btHingeConstraint(*rigidBody, *barrel->GetRigidBody(), connectionPointBase, connectionPointBarrel, axis, axis);
//			context.physicsWorld.addConstraint(barrelBaseConstraint);
		}

		void FireProjectile(GameWorldContext& context)
		{
			barrel->Fire(context);
		}

		void Rotate(float amount)
		{
			rigidBody->applyTorque(btVector3(0.0f, amount, 0.0f));
		}

		/*
		virtual void Update()
		{
			PhysicsObject::Update();
			btTransform transform = rigidBody->getWorldTransform();

			rigidBody->setWorldTransform(transform);
			rigidBody->translate()
		}*/
	};
	const char * Turret::referenceName = "Turret";
}

