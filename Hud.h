#pragma once
#include "../../octet.h"
#include "HUDText.h"

namespace Arena
{
	class Hud
	{
	private:
		HUDText *healthValue;
		//HUDText *debugText;
	public:
		Hud() 
		{
			healthValue = new HUDText(new octet::aabb(octet::vec3(250.0f, 300.0f, 0.0f), octet::vec3(10.0f, 20.0f, 30.0f)));
			//debugText = new HUDText(new octet::aabb(octet::vec3(-350.0f, -300.0f, 0.0f), octet::vec3(10.0f, 20.0f, 30.0f)));
			initialise();
		}
		virtual ~Hud() {}

		void initialise()
		{
			healthValue->initialise();
			healthValue->text = "Hello";
			//debugText->initialise();
			//debugText->text = "Hello";
		}

		void draw(int vx, int vy)
		{
			//debugText->draw(vx, vy);
			healthValue->draw(vx, vy);
		}

		void update(Player& player)
		{
			static char tmp[64];

			snprintf(tmp, sizeof(tmp), "health - [%i]", player.GetHealth());
			healthValue->text = tmp;

			//octet::vec3 playerPos = player.GetPosition();

			//debugText->text = playerPos.toString(tmp, sizeof(tmp));
		}
	};
}

