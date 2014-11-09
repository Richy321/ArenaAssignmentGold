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
}



