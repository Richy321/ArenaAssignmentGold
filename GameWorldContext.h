#pragma once
#include "IObjectPool.h"
#include "Timer.h"
#include "SoundManager.h"

namespace Arena
{
	///Combines all commonly used object refs for convenient parameter passing.
	struct GameWorldContext
	{
	public:
		octet::visual_scene &app_scene;
		btDiscreteDynamicsWorld &physicsWorld;
		IObjectPool &objectPool;
		Timer &timer;
		octet::random& rnd;
		SoundManager& soundManager;

		GameWorldContext(octet::visual_scene& App, btDiscreteDynamicsWorld& PhysicsWorld, IObjectPool& ObjectPool, Timer& Timer, octet::random& Random, SoundManager& Sound) : app_scene(App), physicsWorld(PhysicsWorld), objectPool(ObjectPool), timer(Timer), rnd(Random), soundManager(Sound)
		{
		}
	};
}

