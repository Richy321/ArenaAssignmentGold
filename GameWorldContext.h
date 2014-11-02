#pragma once
#include "IObjectPool.h"
#include "Timer.h"

namespace Arena
{
	struct GameWorldContext
	{
	public:
		octet::visual_scene &app_scene;
		btDiscreteDynamicsWorld &physicsWorld;
		IObjectPool &objectPool;
		Timer &timer;

		GameWorldContext(octet::visual_scene& App, btDiscreteDynamicsWorld& PhysicsWorld, IObjectPool& ObjectPool, Timer& Timer) : app_scene(App), physicsWorld(PhysicsWorld), objectPool(ObjectPool), timer(Timer)
		{
		}
	};
}

