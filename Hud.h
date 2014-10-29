#pragma once
#include "../../octet.h"
#include "HUDText.h"

namespace Arena
{
	class Hud
	{
	private:
		HUDText *healthValue;
		HUDText *debugText;
		octet::bitmap_font *font;

		octet::ref<octet::text_overlay> overlay;

	public:
		Hud() 
		{
			overlay = new octet::text_overlay();
			font = overlay->get_default_font();
			healthValue = new HUDText(new octet::aabb(octet::vec3(250.0f, 300.0f, 0.0f), octet::vec3(10.0f, 20.0f, 30.0f)),font);
			debugText = new HUDText(new octet::aabb(octet::vec3(-350.0f, -300.0f, 0.0f), octet::vec3(10.0f, 20.0f, 30.0f)), font);
			initialise();
		}
		virtual ~Hud() {}

		void initialise()
		{
			
			healthValue->text = "Hello";
			debugText->text = "Hello";

			overlay->add_mesh_text(healthValue->mesh);
			overlay->add_mesh_text(debugText->mesh);
		}

		void draw(int vx, int vy)
		{
			debugText->draw();
			healthValue->draw();

			overlay->render(vx, vy);
		}

		void update(Player& player)
		{
			static char tmp[64];

			snprintf(tmp, sizeof(tmp), "health - [%i]", player.GetHealth());
			healthValue->text = tmp;

			octet::vec3 playerPos = player.GetPosition();
			debugText->text = playerPos.toString(tmp, sizeof(tmp));
		}
	};
}

