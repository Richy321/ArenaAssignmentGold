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
		HUDText *debugText2;
		HUDText *poolingText;
		octet::bitmap_font *font;
		octet::ref<octet::text_overlay> overlay;

	public:
		Hud() 
		{
			overlay = new octet::text_overlay();
			font = overlay->get_default_font();

			healthValue = new HUDText(new octet::aabb(octet::vec3(250.0f, 300.0f, 0.0f), octet::vec3(10.0f, 20.0f, 30.0f)),font);
			debugText = new HUDText(new octet::aabb(octet::vec3(-350.0f, -300.0f, 0.0f), octet::vec3(10.0f, 20.0f, 50.0f)), font);
			debugText2 = new HUDText(new octet::aabb(octet::vec3(-350.0f, -350.0f, 0.0f), octet::vec3(10.0f, 20.0f, 50.0f)), font);
			poolingText = new HUDText(new octet::aabb(octet::vec3(250.0f, -250.0f, 0.0f), octet::vec3(100.0f, 40.0f, 30.0f)), font);
			
			
			initialise();
		}
		virtual ~Hud() {}

		void initialise()
		{
			
			healthValue->text = "Hello";
			debugText->text = "Hello";
			debugText2->text = "Hello";
			poolingText->text = "Hello";

			overlay->add_mesh_text(healthValue->mesh);
			overlay->add_mesh_text(debugText->mesh);
			overlay->add_mesh_text(debugText2->mesh);
			overlay->add_mesh_text(poolingText->mesh);
		}

		void draw(int vx, int vy)
		{
			debugText->draw();
			debugText2->draw();
			healthValue->draw();
			poolingText->draw();

			overlay->render(vx, vy);
		}

		void update(Player& player, ObjectPool& objPool)
		{
			static char tmp[64];

			snprintf(tmp, sizeof(tmp), "health - [%d]", player.GetHealth());
			healthValue->text = tmp;

			debugText->text = player.GetVelocity().toString(tmp, sizeof(tmp));
			debugText2->text = player.GetDampening().toString(tmp, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "EnemyPool - [%u]/[%u]\nProjPool - [%u]/[%u]", objPool.GetActiveEnemyCount(), objPool.GetInactiveEnemyCount(), objPool.GetActiveProjectileCount(), objPool.GetInactiveProjectileCount());

			poolingText->text = tmp;
		}
	};
}

