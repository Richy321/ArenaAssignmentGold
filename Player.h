#pragma once
#define OCTET_BULLET 1
#include "../../octet.h"
#include "PhysicsObject.h"
#include "Turret.h"

namespace Arena
{
	class Player : public PhysicsObject
	{
	public: 		
		enum Direction
		{
			North = 1,
			East,
			South,
			West
		};
	private:
		unsigned int health;
		float speed;
		float baseSpeed;
		float deceleration = 0.9f;

		octet::vec3 size;
		unsigned int barrelCount = 1;

		float takeDamageStartTime = -1.0f;
		float takeDamageDuration = 0.2f;

		octet::material *originalMat;
		octet::material *damagedMat;

		octet::vec4 originalColour;
		octet::vec4 damageColour;

		octet::hash_map<Direction, bool> activeAcceleration;
		bool isTakingDamage = false;

		octet::vec3 dampening;

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
			collisionMask = CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_POWERUP | CollisionFlags::CollisionTypes::COL_PLAYER | CollisionFlags::CollisionTypes::COL_PROJECTILES;
			
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
			health = 100;

			maxSpeed = 30;

			activeAcceleration[Direction::North] = false;
			activeAcceleration[Direction::East] = false;
			activeAcceleration[Direction::South] = false;
			activeAcceleration[Direction::West] = false;

		}
		void addXZConstraint(GameWorldContext &context)
		{
			btTransform local;
			local.setIdentity();

			btGeneric6DofConstraint* constr = new btGeneric6DofConstraint(*rigidBody, local, true);

			context.physicsWorld.addConstraint(constr);

			constr->setLinearLowerLimit(btVector3(-1000, -1000, -1000));
			constr->setLinearUpperLimit(btVector3(1000, 1000, 1000));

			constr->setAngularLowerLimit(btVector3(-SIMD_PI * 0.05f, 0, -SIMD_PI * 0.05f));
			constr->setAngularUpperLimit(btVector3(SIMD_PI * 0.05f, 0, SIMD_PI * 0.05f));
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

			UpdateAcceleration();

			activeAcceleration[Direction::North] = false;
			activeAcceleration[Direction::East] = false;
			activeAcceleration[Direction::South] = false;
			activeAcceleration[Direction::West] = false;
		}

		void Move(octet::vec3 moveVec, Direction accelDir)
		{
			activeAcceleration[accelDir] = true;
		}
		
		void UpdateAcceleration()
		{
			btVector3 linearVelocity = rigidBody->getLinearVelocity();
			octet::vec3 moveVec(0.0f, 0.0f, 0.0f);

			dampening = octet::vec3(0.0f, 0.0f, 0.0f);

			if (activeAcceleration[Direction::North] == true)
				moveVec += octet::vec3(0.0f, 0.0f, -1.0f);
			else if (linearVelocity.z() < 0)
				dampening.z() += linearVelocity.z() - (linearVelocity.z() * (deceleration));

			if (activeAcceleration[Direction::East] == true)
				moveVec += octet::vec3(-1.0f, 0.0f, 0.0f);
			else if (linearVelocity.x() < 0)
				dampening.x() += linearVelocity.x() - (linearVelocity.x() * (deceleration));

			if (activeAcceleration[Direction::South] == true)
				moveVec += octet::vec3(0.0f, 0.0f, 1.0f);
			else if (linearVelocity.z() > 0)
				dampening.z() += linearVelocity.z() - (linearVelocity.z() * (deceleration));

			if (activeAcceleration[Direction::West] == true)
				moveVec += octet::vec3(1.0f, 0.0f, 0.0f);
			else if (linearVelocity.x() > 0)
				dampening.x() += linearVelocity.x() - (linearVelocity.x() * (deceleration));

			moveVec.normalize();
			if (abs(moveVec.length()) > 0 || abs(dampening.length()) > 0)
			{
				if (abs(dampening.length()) > 0)
				{
					rigidBody->setLinearVelocity(rigidBody->getLinearVelocity() - get_btVector3(dampening));
				}
				btVector3 force;
				/*if (abs(dampening.length()) > 0)
					force = btVector3((moveVec.x() * speed) - dampening.x(), (moveVec.y() * speed) - dampening.y(), (moveVec.z() * speed) - dampening.z());
				else*/
				force = btVector3(moveVec.x() * speed, moveVec.y() * speed, moveVec.z() * speed);
	
				rigidBody->applyCentralForce(force);
			}
		}

		void RotateTurret(float amount)
		{
			turret->Rotate(amount);
		}
			
		unsigned int GetHealth()
		{
			return health;
		}

		void SetHealth(unsigned int health) { this->health = health; }

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

		octet::vec3 GetVelocity()
		{
			return octet::vec3(rigidBody->getLinearVelocity().x(), rigidBody->getLinearVelocity().y(), rigidBody->getLinearVelocity().z());
		}

		octet::vec3 GetDampening() { return dampening; }

		void AddBarrel()
		{
			turret->AddBarrel();
		}

		void ApplyDeceleration()
		{
			rigidBody->setLinearVelocity(rigidBody->getLinearVelocity() * deceleration);
		}
	};
	const char * Player::referenceName = "Player";
}

