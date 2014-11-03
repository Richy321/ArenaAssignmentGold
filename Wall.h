#pragma once

namespace Arena
{
	class Wall : public PhysicsObject
	{
	public:

		const float thickness = 0.5f;

		Wall()
		{
		}

		virtual ~Wall()
		{
		}
	};
}

