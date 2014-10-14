#pragma once

namespace octet
{
	class PhysicsObject
	{
	private:
	protected:
		bool isDynamic = true;
		btRigidBody* rigidBody;
		ref<scene_node> node;
		ref<mesh_box> box;
		ref<mesh_instance> mesh;
		ref<material> mat;
	public:
		vec3 size;
		
		PhysicsObject()
		{
			Initialise();
		}

		virtual void Initialise()
		{
			vec3 position = vec3(0.0f, 10.0f, -10.0f);
			size = vec3(2.0f, 2.0f, 2.0f);
			random rnd = random();
			//mat = new material(vec4(rnd.get(0, 1), rnd.get(0, 1), rnd.get(0, 1), 0.0f));

			image *arrowTexture = new image("src/examples/Arena/ArrowTexture.jpg");
			mat = new material(arrowTexture);

			mat4t modelToWorld = mat4t();
			modelToWorld.translate(position.x(), position.y(), position.z());
			
			InitialiseRigidBody(modelToWorld);

			box = new mesh_box(size);
			node = new scene_node(modelToWorld, atom_);
			mesh = new mesh_instance(node, box, mat);
		}
		void InitialiseRigidBody(mat4t modelToWorld)
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

		~PhysicsObject()
		{
			delete rigidBody;
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

