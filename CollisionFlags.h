#pragma once
namespace Arena
{
	class CollisionFlags
	{
	public:
		static enum CollisionTypes
		{
			COL_NOTHING = 0,
			COL_ENEMY = 1,
			COL_WALL = 2,
			COL_POWERUP = 4,
			COL_PLAYER = 8,
			COL_PROJECTILES = 16
		};
	};
	
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
			//add some additional logic here that modified 'collides'
			return collides;
		}
	};
}



