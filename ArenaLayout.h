#pragma once
#include "Floor.h"
#include "Wall.h"

namespace Arena
{
	class ArenaLayout
	{
	public:
		Floor* floor = nullptr;
		octet::dynarray<Wall*> walls;

		ArenaLayout()
		{
		}

		virtual ~ArenaLayout()
		{
		}
	};
}

