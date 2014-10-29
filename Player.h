#pragma once
#define OCTET_BULLET 1
#include "../../octet.h"
#include "PhysicsObject.h"
#include "Turret.h"

namespace Arena
{
	class Player : public PhysicsObject
	{
	private:
		unsigned int health;
		octet::vec3 size;

	public:
		Player()
		{
			Initialise(octet::vec3(0.0f, 10.0f, -10.0f), octet::vec3(2.0f, 2.0f, 2.0f));
		}
		~Player() {}

		//octet::vec3 right = octet::vec3(1, 0, 0);
		
		//octet::vec3 up = octet::vec3(0, 1, 0);

		float speed;

		Turret *turret;

		static const char* referenceName;

		virtual const char* GetReferenceType() override
		{
			return Player::referenceName;
		}

		void addXZConstraint(GameWorldContext &context)
		{
			btTransform local;
			local.setIdentity();


			btGeneric6DofConstraint* constr = new btGeneric6DofConstraint(*rigidBody, local, true);

			context.physicsWorld.addConstraint(constr);

			constr->setLinearLowerLimit(btVector3(-1000, -1000, -1000));
			constr->setLinearUpperLimit(btVector3(1000, 1000, 1000));

			constr->setAngularLowerLimit(btVector3(-SIMD_PI * 0.25f, 0, -SIMD_PI * 0.25f));
			constr->setAngularUpperLimit(btVector3(SIMD_PI * 0.25f, 0, SIMD_PI * 0.25f));


		}

		void addTurretConstraint(GameWorldContext &context)
		{
			btVector3 axis = btVector3(0.0f, 1.0f, 0.0f);
			btVector3 connectionPointPlayer = btVector3(0.0f, size.y(), 0.0f);
			btVector3 connectionPointBase = btVector3(0.0f, 0.0f, 0.0f);

			btHingeConstraint *turretBaseContraint = new btHingeConstraint(*rigidBody, *turret->GetRigidBody(), connectionPointPlayer, connectionPointBase, axis, axis);
			context.physicsWorld.addConstraint(turretBaseContraint);
		}
		virtual void Initialise(octet::vec3 position, octet::vec3 size)
		{
			collisionType = CollisionFlags::CollisionTypes::COL_PLAYER;
			collisionMask = CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_POWERUP;
			
			mat = new octet::material(octet::vec4(0.0f, 0.75f, 0.0f, 1.0f));
			octet::mesh *shape = new octet::mesh_box(size);
			
			this->size = size;
			btBoxShape *collisionShape = new btBoxShape(get_btVector3(size));

			PhysicsObject::Initialise(position, shape, collisionShape, mat);

			rigidBody->setActivationState(DISABLE_DEACTIVATION);
			speed = 25.0f;
			health = 100;

			rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() |
				btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		}

		void addPhysicsObjectToWorld(GameWorldContext& context) override
		{
			addXZConstraint(context);
			PhysicsObject::addPhysicsObjectToWorld(context);
			
			turret = new Turret(context);
			turret->addPhysicsObjectToWorld(context);
			addTurretConstraint(context);
		}

		void Update()
		{
			PhysicsObject::Update();
		}

		void Move(octet::vec3 moveVec)
		{
			moveVec.normalize();
			btVector3 force = btVector3(moveVec.x() * speed, moveVec.y() * speed, moveVec.z() * speed);
			rigidBody->applyForce(force, btVector3(0, 0, 0));
		}

		void RotateTurret(float amount)
		{
			turret->Rotate(amount);
		}
		
		octet::vec3 GetPosition()
		{
			return node->access_nodeToParent()[3].xyz();
		}
		
		unsigned int GetHealth()
		{
			return health;
		}

		void takeDamage(unsigned int damageValue)
		{
			health -= damageValue;
			mat->set_diffuse(octet::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		}
		
		void LookAt(octet::vec3 target)
		{
			
		}

		void FireTurrets(GameWorldContext& context)
		{
			turret->FireProjectile(context);
		}
	};
	const char * Player::referenceName = "Player";
}

