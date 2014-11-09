////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
#define OCTET_BULLET 1
#include <functional>
#include "../../octet.h"

#include "IObjectPool.h"
#include "GameWorldContext.h"
#include "ObjectPool.h"
#include "ObjectPoolT.h"
#include "SoundManager.h"
#include "PhysicsObject.h"
#include "Player.h"
#include "Enemy.h"
#include "Floor.h"
#include "Projectile.h"
#include "Timer.h"
#include "WaveManager.h"
#include "PowerUp.h"
#include "HUD.h"
#include "ArenaLayout.h"
#include "GameMode.h"
#include "SkyPlane.h"
#include "XInputJoypad.h"

namespace Arena
{
	/// Main Arena game app
	class ArenaApp : public octet::app 
	{
	private:
		octet::ref<octet::visual_scene> app_scene;
		btDefaultCollisionConfiguration config;       /// setup for the world
		btCollisionDispatcher *dispatcher;            /// handler for collisions between objects
		btDbvtBroadphase *broadphase;                 /// handler for broadphase (rough) collision
		btSequentialImpulseConstraintSolver *solver;  /// handler to resolve collisions
		btDiscreteDynamicsWorld *world;               /// physics world, contains rigid bodies
		octet::camera_instance *camera;				  /// main camera instance 

		Player *player = nullptr;	///Player1
		Player *player2 = nullptr;  ///Player2 - if present
		Floor *floor;
		ObjectPool *objectPool;
		Timer *timer;
		WaveManager *waveManager;
		ArenaLayout *arena;
		SoundManager *sound;

		Hud *HUD;
		XInputJoypad *joypad;	///joypad in slot one
		XInputJoypad *joypad2;  ///joypad in slot two

		GameWorldContext *worldContext;
		btOverlapFilterCallback *filterCallback;

		GameMode mode = GameMode::Solo;

		octet::random *rnd;

		SkyPlane *skyPlane;

		unsigned int lastSpawnPointIndex = 0;

		//Mouse variables
		int prevMouseX = -1;
		int prevMouseY = -1;
		int curMouseX = -1;
		int curMouseY = -1;
		float sensitivity = 0.3f;
		const int joypadThreshold = 50;

		const float arenaWidth = 50.0f;
		const float arenaHeight = 50.0f;

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

		void HandleKeyboardInput()
		{
			if (mode != None)
			{
				if ((is_key_down(octet::key_left) || is_key_down('A') || is_key_down('a')) ||
					(is_key_down(octet::key_right) || is_key_down('D') || is_key_down('d')) ||
					(is_key_down(octet::key_up) || is_key_down('W') || is_key_down('w')) ||
					(is_key_down(octet::key_down) || is_key_down('S') || is_key_down('s')))
				{
					if (is_key_down(octet::key_left) || is_key_down('A') || is_key_down('a'))
						player->Move(octet::vec3(-1.0f, 0.0f, 0.0f), Player::Direction::East);
					if (is_key_down(octet::key_right) || is_key_down('D') || is_key_down('d'))
						player->Move(octet::vec3(1.0f, 0.0f, 0.0f), Player::Direction::West);
					if (is_key_down(octet::key_up) || is_key_down('W') || is_key_down('w'))
						player->Move(octet::vec3(0.0f, 0.0f, -1.0f), Player::Direction::North);
					if (is_key_down(octet::key_down) || is_key_down('S') || is_key_down('s'))
						player->Move(octet::vec3(0.0f, 0.0f, 1.0f), Player::Direction::South);
				}

				if (is_key_down(octet::key_down) || is_key_down('Q') || is_key_down('q') ||
					(is_key_down(octet::key_down) || is_key_down('E') || is_key_down('e')))
				{
					if (is_key_down(octet::key_down) || is_key_down('Q') || is_key_down('q'))
						player->RotateTurret(1.0f);

					if (is_key_down(octet::key_down) || is_key_down('E') || is_key_down('e'))
						player->RotateTurret(-1.0f);
				}

				if (is_key_down(octet::key_space))
					player->FireTurrets(*worldContext);
			}

			if (is_key_down(octet::key_f1))
			{
				if (mode == None)
				{
					InitGameMode(Solo);
				}
				else
				{
					for (unsigned int i = 0; i < objectPool->GetActiveEnemies().size(); i++)
					{
						objectPool->GetActiveEnemies()[i]->SetTarget(nullptr);
						objectPool->GetActiveEnemies()[i]->SetAIMode(Enemy::AIMode::Idle);
					}
				}
			}

			if (is_key_down(octet::key_f2))
			{
				if (mode == None)
				{
					InitGameMode(Versus);
				}
				else
				{
					for (unsigned int i = 0; i < objectPool->GetActiveEnemies().size(); i++)
					{
						objectPool->GetActiveEnemies()[i]->SetTarget(player);
						objectPool->GetActiveEnemies()[i]->SetAIMode(Enemy::AIMode::Chase);
					}
				}
			}

			if (is_key_down(octet::key_f3))
			{
				if (mode == None)
				{
					InitGameMode(Coop);
				}
			}

			if (is_key_down(octet::key_f9))
			{
				objectPool->KillAllActiveEnemys();
			}

			if (is_key_down(octet::key_esc))
			{
				cleanup();
				exit(1); //quick exit, cleanup?
			}
		}

		void handleInput()
		{
			HandleKeyboardInput();
			HandleXInputJoypads();
		}

		void HandleXInputJoypads()
		{
			if (joypad->Connected())
			{
				HandleXInputJoypad(*player, *joypad);
			}

			if (joypad2->Connected() && player2 != nullptr && (mode == Coop || mode == Versus))
			{
				HandleXInputJoypad(*player2, *joypad2);
			}
		}

		void HandleXInputJoypad(Player& player, XInputJoypad& curJoypad)
		{
			if (!curJoypad.LStickInDeadzone())
				if(curJoypad.LeftStick_X() < 0.0f)
					player.Move(octet::vec3(-1.0f, 0.0f, 0.0f), Player::Direction::East);

			if (!curJoypad.LStickInDeadzone())
				if (curJoypad.LeftStick_X() > 0.0f)
					player.Move(octet::vec3(1.0f, 0.0f, 0.0f), Player::Direction::West);

			if (!curJoypad.LStickInDeadzone())
				if (curJoypad.LeftStick_Y() < 0.0f)
					player.Move(octet::vec3(0.0f, 0.0f, 1.0f), Player::Direction::South);

			if (!curJoypad.LStickInDeadzone())
				if (curJoypad.LeftStick_Y() > 0.0f)
					player.Move(octet::vec3(0.0f, 0.0f, -1.0f), Player::Direction::North);

			if (!curJoypad.RStickInDeadzone())
				if (curJoypad.RightStick_X() > 0.0f)
					player.RotateTurret(-1.0f);

			if (!curJoypad.RStickInDeadzone())
				if (curJoypad.RightStick_X() < 0.0f)
					player.RotateTurret(1.0f);

			if (curJoypad.LeftTrigger() > 0.0f || curJoypad.RightTrigger() > 0.0f)
				player.FireTurrets(*worldContext);
		}

		void cleanup()
		{
			waveManager->state = WaveManager::Inactive;
			delete world;
			delete solver;
			delete broadphase;
			delete dispatcher;
		}

		void update()
		{
			if (mode != None)
			{
				int livesCount = 0;
				if (player != nullptr)
					livesCount += player->GetRemainingLives();
				if (player2 != nullptr && (mode == Coop || mode == Versus))
					livesCount += player2->GetRemainingLives();

				if (livesCount == 0)
					mode = None;
			}
			
			timer->Update();
			handleInput();
			if (mode == GameMode::Solo)
				cameraFollow((*player));
			
			HUD->update(player, player2, *objectPool, *waveManager, mode);

			waveManager->Update(*worldContext, player, player2, mode);
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

			world->getPairCache()->setOverlapFilterCallback(filterCallback); //register custom broadphase callback filter
		}

		~ArenaApp()
		{
			cleanup();
		}

		void PlayerRespawn(Player& player)
		{
			unsigned int spawnIndex = lastSpawnPointIndex;
			octet::vec3 pos;

			while (spawnIndex == lastSpawnPointIndex)
				pos = arena->GetRandomSpawnPoint(spawnIndex);

			octet::mat4t transform;
			transform.translate(pos.x(), pos.y() + 5.0f, pos.z());
			player.SetWorldTransform(transform);
			lastSpawnPointIndex = spawnIndex;
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

			camera->get_node()->loadIdentity();
			camera->get_node()->rotate(-90, octet::vec3(1, 0, 0));
			camera->get_node()->translate(octet::vec3(0.0f, 0.0f, 130));

			mode = None;

			joypad = new XInputJoypad(0);
			joypad2 = new XInputJoypad(1);

			sound = new SoundManager();

			objectPool = new ObjectPool();
			rnd = new octet::random(timer->GetTime());
			worldContext = new GameWorldContext(*app_scene, *world, *objectPool, *timer, *rnd, *sound);
			
			objectPool->Initialise(*worldContext, 25, 30, WaveManager::maxPowerUpsPerType, WaveManager::maxPowerUpsPerType);

			arena = new ArenaLayout(arenaWidth, arenaHeight, *worldContext);
			waveManager = new WaveManager(*arena, *worldContext);

			skyPlane = new SkyPlane(arenaWidth + 100.0f, arenaHeight + 100.0f, octet::vec3(0.0f, 0.0f, 0.0f), *worldContext);

			HUD = new Hud();
			HUD->initialise();

			gContactAddedCallback = contactCallback;
		}

		void InitGameMode(GameMode newMode)
		{
			if (player == nullptr)
			{
				player = new Player();
				player->addPhysicsObjectToWorld(*worldContext);
				player->respawnCallback = std::bind(&ArenaApp::PlayerRespawn, this, std::placeholders::_1);
			}

			switch (newMode)
			{
			case Solo:
				player->Initialise();
				waveManager->state = WaveManager::BetweenWaves;
				break;
			case Versus:
				waveManager->state = WaveManager::Inactive;
				if (player2 == nullptr)
				{
					player2 = new Player();
					player2->addPhysicsObjectToWorld(*worldContext);
					player2->respawnCallback = std::bind(&ArenaApp::PlayerRespawn, this, std::placeholders::_1);
				}
				player2->Initialise();
				player2->Translate(octet::vec3(5.0f, 0.0f, 0.0f));
				break;
			case Coop:
				waveManager->state = WaveManager::BetweenWaves;
				if (player2 == nullptr)
				{
					player2 = new Player();
					player2->addPhysicsObjectToWorld(*worldContext);
					player2->respawnCallback = std::bind(&ArenaApp::PlayerRespawn, this, std::placeholders::_1);
				}
				player2->Initialise();
				player2->Translate(octet::vec3(5.0f, 0.0f, 0.0f));
				break;
			}
			mode = newMode;
		}

		///Follows a Player target using a top down view.
		void cameraFollow(Player& target)
		{
			if (target.curState == Player::State::Alive)
			{
				camera->get_node()->loadIdentity();

				octet::vec3 targetPos = target.GetPosition();
				camera->get_node()->rotate(-90, octet::vec3(1, 0, 0));
				camera->get_node()->translate(octet::vec3(targetPos.x(), -targetPos.z(), 100));
			}
		}
		/// this is called to draw the world
		void draw_world(int x, int y, int w, int h) {

			get_mouse_pos(curMouseX, curMouseY);

			update();

			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			HUD->draw(vx, vy, mode);

			world->stepSimulation(1.0f / 30);

			objectPool->UpdatePhysicsObjects(*worldContext);

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);
			// draw the scene
			app_scene->render((float)vx / vy);
		}

		octet::vec3 calcMouseToWorld()
		{
			//camera->get_cameraToProjection().
			//camera->get_cameraToProjection
		}

		///contact callback function to enable custom actions based on detection of specific object-object collision
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
			Floor *floor = nullptr;

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

				if (physObj->GetReferenceType() == Floor::referenceName)
				{
					floor = ((Floor*)physObj);
				}
			}

			//Player & Enemy Collision
			if (player != nullptr && enemy != nullptr)
			{
				player->TakeDamage(enemy->GetDamage());
				enemy->TakeDamage(player->collisionDamage);
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

		///Early collision detection callback to prevent collision response on certain occasions.
		///Ignore enemy collisions when the enemy is dead (can still be animating but we don't want a collision)
		///Using this due to problems disabling simulation on rigid body
		static struct customFilterCallback : public btOverlapFilterCallback
		{
			// return true when pairs need collision
			virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
			{
				bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
				collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);
				
				if (proxy0->m_collisionFilterGroup == CollisionFlags::CollisionTypes::COL_ENEMY)
				{
					btRigidBody* rb0 = btRigidBody::upcast(static_cast<btCollisionObject*>(proxy0->m_clientObject));
					if (rb0 != NULL)
					{
						Enemy* nme = (Enemy*)rb0->getUserPointer();
						if (nme->state == Enemy::state::Dead)
							collides = false;
					}
				}

				if (proxy1->m_collisionFilterGroup == CollisionFlags::CollisionTypes::COL_ENEMY)
				{
					btRigidBody* rb0 = btRigidBody::upcast(static_cast<btCollisionObject*>(proxy1->m_clientObject));
					if (rb0 != NULL)
					{
						Enemy* nme = (Enemy*)rb0->getUserPointer();
						if (nme->state == Enemy::state::Dead)
							collides = false;
					}
				}

				return collides;
			}
		};
	};
}
