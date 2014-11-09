#pragma once
#include "CollisionFlags.h"

namespace Arena
{
	///Base class for a game object with physics (a rigid body)
	///
	class PhysicsObject
	{
	private:
		///Updates node matrix based upon rigid body matrix
		void UpdateNodeMatrixFromPhysics()
		{
			btQuaternion btq = rigidBody->getOrientation();
			btVector3 pos = rigidBody->getCenterOfMassPosition();
			octet::quat q(btq[0], btq[1], btq[2], btq[3]);
			octet::mat4t modelToWorld = q;
			modelToWorld[3] = octet::vec4(pos[0], pos[1], pos[2], 1);
			node->access_nodeToParent() = modelToWorld;
		}
	protected:
		float mass = 10.0f; ///Rigid Body mass used on initialisation
		btRigidBody* rigidBody;
		octet::ref<octet::scene_node> node;
		octet::ref<octet::mesh_instance> mesh;
		octet::ref<octet::material> mat = nullptr;
		static const octet::vec3 defaultSize; ///Default mesh and collider shape to use when initialised with null
		
		float maxSpeed = -1.0f; ///Maximum linear velocity the rigid body will be allowed to achieve.
		float maxAngularVelocity = -1.0f; ///maximum angular velocity the rigid body will be allowed to achieve.
		IObjectPool* objectPool;
		Timer* timer;

		///Sets up the rigid body and corrosponding collision shape
		void InitialiseRigidBody(octet::mat4t modelToWorld, btCollisionShape* collisionShape = nullptr)
		{
			btMatrix3x3 btMatrix(get_btMatrix3x3(modelToWorld));
			btVector3 btPos(get_btVector3(modelToWorld[3].xyz()));

			if (collisionShape == nullptr)
				collisionShape = new btBoxShape(get_btVector3(defaultSize));

			btTransform transform(btMatrix, btPos);

			btDefaultMotionState *motionState = new btDefaultMotionState(transform);
			btVector3 inertiaTensor;

			collisionShape->calculateLocalInertia(mass, inertiaTensor);
			rigidBody = new btRigidBody(mass, motionState, collisionShape, inertiaTensor);
			rigidBody->setUserPointer(this);
		}

	public:

		enum ActiveState
		{
			Enabled,
			Disabled
		} activeState = ActiveState::Enabled;

		CollisionFlags::CollisionTypes collisionType = CollisionFlags::CollisionTypes::COL_NOTHING;
		int collisionMask = CollisionFlags::CollisionTypes::COL_WALL;

		static const char* referenceName; ///Unique identifier for this class, used to identify from derived classes

		///returns a unique string for this class
		virtual const char* GetReferenceType()
		{
			return referenceName;
		}

		PhysicsObject()
		{
		}

		virtual ~PhysicsObject()
		{
			delete rigidBody;
		}

		///Initialise PhysicsObject using supplied parameters. Any null parameters will be auto-generated.
		virtual void Initialise(octet::vec3 position, octet::mesh *shape = nullptr, btCollisionShape* collisionShape = nullptr, octet::material* mat = nullptr)
		{
			octet::mat4t modelToWorld = octet::mat4t();
			modelToWorld.translate(position.x(), position.y(), position.z());

			if (mat == nullptr)
			{
				mat = new octet::material(octet::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			}

			if (shape == nullptr)
				shape = new octet::mesh_box(defaultSize);

			InitialiseRigidBody(modelToWorld, collisionShape);

			node = new octet::scene_node(modelToWorld, octet::atom_);
			mesh = new octet::mesh_instance(node, shape, mat);
		}

		///Adds rigid to the physics world, mesh and top node to the game world  
		virtual void addPhysicsObjectToWorld(GameWorldContext& context)
		{
			context.physicsWorld.addRigidBody(GetRigidBody(), collisionType, collisionMask);
			context.app_scene.add_child(GetNode());

			context.app_scene.add_mesh_instance(GetMesh());
			context.objectPool.AddPhysicsObject(this);
			objectPool = &context.objectPool;
			timer = &context.timer;
		}

		///Updates the game physics objects tranforms and caps angular/linear velocities.
		virtual void Update(GameWorldContext& context)
		{
			//cap velocity
			if (maxSpeed >= 0)
			{
				btVector3 vel = rigidBody->getLinearVelocity();
				float speed = vel.length();
				if (speed > maxSpeed)
					rigidBody->setLinearVelocity(vel * (maxSpeed / speed));
			}

			if (maxAngularVelocity >= 0)
			{
				btVector3 vel = rigidBody->getAngularVelocity();
				float speed = vel.length();
				if (speed > maxAngularVelocity)
					rigidBody->setAngularVelocity(vel * (maxAngularVelocity / speed));
			}

			UpdateNodeMatrixFromPhysics();
		}

		///Sets the rigid body to be active
		virtual void Enable()
		{
			activeState = Enabled;
			rigidBody->setActivationState(ACTIVE_TAG);
		}

		///Moves the object off screeen and resets velocities to zero.
		virtual void Disable()
		{
			activeState = Disabled;
			//rigidBody->setActivationState(DISABLE_SIMULATION); //this messed up when enabling/disabling and setting forces on the same physics update.
			rigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
			rigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
			rigidBody->translate(btVector3(1000, 0, -1000));
		}

		///Translate via rigid body using vec3 for convience
		void Translate(octet::vec3 dir)
		{
			rigidBody->translate(get_btVector3(dir));
		}

		///Translate using rigid body
		void Translate(btVector3 dir)
		{
			rigidBody->translate(dir);
		}

		#pragma region Setter functions
		///Setter for world tranforms taking mat4t for convience
		void SetWorldTransform(octet::mat4t world4t)
		{
			btMatrix3x3 btMat = get_btMatrix3x3(world4t);
			btVector3 btPos = get_btVector3(world4t[3].xyz());
			btTransform transform = btTransform(btMat, btPos);
			rigidBody->setWorldTransform(transform);
		}

		///Setter for world tranforms
		void SetWorldTransform(btTransform transform)
		{
			rigidBody->setWorldTransform(transform);
		}
		#pragma endregion

		#pragma region Getter function
		///return Material
		virtual octet::material &GetMaterial()
		{
			return *mat;
		}

		///returns rigid body
		virtual btRigidBody *GetRigidBody()
		{
			return rigidBody;
		}

		///returns mesh instance
		virtual octet::mesh_instance *GetMesh()
		{
			return mesh;
		}

		///returns top node
		virtual octet::scene_node *GetNode()
		{
			return node;
		}

		///Gets world position from node matrix
		octet::vec3 GetPosition()
		{
			return node->access_nodeToParent()[3].xyz();
		}
		#pragma endregion
	};
	

	const char *PhysicsObject::referenceName = "PhysicsObject";
	const octet::vec3 PhysicsObject::defaultSize = octet::vec3(2.0f, 2.0f, 2.0f);
}
