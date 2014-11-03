////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
#define OCTET_BULLET 1
#include "../../octet.h"
#include "IObjectPool.h"
#include "GameWorldContext.h"

#include "ObjectPool.h"
#include "PhysicsObject.h"
#include "Player.h"
#include "Enemy.h"
#include "Floor.h"
#include "Projectile.h"
#include "Timer.h"
#include "WaveManager.h"
#include "PowerUp.h"
#include "HUD.h"


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
		btDiscreteDynamicsWorld *world;               /// physics world, contains rigid bodies

		octet::camera_instance *camera;

		Player *player;
		Player *player2;
		Floor *floor;
		ObjectPool *objectPool;
		Timer *timer;
		WaveManager *waveManager;
		
		Hud *HUD;

		btOverlapFilterCallback *filterCallback;

		GameWorldContext *worldContext;

		//Mouse variables
		int prevMouseX = -1;
		int prevMouseY = -1;
		int curMouseX = -1;
		int curMouseY = -1;
		float sensitivity = 0.3f;

		void handleCameraMovement()
		{
			if (prevMouseX != -1 && prevMouseY != -1)
			{
				int dx = (curMouseX - prevMouseX);
				int dy = (curMouseY - prevMouseY);

				octet::vec3 up = octet::vec3(0, 1, 0);
				octet::vec3 right = octet::vec3(1, 0, 0);

				octet::vec4 mouseMove = octet::vec4((float)dy, (float)dx, 0);
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
			if ((is_key_down(octet::key_left) || is_key_down('A') || is_key_down('a')) ||
				(is_key_down(octet::key_right) || is_key_down('D') || is_key_down('d')) ||
				(is_key_down(octet::key_up) || is_key_down('W') || is_key_down('w')) ||
				(is_key_down(octet::key_down) || is_key_down('S') || is_key_down('s')) )
			{
				if (is_key_down(octet::key_left) || is_key_down('A') || is_key_down('a'))
					player->Move(octet::vec3(-1.0f, 0.0f, 0.0f));
				if (is_key_down(octet::key_right) || is_key_down('D') || is_key_down('d'))
					player->Move(octet::vec3(1.0f, 0.0f, 0.0f));
				if (is_key_down(octet::key_up) || is_key_down('W') || is_key_down('w'))
					player->Move(octet::vec3(0.0f, 0.0f, -1.0f));
				if (is_key_down(octet::key_down) || is_key_down('S') || is_key_down('s'))
					player->Move(octet::vec3(0.0f, 0.0f, 1.0f));
			}
			else
			{
				player->ApplyDeceleration();
			}

			if (is_key_down(octet::key_down) || is_key_down('Q') || is_key_down('q') ||
				(is_key_down(octet::key_down) || is_key_down('E') || is_key_down('e')))
			{
				if (is_key_down(octet::key_down) || is_key_down('Q') || is_key_down('q'))
					player->RotateTurret(1.0f);

				if (is_key_down(octet::key_down) || is_key_down('E') || is_key_down('e'))
					player->RotateTurret(-1.0f);
			}
			else
			{
				player->SetTurretDampening(2.5f);
			}

			if (is_key_down(octet::key_space))
				player->FireTurrets(*worldContext);

			if (is_key_down(octet::key_f1))
			{
				for (unsigned int i = 0; i < objectPool->GetActiveEnemies().size(); i++)
				{
					objectPool->GetActiveEnemies()[i]->SetTarget(nullptr);
					objectPool->GetActiveEnemies()[i]->SetAIMode(Enemy::AIMode::Idle);
				}
			}

			if (is_key_down(octet::key_f2))
			{
				for (unsigned int i = 0; i < objectPool->GetActiveEnemies().size(); i++)
				{
					objectPool->GetActiveEnemies()[i]->SetTarget(player);
					objectPool->GetActiveEnemies()[i]->SetAIMode(Enemy::AIMode::DumbChase);
				}
			}

			if (is_key_down(octet::key_f3))
			{
				for (unsigned int i = 0; i < objectPool->GetActiveEnemies().size(); i++)
				{
					objectPool->GetActiveEnemies()[i]->SetTarget(player);
					objectPool->GetActiveEnemies()[i]->SetAIMode(Enemy::AIMode::Chase);
				}
			}

			if (is_key_down(octet::key_esc))
			{
				cleanup();
				exit(1); //quick exit, cleanup?
			}
		}

		void cleanup()
		{
			delete world;
			delete solver;
			delete broadphase;
			delete dispatcher;
		}

		void update()
		{
			handleInput();
			cameraFollow((*player));
			HUD->update(*player, *objectPool);
			timer->Update();
		}

	public:
		/// this is called when we construct the class before everything is initialised.
		ArenaApp(int argc, char **argv) : octet::app(argc, argv) 
		{
			dispatcher = new btCollisionDispatcher(&config);
			broadphase = new btDbvtBroadphase();
			solver = new btSequentialImpulseConstraintSolver();
			world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, &config);
			filterCallback = new ArenaApp::customFilterCallback();
			
			//world->getPairCache()->setOverlapFilterCallback(filterCallback); //register custom broadphase callback filter
		}

		~ArenaApp()
		{
			cleanup();
		}

		/// this is called once OpenGL is initialized
		void app_init()
		{
			app_scene = new octet::visual_scene();
			app_scene->create_default_camera_and_lights();
			camera = app_scene->get_camera_instance(0);
			camera->set_far_plane(1000.0f);
			timer = new Timer();
			timer->Start();

			objectPool = new ObjectPool();

			worldContext = new GameWorldContext(*app_scene, *world, *objectPool, *timer);

			floor = new Floor();
			floor->addPhysicsObjectToWorld(*worldContext);

			player = new Player();
			player->addPhysicsObjectToWorld(*worldContext);
			
			//player2 = new Player();
			//player2->addPhysicsObjectToWorld(*worldContext);

			HUD = new Hud();
			HUD->initialise();

			objectPool->Initialise(*worldContext, 25, 30);
			
			waveManager = new WaveManager(*worldContext, floor->width, floor->height);
			waveManager->SpawnPhysicsExampleWave();

			gContactAddedCallback = contactCallback;
		}

		void cameraFollow(Player& target)
		{
			camera->get_node()->loadIdentity();

			octet::vec3 targetPos = target.GetPosition();
			camera->get_node()->rotate(-90, octet::vec3(1, 0, 0));
			camera->get_node()->translate(octet::vec3(targetPos.x(), -targetPos.z(), 100));
		}
		/// this is called to draw the world
		void draw_world(int x, int y, int w, int h) {

			get_mouse_pos(curMouseX, curMouseY);

			update();

			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			HUD->draw(vx, vy);

			world->stepSimulation(1.0f / 30);

			objectPool->UpdatePhysicsObjects();

			// update matrices. assume 30 fps .
			app_scene->update(1.0f / 30);
			// draw the scene
			app_scene->render((float)vx / vy);
		}

		octet::vec3 calcMouseToWorld()
		{
			//camera->get_cameraToProjection().
			//camera->get_cameraToProjection
		}

		static bool contactCallback(btManifoldPoint &btmanifoldpoint,
			const btCollisionObjectWrapper *btcollisionobject0,
			int part_0, int index_0,
			const btCollisionObjectWrapper *btcollisionobject1,
			int part_1, int index_1)
		{
			Enemy *enemy = nullptr;
			Player *player = nullptr;
			Projectile *projectile = nullptr;
			PowerUp *powerUp = nullptr;

			for (int i = 0; i < 2; i++)
			{
				btRigidBody *rigidBody = nullptr;
				PhysicsObject *physObj = nullptr;
				
				if (i == 0)
					physObj = (PhysicsObject*)btcollisionobject0->getCollisionObject()->getUserPointer();
				else
					physObj = (PhysicsObject*)btcollisionobject1->getCollisionObject()->getUserPointer();

				if (physObj->GetReferenceType() == Enemy::referenceName)
					enemy = ((Enemy*)physObj);

				if (physObj->GetReferenceType() == Player::referenceName)
					player = ((Player*)physObj);

				if (physObj->GetReferenceType() == Projectile::referenceName)
					projectile = ((Projectile*)physObj);

				if (physObj->GetReferenceType() == PowerUp::referenceName)
				{
					powerUp = ((PowerUp*)physObj);
				}
			}

			//Player & Enemy Collision
			if (player != nullptr && enemy != nullptr)
			{
				player->TakeDamage(enemy->GetDamage());
				enemy->DestroyViaPool();
			}

			//Projectile & Enemy
			if (projectile != nullptr && enemy != nullptr)
			{
				enemy->TakeDamage(projectile->GetDamage());
				projectile->DestroyViaPool();
			}

			//Projectile & Player
			if (player != nullptr && projectile != nullptr)
			{
				if (player != projectile->owner)
				{
					player->TakeDamage(projectile->GetDamage());
					projectile->DestroyViaPool();
				}
			}

			//Powerup & Player
			if (powerUp != nullptr && player != nullptr)
			{
				powerUp->ApplyPowerUp(*player);
				powerUp->OnHit();
			}

			return false;
		}

		static struct customFilterCallback : public btOverlapFilterCallback
		{
			// return true when pairs need collision
			virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
			{
				bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
				collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);
				
				if ((proxy0->m_collisionFilterGroup == CollisionFlags::CollisionTypes::COL_ENEMY && proxy1->m_collisionFilterGroup == CollisionFlags::CollisionTypes::COL_PLAYER) ||
					(proxy1->m_collisionFilterGroup == CollisionFlags::CollisionTypes::COL_ENEMY && proxy0->m_collisionFilterGroup == CollisionFlags::CollisionTypes::COL_PLAYER))
				{
					collides = false;
				}
				return collides;
			}
		};
	};
}
