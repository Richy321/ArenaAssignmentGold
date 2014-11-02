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
		float speed;
		octet::vec3 size;

		float takeDamageStartTime = -1.0f;
		float takeDamageDuration = 0.5f;

		octet::material *originalMat;
		octet::material *damagedMat;

		octet::vec4 originalColour;
		octet::vec4 damageColour;

		bool isTakingDamage = false;
	public:
		Player()
		{
			Initialise(octet::vec3(0.0f, 10.0f, -10.0f), octet::vec3(2.0f, 2.0f, 2.0f));
		}
		~Player() {}

		Turret *turret;

		static const char* referenceName;

		virtual const char* GetReferenceType() override
		{
			return Player::referenceName;
		}

		virtual void Initialise(octet::vec3 position, octet::vec3 size)
		{
			collisionType = CollisionFlags::CollisionTypes::COL_PLAYER;
			collisionMask = CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_POWERUP;
			
			damageColour = octet::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			originalColour = octet::vec4(0.0f, 0.75f, 0.0f, 1.0f);

			//damagedMat = new octet::material(octet::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			mat = new octet::material(originalColour);
			octet::mesh *shape = new octet::mesh_box(size);
			
			this->size = size;
			btBoxShape *collisionShape = new btBoxShape(get_btVector3(size));

			PhysicsObject::Initialise(position, shape, collisionShape, mat);

			rigidBody->setActivationState(DISABLE_DEACTIVATION);

			rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() |
				btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

			speed = 500.0f;
			health = 100.0f;
		}
		void addXZConstraint(GameWorldContext &context)
		{
			btTransform local;
			local.setIdentity();

			btGeneric6DofConstraint* constr = new btGeneric6DofConstraint(*rigidBody, local, true);

			context.physicsWorld.addConstraint(constr);

			constr->setLinearLowerLimit(btVector3(-1000, -1000, -1000));
			constr->setLinearUpperLimit(btVector3(1000, 1000, 1000));

			constr->setAngularLowerLimit(btVector3(-SIMD_PI * 0.20f, 0, -SIMD_PI * 0.20f));
			constr->setAngularUpperLimit(btVector3(SIMD_PI * 0.20f, 0, SIMD_PI * 0.20f));
		}

		void addTurretConstraint(GameWorldContext &context)
		{
			btVector3 axis = btVector3(0.0f, 1.0f, 0.0f);
			btVector3 connectionPointPlayer = btVector3(0.0f, size.y(), 0.0f);
			btVector3 connectionPointBase = btVector3(0.0f, 0.0f, 0.0f);

			btHingeConstraint *turretBaseContraint = new btHingeConstraint(*rigidBody, *turret->GetRigidBody(), connectionPointPlayer, connectionPointBase, axis, axis);
			context.physicsWorld.addConstraint(turretBaseContraint);
		}

		void addPhysicsObjectToWorld(GameWorldContext& context) override
		{
			addXZConstraint(context);
			PhysicsObject::addPhysicsObjectToWorld(context);
			
			turret = new Turret(context, this);
			turret->addPhysicsObjectToWorld(context);
			
			btTransform turretTrans = rigidBody->getWorldTransform();
			btVector3 pos = rigidBody->getCenterOfMassPosition();
			//pos.setY(pos.y() + size.y()/2);
			turret->GetRigidBody()->setWorldTransform(turretTrans);
			addTurretConstraint(context);
		}

		void Update()
		{
			PhysicsObject::Update();

			if (isTakingDamage && timer->GetRunningTime() > takeDamageStartTime + takeDamageDuration)
			{
				isTakingDamage = false;
				mat->set_diffuse(originalColour);
			}
		}

		void Move(octet::vec3 moveVec)
		{
			SetMovementDampening(0.0f);
			moveVec.normalize();
			btVector3 force = btVector3(moveVec.x() * speed, moveVec.y() * speed, moveVec.z() * speed);
			rigidBody->applyForce(force, btVector3(0, 0, 0));
		}

		void RotateTurret(float amount)
		{
			turret->Rotate(amount);
		}
			
		unsigned int GetHealth()
		{
			return health;
		}

		void TakeDamage(unsigned int damageValue)
		{
			if (!isTakingDamage)
			{
				isTakingDamage = true;
				health -= damageValue;
				takeDamageStartTime = timer->GetRunningTime();
				mat->set_diffuse(damageColour);
			}
		}
		
		void LookAt(octet::vec3 target)
		{
			
		}

		void FireTurrets(GameWorldContext& context)
		{
			turret->FireProjectile(context);
		}

		void SetTurretDampening(float value)
		{
			turret->SetDampening(value);
		}

		void SetMovementDampening(float value)
		{
			rigidBody->setDamping(value, 0.0f);
		}
	};
	const char * Player::referenceName = "Player";
}

