#pragma once
#include "PhysicsObject.h"

namespace octet
{
	class Enemy
	{
	public:
		ref<material> mat;
		
		btRigidBody* rigidBody;
		scene_node *node;
		mesh_box *box;
		vec3 size;
		mesh_instance* mesh;

		bool isDynamic = true;
		vec3 up = vec3(0, 1, 0);
		vec3 right = vec3(0, 1, 0);
		vec3 forward = vec3(0, 0, 1);

		btScalar mass;

		float speed;

		void updateNodeFromPhysics()
		{

		}
		
		material GetMaterial()
		{
			return *mat;
		}

		btRigidBody &GetRigidBody()
		{
			return *rigidBody;
		}
		virtual mesh_box &GetMesh() = 0;
		virtual scene_node &GetNode() = 0;

		Enemy()
		{

		}

		~Enemy()
		{
		}

		void Initialise(vec3 position)
		{
			mat4t modelToWorld = mat4t();
			modelToWorld.translate(position.x(), position.y(), position.z());

			size = vec3(2, 2, 2);
			speed = 25.0f;
			mat = new material(vec4(0.0f, 0.2f, 0.75f, 1.0f));

			btMatrix3x3 matrix(get_btMatrix3x3(modelToWorld));
			btVector3 pos(get_btVector3(modelToWorld[3].xyz()));

			btCollisionShape *shape = new btBoxShape(get_btVector3(size));

			btTransform transform(matrix, pos);

			btDefaultMotionState *motionState = new btDefaultMotionState(transform);
			btScalar mass = isDynamic ? 10.0f : 0.0f;
			btVector3 inertiaTensor;

			shape->calculateLocalInertia(mass, inertiaTensor);
			rigidBody = new btRigidBody(mass, motionState, shape, inertiaTensor);

			box = new mesh_box(size);
			node = new scene_node(modelToWorld, atom_);
			mesh = new mesh_instance(node, box, mat);
		}
	};
}
