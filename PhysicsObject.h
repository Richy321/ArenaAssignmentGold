#pragma once
#include "../../octet.h"

namespace octet
{
	class PhysicsObject
	{
	private:
		void UpdateNodeMatrixFromPhysics()
		{
			btQuaternion btq = rigidBody->getOrientation();
			btVector3 pos = rigidBody->getCenterOfMassPosition();
			quat q(btq[0], btq[1], btq[2], btq[3]);
			mat4t modelToWorld = q;
			modelToWorld[3] = vec4(pos[0], pos[1], pos[2], 1);
			node->access_nodeToParent() = modelToWorld;
		}
	protected:
		bool isDynamic = true;
		btRigidBody* rigidBody;
		ref<scene_node> node;
		ref<mesh_box> box;
		ref<mesh_instance> mesh;
		ref<material> mat = nullptr;
	public:
		
		PhysicsObject()
		{
		}

		virtual ~PhysicsObject()
		{
			delete rigidBody;
		}
		virtual void Initialise(vec3 position, vec3 size)
		{
			random rnd = random();
			if (mat == nullptr)
				mat = new material(vec4(rnd.get(0.0f, 1.0f), rnd.get(0.0f, 1.0f), rnd.get(0.0f, 1.0f), 0.0f));

			mat4t modelToWorld = mat4t();
			modelToWorld.translate(position.x(), position.y(), position.z());
			
			InitialiseRigidBody(modelToWorld, size);

			box = new mesh_box(size);
			node = new scene_node(modelToWorld, atom_);
			mesh = new mesh_instance(node, box, mat);
		}
		
		void InitialiseRigidBody(mat4t modelToWorld, vec3 size)
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

		virtual material &GetMaterial()
		{
			return *mat;
		}

		virtual btRigidBody *GetRigidBody()
		{
			return rigidBody;
		}

		virtual mesh_instance *GetMesh()
		{
			return mesh;
		}

		virtual scene_node *GetNode()
		{
			return node;
		}
	};
}

