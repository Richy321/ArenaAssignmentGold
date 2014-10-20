#pragma once
#define OCTET_BULLET 1
#include "../../octet.h"

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
		octet::ref<octet::mesh_box> box;
		octet::ref<octet::mesh_instance> mesh;
		octet::ref<octet::material> mat = nullptr;
	public:
		
		PhysicsObject()
		{
		}

		virtual ~PhysicsObject()
		{
			delete rigidBody;
		}
		virtual void Initialise(octet::vec3 position, octet::vec3 size)
		{
			octet::random rnd = octet::random();
			if (mat == nullptr)
				mat = new octet::material(octet::vec4(rnd.get(0.0f, 1.0f), rnd.get(0.0f, 1.0f), rnd.get(0.0f, 1.0f), 0.0f));

			octet::mat4t modelToWorld = octet::mat4t();
			modelToWorld.translate(position.x(), position.y(), position.z());
			
			InitialiseRigidBody(modelToWorld, size);

			box = new octet::mesh_box(size);
			node = new octet::scene_node(modelToWorld, octet::atom_);
			mesh = new octet::mesh_instance(node, box, mat);
		}
		
		void InitialiseRigidBody(octet::mat4t modelToWorld, octet::vec3 size)
		{
			btMatrix3x3 btMatrix(get_btMatrix3x3(modelToWorld));
			btVector3 btPos(get_btVector3(modelToWorld[3].xyz()));

			btCollisionShape *shape = new btBoxShape(get_btVector3(size));
			btTransform transform(btMatrix, btPos);

			btDefaultMotionState *motionState = new btDefaultMotionState(transform);
			btScalar mass = isDynamic ? 10.0f : 0.0f;
			btVector3 inertiaTensor;

			shape->calculateLocalInertia(mass, inertiaTensor);
			rigidBody = new btRigidBody(mass, motionState, shape, inertiaTensor);
		}

	
		virtual void Update()
		{
			UpdateNodeMatrixFromPhysics();
		}

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
	};
}

