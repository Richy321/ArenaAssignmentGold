#pragma once
namespace octet {
	class Player
	{
	public:
		Player()
		{
			Initialise();
		}
		~Player() {}

		material* mat;
		bool isDynamic = true;
		btRigidBody* rigidBody;
		scene_node *node;
		mesh_box *box;
		vec3 size;
		mesh_instance* mesh;


		vec3 up = vec3(0, 1, 0);
		vec3 right = vec3(0, 1, 0);
		vec3 forward = vec3(0, 0, 1);

		float speed;
		void Initialise()
		{
			vec3 position = vec3(0.0f, 10.0f, -10.0f);
			mat4t modelToWorld = mat4t();
			
			modelToWorld.translate(position.x(), position.y(), position.z());
			
			size = vec3(2, 2, 2);
			speed = 25.0;
			image *arrowTexture = new image("src/examples/Arena/ArrowTexture.jpg");
			//image *arrowTexture = new image("assets/reije081.home.xs4all.nl/top.jpg");
			mat = new material(arrowTexture);

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

		void Update()
		{
		}

		void Move(vec3 moveVec)
		{
			moveVec.normalize();
			btVector3 force = btVector3(moveVec.x() * speed, moveVec.y() * speed, moveVec.z() * speed);

			rigidBody->applyForce(force, btVector3(0, 0, 0));
		}

		vec3 GetPosition()
		{
			return node->access_nodeToParent()[3].xyz();
		}
	private:

	};
}