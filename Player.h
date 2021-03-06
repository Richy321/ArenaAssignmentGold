#pragma once
#define OCTET_BULLET 1
#include "../../octet.h"
#include "PhysicsObject.h"
#include "Turret.h"
#include <functional>

namespace Arena
{
	class ArenaApp;

	///Represents a tank player with rotatable turret
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
		enum State
		{
			Alive,
			Dead
		} curState = Dead;
	private:
		int health;
		int baseHealth;
		float speed;
		float baseSpeed;
		float deceleration = 0.9f;

		octet::vec3 size;
		unsigned int barrelCount = 1;

		float takeDamageStartTime = -1.0f;
		float takeDamageDuration = 0.2f;
		float respawnDelay = 5.0f;
		float diedTime = -respawnDelay;
		
		octet::vec4 originalColour;
		octet::vec4 damageColour;

		octet::hash_map<Direction, bool> activeAcceleration;
		bool isTakingDamage = false;
		bool isTurretRotating = false;
		octet::vec3 dampening;

		btGeneric6DofConstraint* constrXY;
		btHingeConstraint *constrTurretBase;

		int baseLives;
		int remainingLives;

		struct custom_vertex
		{
			octet::vec3p pos;
			uint32_t color;
		};

	public:
		Player(GameWorldContext& context, octet::vec4 colour)
		{
			originalColour = colour;
			octet::vec3 position = octet::vec3(0.0f, 10.0f, -10.0f);
			octet::vec3 size = octet::vec3(2.0f, 2.0f, 2.0f);

			collisionType = CollisionFlags::CollisionTypes::COL_PLAYER;
			collisionMask = CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_POWERUP | CollisionFlags::CollisionTypes::COL_PLAYER | CollisionFlags::CollisionTypes::COL_PROJECTILES;

			damageColour = octet::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		
			mat = new octet::material(originalColour);

			octet::mesh *shape = new octet::mesh_box(size);
			shape->set_mode(GL_LINES);

			this->size = size;
			btBoxShape *collisionShape = new btBoxShape(get_btVector3(size));

			PhysicsObject::Initialise(position, shape, collisionShape, mat);

			rigidBody->setActivationState(DISABLE_DEACTIVATION);

			rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() |
				btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

			Initialise();
		}
		~Player() {}

		unsigned int collisionDamage = 20;

		Turret *turret;

		std::function<void(Player&)> respawnCallback = nullptr;

		static const char* referenceName;

		virtual const char* GetReferenceType() override
		{
			return Player::referenceName;
		}
		
		virtual void Initialise()
		{
			baseSpeed = 1000.0f;
			speed = baseSpeed;

			baseHealth = 100;
			health = baseHealth;
			
			baseLives = 5;
			remainingLives = baseLives;

			maxSpeed = 90;

			activeAcceleration[Direction::North] = false;
			activeAcceleration[Direction::East] = false;
			activeAcceleration[Direction::South] = false;
			activeAcceleration[Direction::West] = false;
		}

		void addXZConstraint(GameWorldContext &context)
		{
			btTransform local;
			local.setIdentity();

			constrXY = new btGeneric6DofConstraint(*rigidBody, local, true);

			context.physicsWorld.addConstraint(constrXY);

			constrXY->setLinearLowerLimit(btVector3(-1000, -1000, -1000));
			constrXY->setLinearUpperLimit(btVector3(1000, 1000, 1000));

			constrXY->setAngularLowerLimit(btVector3(-SIMD_PI * 0.05f, 0, -SIMD_PI * 0.05f));
			constrXY->setAngularUpperLimit(btVector3(SIMD_PI * 0.05f, 0, SIMD_PI * 0.05f));
		}

		void addTurretConstraint(GameWorldContext &context)
		{
			btVector3 axis = btVector3(0.0f, 1.0f, 0.0f);
			btVector3 connectionPointPlayer = btVector3(0.0f, size.y(), 0.0f);
			btVector3 connectionPointBase = btVector3(0.0f, 0.0f, 0.0f);

			constrTurretBase = new btHingeConstraint(*rigidBody, *turret->GetRigidBody(), connectionPointPlayer, connectionPointBase, axis, axis);
			context.physicsWorld.addConstraint(constrTurretBase);
		}

		void addPhysicsObjectToWorld(GameWorldContext& context) override
		{
			PhysicsObject::addPhysicsObjectToWorld(context);
			
			turret = new Turret(context, this);
			turret->addPhysicsObjectToWorld(context);
			
			AttachConstraints(context);
		}

		void AttachConstraints(GameWorldContext& context)
		{
			addXZConstraint(context);

			btTransform turretTrans = rigidBody->getWorldTransform();
			btVector3 pos = rigidBody->getCenterOfMassPosition();
			turret->GetRigidBody()->setWorldTransform(turretTrans);
			addTurretConstraint(context);
		}

		octet::mesh* BuildBaseMesh(float size)
		{
			octet::mesh* baseMesh = new octet::mesh();

			baseMesh->allocate(sizeof(octet::mesh::vertex) * 8, sizeof(uint32_t) * 32);

			baseMesh->add_attribute(octet::attribute_pos, 3, GL_FLOAT, 0);
			baseMesh->add_attribute(octet::attribute_normal, 3, GL_FLOAT, 12);
			baseMesh->add_attribute(octet::attribute_uv, 2, GL_FLOAT, 24);
			baseMesh->set_params(sizeof(octet::mesh::vertex), 32, 8, GL_LINES, GL_UNSIGNED_INT);
			
			octet::gl_resource::wolock vl(baseMesh->get_vertices());
			octet::mesh::vertex *vtx = (octet::mesh::vertex *)vl.u8();
			octet::gl_resource::wolock il(baseMesh->get_indices());
			uint32_t *idx = il.u32();

			float frontOffsetX = 0.5f;
			float frontOffsetZ = 0.5f;

			float backOffsetX = 0.75;
			float backOffsetZ = 0.5f;

			//back
			vtx->pos = octet::vec3p(-size, -size, size); vtx->normal = octet::vec3p(0.0f, 0.0f, 1.0f);  vtx->uv = octet::vec2p(0.0f, 0.0f); vtx++; //bottom left
			vtx->pos = octet::vec3p(-size, size, size); vtx->normal = octet::vec3p(0.0f, 0.0f, 1.0f);  vtx->uv = octet::vec2p(0.0f, 1.0f); vtx++; //top left
			vtx->pos = octet::vec3p(size, size, size); vtx->normal = octet::vec3p(0.0f, 0.0f, 1.0f);  vtx->uv = octet::vec2p(1.0f, 1.0f); vtx++; //top right
			vtx->pos = octet::vec3p(size, -size, size); vtx->normal = octet::vec3p(0.0f, 0.0f, 1.0f);  vtx->uv = octet::vec2p(1.0f, 0.0f); vtx++; //bottom right

			//front
			vtx->pos = octet::vec3p(-size, -size, -size); vtx->normal = octet::vec3p(0.0f, 0.0f, 1.0f);  vtx->uv = octet::vec2p(0.0f, 0.0f); vtx++;
			vtx->pos = octet::vec3p(-size, size, -size); vtx->normal = octet::vec3p(0.0f, 0.0f, 1.0f);  vtx->uv = octet::vec2p(0.0f, 1.0f); vtx++;
			vtx->pos = octet::vec3p(size, size, -size); vtx->normal = octet::vec3p(0.0f, 0.0f, 1.0f);  vtx->uv = octet::vec2p(1.0f, 1.0f); vtx++;
			vtx->pos = octet::vec3p(size, -size, -size); vtx->normal = octet::vec3p(0.0f, 0.0f, 1.0f);  vtx->uv = octet::vec2p(1.0f, 0.0f); vtx++;

			for (int i = 0; i < 6; i++)
			{
				idx[0] = i + 0;
				idx[1] = i + 3;
				idx[2] = i + 1;
				idx += 3;

				idx[0] = i + 0;
				idx[1] = i + 2;
				idx[2] = i + 3;
				idx += 3;
			}

			return baseMesh;
		}

		void Update(GameWorldContext& context) override
		{
			PhysicsObject::Update(context);

			if (curState == Alive)
			{
				if (health <= 0)
					Die(context);
			}
			else if (curState == Dead)
			{
				float time = context.timer.GetRunningTime();
		
				if (time > diedTime + respawnDelay)
					Respawn(context);
			}

			if (isTakingDamage && timer->GetRunningTime() > takeDamageStartTime + takeDamageDuration)
			{
				isTakingDamage = false;
				mat->set_diffuse(originalColour);
			}

			UpdateAcceleration();

			if (!isTurretRotating)
				SetTurretDampening(1.5f);

			activeAcceleration[Direction::North] = false;
			activeAcceleration[Direction::East] = false;
			activeAcceleration[Direction::South] = false;
			activeAcceleration[Direction::West] = false;
			isTurretRotating = false;
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
				force = btVector3(moveVec.x() * speed, moveVec.y() * speed, moveVec.z() * speed);
	
				rigidBody->applyCentralForce(force);
			}
		}

		void RotateTurret(float amount)
		{
			turret->Rotate(amount);
			isTurretRotating = true;
		}

		unsigned int GetHealth() { return health; }

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

		void SetTurretDampening(float value) { turret->SetDampening(value);}

		void SetMovementDampening(float value) { rigidBody->setDamping(value, 0.0f); }

		int GetRemainingLives() { return remainingLives; }

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

		void Die(GameWorldContext& context)
		{
			diedTime = context.timer.GetRunningTime();
			float initValue = 0.0f;
			curState = Dead;
			context.physicsWorld.removeConstraint(constrXY);
			context.physicsWorld.removeConstraint(constrTurretBase);

			rigidBody->applyCentralImpulse(btVector3(0.0f, 0.5f, 0.0f));
			turret->SetDampening(0.0f);
			turret->GetRigidBody()->applyCentralImpulse(btVector3(0.0f, 0.5f, 0.0f));
			turret->GetRigidBody()->applyTorqueImpulse(btVector3(1.0f, 11.0f, 1.0f));

			rigidBody->translate(btVector3(1000.0f, 1000.0f, 1000.0f));

			remainingLives--;
		}

		void Respawn(GameWorldContext& context)
		{
			if (respawnCallback != nullptr)
				respawnCallback(*this);

			curState = Alive;	

			rigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
			rigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
			AttachConstraints(context);
			
			health = baseHealth;
		}
	};
	const char * Player::referenceName = "Player";
}

