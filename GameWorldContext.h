#pragma once
#include "IObjectPool.h"

namespace Arena
{
	struct GameWorldContext
	{
	public:
		octet::visual_scene &app_scene;
		btDiscreteDynamicsWorld &physicsWorld;
		IObjectPool &objectPool;

		GameWorldContext(octet::visual_scene& App, btDiscreteDynamicsWorld& PhysicsWorld, IObjectPool& ObjectPool) : app_scene(App), physicsWorld(PhysicsWorld), objectPool(ObjectPool)
		{
		}
	};
}

