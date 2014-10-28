#pragma once
#define OCTET_BULLET 1
#include "../../octet.h"
#include "PhysicsObject.h"

namespace Arena
{
	class Player : public PhysicsObject
	{
	private:
		unsigned int health;
		float fireForce = 1000.0f;
		float weaponOffset = 1.0f;
		
	public:
		Player()
		{
			Initialise(octet::vec3(0.0f, 10.0f, -10.0f), octet::vec3(2.0f, 2.0f, 2.0f));
		}
		~Player() {}

		octet::vec3 right = octet::vec3(1, 0, 0);
		octet::vec3 forward = octet::vec3(0, 0, -1); 
		octet::vec3 up = octet::vec3(0, 1, 0);

		float speed;

		static const char* referenceName;

		virtual const char* GetReferenceType() override
		{
			return Player::referenceName;
		}

		virtual void Initialise(octet::vec3 position, octet::vec3 size)
		{
			collisionType = CollisionFlags::CollisionTypes::COL_PLAYER;
			collisionMask = CollisionFlags::CollisionTypes::COL_WALL | CollisionFlags::CollisionTypes::COL_ENEMY | CollisionFlags::CollisionTypes::COL_POWERUP;
			
			mat = new octet::material(octet::vec4(0.0f, 0.75f, 0.0f, 1.0f));
			octet::mesh *shape = new octet::mesh_box(size);
			btBoxShape *collisionShape = new btBoxShape(get_btVector3(size));

			PhysicsObject::Initialise(position, shape, collisionShape, mat);

			rigidBody->setActivationState(DISABLE_DEACTIVATION);
			speed = 25.0f;
			health = 100;

			rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() |
				btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
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

		void FireProjectile(GameWorldContext& context)
		{
			Projectile *proj = context.objectPool.GetProjectileObject();
			
			proj->SetWorldTransform(node->access_nodeToParent());
			proj->Translate(btVector3(0,0,weaponOffset)); //weaponOffset

			btVector3 force = btVector3(forward.x() * fireForce, forward.y() * fireForce, forward.z() * fireForce);
			proj->GetRigidBody()->applyImpulse(force, btVector3(0.0f, 0.0f, 0.0f));
			
		}
	};
	const char * Player::referenceName = "Player";
}

