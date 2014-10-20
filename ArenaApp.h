////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
#define OCTET_BULLET 1
#include "../../octet.h"
#include "PhysicsObject.h"
#include "Player.h"
#include "Enemy.h"
#include "Floor.h"

#include "HUDText.h"
namespace Arena
{
	/// Scene using bullet for physics effects.
	class ArenaApp : public octet::app 
	{
	private:
		octet::ref<octet::visual_scene> app_scene;
		btDefaultCollisionConfiguration config;       /// setup for the world
		btCollisionDispatcher *dispatcher;            /// handler for collisions between objects
		btDbvtBroadphase *broadphase;                 /// handler for broadphase (rough) collision
		btSequentialImpulseConstraintSolver *solver;  /// handler to resolve collisions
		btDiscreteDynamicsWorld *world;             /// physics world, contains rigid bodies

		octet::dynarray<btRigidBody*> rigid_bodies;
		octet::dynarray<octet::scene_node*> nodes;
		octet::dynarray<PhysicsObject*> physicsObjects;

		octet::camera_instance *camera;
		Player *player;
		Floor* floor;
		HUDText *debugText;

		btOverlapFilterCallback *customFilterCallback;

		//Mouse variables
		int prevMouseX = -1;
		int prevMouseY = -1;
		int curMouseX = -1;
		int curMouseY = -1;
		float sensitivity = 0.3f;

		bool debugMode = true;

		void handleCameraMovement()
		{
			if (prevMouseX != -1 && prevMouseY != -1)
			{
				int dx = (curMouseX - prevMouseX);
				int dy = (curMouseY - prevMouseY);

				octet::vec3 up = octet::vec3(0, 1, 0);
				octet::vec3 right = octet::vec3(1, 0, 0);

				octet::vec4 mouseMove = octet::vec4(dy, dx, 0);
				octet::vec4 mouseMoveDir = mouseMove.normalize();
				float mouseMoveLength = mouseMove.squared();
				if (mouseMoveLength > 0)
				{
					octet::mat4t &camMat = app_scene->get_camera_instance(0)->get_node()->access_nodeToParent();
					camMat.loadIdentity();
					camMat.rotateY(-curMouseX * sensitivity);
					camMat.rotateX(-curMouseY * sensitivity);
				}
			}
			prevMouseX = curMouseX;
			prevMouseY = curMouseY;
		}

		void handleInput()
		{
			if (is_key_down(octet::key_left) || is_key_down('A') || is_key_down('a'))
				player->Move(octet::vec3(-player->speed, 0, 0));
			if (is_key_down(octet::key_right) || is_key_down('D') || is_key_down('d'))
				player->Move(octet::vec3(player->speed, 0, 0));
			if (is_key_down(octet::key_up) || is_key_down('W') || is_key_down('w'))
				player->Move(octet::vec3(0, 0, -player->speed));
			if (is_key_down(octet::key_down) || is_key_down('S') || is_key_down('s'))
				player->Move(octet::vec3(0, 0, player->speed));

			if (is_key_down(octet::key_esc))
			{
				Cleanup();
				exit(1); //quick exit, cleanup?
			}
		}

		void Cleanup()
		{
			delete world;
			delete solver;
			delete broadphase;
			delete dispatcher;
		}

		void add_box(octet::mat4t_in modelToWorld, octet::vec3_in size, octet::material *mat, bool is_dynamic = true) {

			btMatrix3x3 matrix(get_btMatrix3x3(modelToWorld));
			btVector3 pos(get_btVector3(modelToWorld[3].xyz()));

			btCollisionShape *shape = new btBoxShape(get_btVector3(size));

			btTransform transform(matrix, pos);

			btDefaultMotionState *motionState = new btDefaultMotionState(transform);
			btScalar mass = is_dynamic ? 1.0f : 0.0f;
			btVector3 inertiaTensor;

			shape->calculateLocalInertia(mass, inertiaTensor);

			btRigidBody * rigid_body = new btRigidBody(mass, motionState, shape, inertiaTensor);
			world->addRigidBody(rigid_body);
			rigid_bodies.push_back(rigid_body);

			octet::mesh_box *box = new octet::mesh_box(size);
			octet::scene_node *node = new octet::scene_node(modelToWorld, octet::atom_);
			nodes.push_back(node);

			app_scene->add_child(node);
			app_scene->add_mesh_instance(new octet::mesh_instance(node, box, mat));
		}

		void update()
		{
			handleInput();
			cameraFollow((*player));
			octet::vec3 playerPos = player->GetPosition();
			static char tmp[64];

			debugText->text = playerPos.toString(tmp, sizeof(tmp));
		}

	public:
		/// this is called when we construct the class before everything is initialised.
		ArenaApp(int argc, char **argv) : octet::app(argc, argv) {
			dispatcher = new btCollisionDispatcher(&config);
			broadphase = new btDbvtBroadphase();
			solver = new btSequentialImpulseConstraintSolver();
			world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, &config);
			customFilterCallback = new Arena::customFilterCallback();
			world->getPairCache()->setOverlapFilterCallback(customFilterCallback); //register custom broadphase callback filter
		}

		~ArenaApp()
		{
			Cleanup();
		}

		/// this is called once OpenGL is initialized
		void app_init()
		{
			app_scene = new octet::visual_scene();
			app_scene->create_default_camera_and_lights();
			camera = app_scene->get_camera_instance(0);
			
			floor = new Floor();
			addPhysicsObjectToWorld(floor);

			player = new Player();
			addPhysicsObjectToWorld(player);

			debugText = new HUDText(new octet::aabb(octet::vec3(-350.0f, -300, 0.0f), octet::vec3(10.0f, 20.0f, 30.0f)));
			debugText->text = "Hello";
			
			//add the boxes (as dynamic objects)
			octet::mat4t modelToWorld;
			modelToWorld.translate(-4.5f, 10.0f, 0);
			octet::material *mat = new octet::material(octet::vec4(0, 1.0f, 0.5f, 1.0f));
			for (int i = 0; i != 20; ++i) 
			{
				modelToWorld.translate(3, 0, 0);
				modelToWorld.rotateZ(360 / 20);
				Enemy *enemy = new Enemy(modelToWorld[3].xyz());
				addPhysicsObjectToWorld(enemy);
			}
		}

		void addPhysicsObjectToWorld(PhysicsObject* physObj)
		{
			world->addRigidBody(physObj->GetRigidBody(), physObj->collisionType, physObj->collisionMask);
			app_scene->add_child(physObj->GetNode());
			app_scene->add_mesh_instance(physObj->GetMesh());
			physicsObjects.push_back(physObj);
		}

		void drawDebug()
		{
			if (debugMode)
			{

			}
		}

		void cameraFollow(Player& target)
		{
			camera->get_node()->loadIdentity();

			octet::vec3 targetPos = target.GetPosition();
			camera->get_node()->rotate(-90, octet::vec3(1, 0, 0));
			camera->get_node()->translate(octet::vec3(targetPos.x(), -targetPos.z(), 30));
		}
		/// this is called to draw tnhe world
		void draw_world(int x, int y, int w, int h) {

			get_mouse_pos(curMouseX, curMouseY);

			update();

			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			debugText->draw(vx, vy);

			world->stepSimulation(1.0f / 30);
			
			for (unsigned int i = 0; i < physicsObjects.size(); ++i)
			{
				physicsObjects[i]->Update();
			}

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);
			// draw the scene
			app_scene->render((float)vx / vy);
		}

		octet::vec3 calcMouseToWorld()
		{



		}
	};
}
