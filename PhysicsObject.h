#pragma once
#include "CollisionFlags.h"

namespace Arena
{
	class PhysicsObject
	{
	private:
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
		bool isDynamic = true;
		btRigidBody* rigidBody;
		octet::ref<octet::scene_node> node;
		octet::ref<octet::mesh_instance> mesh;
		octet::ref<octet::material> mat = nullptr;
		static const octet::vec3 defaultSize;

		IObjectPool* objectPool;

	public:
		CollisionFlags::CollisionTypes collisionType = CollisionFlags::CollisionTypes::COL_NOTHING;
		int collisionMask = CollisionFlags::CollisionTypes::COL_WALL;

		static const char* referenceName;

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

		virtual void Initialise(octet::vec3 position, octet::mesh *shape = nullptr, btCollisionShape* collisionShape = nullptr, octet::material* mat = nullptr)
		{
			octet::mat4t modelToWorld = octet::mat4t();
			modelToWorld.translate(position.x(), position.y(), position.z());

			if (mat == nullptr)
			{
				octet::random rnd = octet::random();
				mat = new octet::material(octet::vec4(rnd.get(0.0f, 1.0f), rnd.get(0.0f, 1.0f), rnd.get(0.0f, 1.0f), 0.0f));
			}

			if (shape == nullptr)
				shape = new octet::mesh_box(defaultSize);

			InitialiseRigidBody(modelToWorld, collisionShape);

			node = new octet::scene_node(modelToWorld, octet::atom_);
			mesh = new octet::mesh_instance(node, shape, mat);
		}

		void InitialiseRigidBody(octet::mat4t modelToWorld, btCollisionShape* collisionShape = nullptr)
		{
			btMatrix3x3 btMatrix(get_btMatrix3x3(modelToWorld));
			btVector3 btPos(get_btVector3(modelToWorld[3].xyz()));

			if (collisionShape == nullptr)
				collisionShape = new btBoxShape(get_btVector3(defaultSize));

			btTransform transform(btMatrix, btPos);

			btDefaultMotionState *motionState = new btDefaultMotionState(transform);
			btScalar mass = isDynamic ? 10.0f : 0.0f;
			btVector3 inertiaTensor;

			collisionShape->calculateLocalInertia(mass, inertiaTensor);
			rigidBody = new btRigidBody(mass, motionState, collisionShape, inertiaTensor);
			rigidBody->setUserPointer(this);
		}

		void addPhysicsObjectToWorld(GameWorldContext& context)
		{
			context.physicsWorld.addRigidBody(GetRigidBody(), collisionType, collisionMask);
			context.app_scene.add_child(GetNode());

			context.app_scene.add_mesh_instance(GetMesh());
			context.objectPool.AddPhysicsObject(this);
			objectPool = &context.objectPool;
		}

		virtual void Update()
		{
			UpdateNodeMatrixFromPhysics();
		}

		void Enable()
		{
			rigidBody->setActivationState(ACTIVE_TAG);
		}

		void Disable()
		{
			rigidBody->setActivationState(DISABLE_SIMULATION);
			rigidBody->translate(btVector3(1000, 1000, -1000));
		}

		void SetWorldTransform(octet::mat4t world4t)
		{
			btMatrix3x3 btMat = get_btMatrix3x3(world4t);
			btVector3 btPos = get_btVector3(world4t[3].xyz());
			btTransform transform = btTransform(btMat, btPos);
			rigidBody->setWorldTransform(transform);
		}

		void Translate(octet::vec3 dir)
		{
			rigidBody->translate(get_btVector3(dir));
		}
		void Translate(btVector3 dir)
		{
			rigidBody->translate(dir);
		}

		#pragma region getters
		virtual octet::material &GetMaterial()
		{
			return *mat;
		}

		virtual btRigidBody *GetRigidBody()
		{
			return rigidBody;
		}

		virtual octet::mesh_instance *GetMesh()
		{
			return mesh;
		}

		virtual octet::scene_node *GetNode()
		{
			return node;
		}
		#pragma endregion
	};
	

	const char *PhysicsObject::referenceName = "PhysicsObject";
	const octet::vec3 PhysicsObject::defaultSize = octet::vec3(2.0f, 2.0f, 2.0f);
}
