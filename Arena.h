////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
#include "RoamCamera.h"
#include "Player.h"
#include "Enemy.h"
#include "IPhysicsCharacter.h"

namespace octet {
	/// Scene using bullet for physics effects.
	class Arena : public app {
		// scene for drawing box
		ref<visual_scene> app_scene;

		btDefaultCollisionConfiguration config;       /// setup for the world
		btCollisionDispatcher *dispatcher;            /// handler for collisions between objects
		btDbvtBroadphase *broadphase;                 /// handler for broadphase (rough) collision
		btSequentialImpulseConstraintSolver *solver;  /// handler to resolve collisions
		btDiscreteDynamicsWorld *world;             /// physics world, contains rigid bodies

		dynarray<btRigidBody*> rigid_bodies;
		dynarray<scene_node*> nodes;

		camera_instance *camera;
		Player* player;

		//Mouse variables
		float prevMouseX = -1;
		float prevMouseY = -1;
		float sensitivity = 0.3f;

		bool debugMode = true;

		// a texture for our text
		GLuint font_texture;

		// information for our text
		bitmap_font font;

		void handleCameraMovement()
		{
			int x = 0, y = 0;
			get_mouse_pos(x, y);

			if (prevMouseX != -1 && prevMouseY != -1)
			{
				float dx = (x - prevMouseX);
				float dy = (y - prevMouseY);

				vec3 up = vec3(0, 1, 0);
				vec3 right = vec3(1, 0, 0);

				vec4 mouseMove = vec4(dy, dx, 0);
				vec4 mouseMoveDir = mouseMove.normalize();
				float mouseMoveLength = mouseMove.squared();
				if (mouseMoveLength > 0)
				{
					mat4t &camMat = app_scene->get_camera_instance(0)->get_node()->access_nodeToParent();
					camMat.loadIdentity();
					camMat.rotateY(-x * sensitivity);
					camMat.rotateX(-y * sensitivity);
				}
			}
			prevMouseX = x;
			prevMouseY = y;
		}

		void handleInput()
		{
			if (is_key_down(key_left) || is_key_down('A') || is_key_down('a'))
				player->Move(vec3(-player->speed, 0, 0));
			if (is_key_down(key_right) || is_key_down('D') || is_key_down('d'))
				player->Move(vec3(player->speed, 0, 0));
			if (is_key_down(key_up) || is_key_down('W') || is_key_down('w'))
				player->Move(vec3(0, 0, -player->speed));
			if (is_key_down(key_down) || is_key_down('S') || is_key_down('s'))
				player->Move(vec3(0, 0, player->speed));

			if (is_key_down(key_esc))
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
		void create_default_roam_camera_and_lights()
		{
			/// default camera_instance
			if (app_scene->get_num_camera_instances() == 0)
			{
				aabb bb = app_scene->get_world_aabb();
				bb = bb.get_union(aabb(vec3(0, 0, 0), vec3(5, 5, 5)));

				//scene_node *node = app_scene->add_scene_node();

				float bb_size = length(bb.get_half_extent()) * 2.0f;
				float distance = max(bb.get_max().z(), bb_size) * 2;

				//camera = new RoamCamera();
				//camera->InitialiseCamera(vec3(0, 10, distance), app_scene);

				//node->access_nodeToParent().translate(0, 0, distance);
				//float f = distance * 2, n = f * 0.001f;
				//cam->set_node(node);
				//cam->set_perspective(0, 45, 1, n, f);
				//app_scene->add_child(camera->nodeX);
				//app_scene->add_camera_instance(camera->camera);
			}

			/// default light instance
			if (app_scene->get_num_light_instances() == 0) {
				scene_node *node = app_scene->add_scene_node();
				light *_light = new light();
				light_instance *li = new light_instance();
				node->access_nodeToParent().translate(100, 100, 100);
				node->access_nodeToParent().rotateX(45);
				node->access_nodeToParent().rotateY(45);
				_light->set_color(vec4(1, 1, 1, 1));
				_light->set_kind(atom_directional);
				li->set_node(node);
				li->set_light(_light);
				app_scene->add_light_instance(li);
			}

			//if (app_scene->)

		}

		void add_box(mat4t_in modelToWorld, vec3_in size, material *mat, bool is_dynamic = true) {

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

			mesh_box *box = new mesh_box(size);
			scene_node *node = new scene_node(modelToWorld, atom_);
			nodes.push_back(node);

			app_scene->add_child(node);
			app_scene->add_mesh_instance(new mesh_instance(node, box, mat));
		}

		void draw_text(texture_shader &shader, float x, float y, float scale, const char *text)
		{
			mat4t modelToWorld;
			modelToWorld.loadIdentity();
			modelToWorld.translate(x, y, 0);
			modelToWorld.scale(scale, scale, 1);
			mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, camera->get_node()->access_nodeToParent());

			/*mat4t tmp;
			glLoadIdentity();
			glTranslatef(x, y, 0);
			glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&tmp);
			glScalef(scale, scale, 1);
			glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&tmp);*/

			enum { max_quads = 32 };
			bitmap_font::vertex vertices[max_quads * 4];
			uint32_t indices[max_quads * 6];
			aabb bb(vec3(0, 0, 0), vec3(256, 256, 0));

			unsigned num_quads = font.build_mesh(bb, vertices, indices, max_quads, text, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, font_texture);

			shader.render(modelToProjection, 0);

			glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].x);
			glEnableVertexAttribArray(attribute_pos);
			glVertexAttribPointer(attribute_uv, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].u);
			glEnableVertexAttribArray(attribute_uv);

			glDrawElements(GL_TRIANGLES, num_quads * 6, GL_UNSIGNED_INT, indices);
		}

		void update()
		{
			player->Update();
			handleInput();
			cameraFollow((*player));
			//handleCameraMovement
		}

	public:
		/// this is called when we construct the class before everything is initialised.
		Arena(int argc, char **argv) : app(argc, argv) {
			dispatcher = new btCollisionDispatcher(&config);
			broadphase = new btDbvtBroadphase();
			solver = new btSequentialImpulseConstraintSolver();
			world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, &config);
		}

		~Arena() 
		{
			Cleanup();
		}

		/// this is called once OpenGL is initialized
		void app_init()
		{
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			camera = app_scene->get_camera_instance(0);
			mat4t modelToWorld;
			material *floor_mat = new material(vec4(0, 1, 1, 1));

			// add the ground (as a static object)
			add_box(modelToWorld, vec3(200.0f, 0.5f, 200.0f), floor_mat, false);
			rigid_bodies[rigid_bodies.size() - 1]->setFriction(0.70f);

			player = new Player();
			world->addRigidBody(player->rigidBody);
			rigid_bodies.push_back(player->rigidBody);
			nodes.push_back(player->node);
			app_scene->add_child(player->node);
			app_scene->add_mesh_instance(player->mesh);

			//add the boxes (as dynamic objects)
			modelToWorld.translate(-4.5f, 10.0f, 0);
			material *mat = new material(vec4(0, 1.0f, 0.5f, 1.0f));
			for (int i = 0; i != 20; ++i) {
				modelToWorld.translate(3, 0, 0);
				modelToWorld.rotateZ(360 / 20);
				add_box(modelToWorld, vec3(0.5f), mat);
			}
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

			vec3 targetPos = target.GetPosition();
			printf("%f %f %f\n", targetPos.x(), targetPos.y(), targetPos.z());
			camera->get_node()->rotate(-90, vec3(1, 0, 0));
			camera->get_node()->translate(vec3(targetPos.x(), -targetPos.z(), 30));
		}
		/// this is called to draw the world
		void draw_world(int x, int y, int w, int h) {

			update();

			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			world->stepSimulation(1.0f / 30);
			for (unsigned i = 0; i != rigid_bodies.size(); ++i) {
				btRigidBody *rigid_body = rigid_bodies[i];
				btQuaternion btq = rigid_body->getOrientation();
				btVector3 pos = rigid_body->getCenterOfMassPosition();
				quat q(btq[0], btq[1], btq[2], btq[3]);
				mat4t modelToWorld = q;
				modelToWorld[3] = vec4(pos[0], pos[1], pos[2], 1);
				nodes[i]->access_nodeToParent() = modelToWorld;
			}

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);

			// draw the scene
			app_scene->render((float)vx / vy);
		}
	};
}
