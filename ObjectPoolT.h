#pragma once
#include "PhysicsObject.h"
#include "GameWorldContext.h"
namespace Arena
{
	template<class T> class ObjectPoolT
	{
		octet::dynarray<Enemy*> active;
		octet::dynarray<Enemy*> inactive;
		GameWorldContext& context;
		
		ObjectPoolT<T>(GameWorldContext& Context, int initialCapacity) : context(Context)
		{
			active.reserve(initialCapacity);
			inactive.reserve(initialCapacity);

			for (int i = 0; i < initialCapacity; i++)
			{
				T* obj = CreateNew();
				//obj->Disable();
				inactive.push_back(obj);
			}
		}

		T* CreateNew()
		{
			T* enemy = new Enemy();
			enemy->addPhysicsObjectToWorld(*gameWorldContext);
			return enemy;
		}

		virtual ~ObjectPoolT<T>()
		{

		}

		T* Get()
		{
			T* obj = nullptr;
			if (inactiveEnemies.size() > 0)
			{
				obj = inactiveEnemies[inactiveEnemies.size() - 1];
				inactive->Translate(get_btVector3(position));
				inactive.pop_back();
			}
			else
			{
				//allocate new - try to avoid this
				obj = new Enemy();
				obj->addPhysicsObjectToWorld(*gameWorldContext);
			}

			active.push_back(obj);
			//obj->Enable();
			return obj;
		}

		unsigned int GetActiveCount() override { return active.size(); }
		unsigned int GetInactiveCount(){ return inactive.size(); }
	};
}
