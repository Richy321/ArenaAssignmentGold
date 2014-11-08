#pragma once
#include "../../octet.h"
#include "HUDText.h"
#include "GameMode.h"

namespace Arena
{
	class Hud
	{
	private:
		HUDText *debugText;
		HUDText *debugText2;
		HUDText *poolingText;
		HUDText *optionsText;
		HUDText *waveCountdownText;
		HUDText *player1Stats;
		HUDText *player2Stats;

		octet::bitmap_font *font;
		octet::ref<octet::text_overlay> overlay;

	public:
		Hud() 
		{
			overlay = new octet::text_overlay();
			font = overlay->get_default_font();

			debugText = new HUDText(new octet::aabb(octet::vec3(-350.0f, -300.0f, 0.0f), octet::vec3(20.0f, 20.0f, 50.0f)), font);
			debugText2 = new HUDText(new octet::aabb(octet::vec3(-350.0f, -350.0f, 0.0f), octet::vec3(20.0f, 20.0f, 50.0f)), font);
			poolingText = new HUDText(new octet::aabb(octet::vec3(250.0f, -250.0f, 0.0f), octet::vec3(100.0f, 40.0f, 30.0f)), font);
			waveCountdownText = new HUDText(new octet::aabb(octet::vec3(0.0f, 0.0f, 0.0f), octet::vec3(50.0f, 20.0f, 50.0f)), font);
			optionsText = new HUDText(new octet::aabb(octet::vec3(35.0f, 25.0f, 0.0f), octet::vec3(70.0f, 50.0f, 0.0f)), font);

			player1Stats = new HUDText(new octet::aabb(octet::vec3(250.0f, 300.0f, 0.0f), octet::vec3(90.0f, 50.0f, 0.0f)), font);
			player2Stats = new HUDText(new octet::aabb(octet::vec3(-250.0f, 300.0f, 0.0f), octet::vec3(90.0f, 50.0f, 0.0f)), font);

			initialise();
		}
		virtual ~Hud() {}

		void initialise()
		{			
			overlay->add_mesh_text(debugText->mesh);
			overlay->add_mesh_text(debugText2->mesh);
			overlay->add_mesh_text(poolingText->mesh);
			overlay->add_mesh_text(waveCountdownText->mesh);
			overlay->add_mesh_text(optionsText->mesh);
			overlay->add_mesh_text(player1Stats->mesh);
			overlay->add_mesh_text(player2Stats->mesh);
		}

		void draw(int vx, int vy, GameMode mode)
		{
			debugText->draw();
			debugText2->draw();
			poolingText->draw();
			waveCountdownText->draw();
			player1Stats->draw();
			player2Stats->draw();

			if (mode == GameMode::None)
				optionsText->draw();
			else
			{
				optionsText->mesh->clear();
				optionsText->mesh->update();
			}

			overlay->render(vx, vy);
		}

		void update(Player* player, Player* player2, ObjectPool& objPool, WaveManager& waveManager, GameMode mode)
		{
			static char tmp[64];
		
			debugText->text = player->GetVelocity().toString(tmp, sizeof(tmp));
			debugText2->text = player->GetDampening().toString(tmp, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "EnemyPool - %u/%u\nProjPool - %u/%u", objPool.GetActiveEnemyCount(), objPool.GetInactiveEnemyCount(), objPool.GetActiveProjectileCount(), objPool.GetInactiveProjectileCount());
			poolingText->text = tmp;

			float waveDelayTimeRemaining = waveManager.remainingWaveDelayTime;
			if (waveDelayTimeRemaining > 0.0f)
			{
  				snprintf(tmp, sizeof(tmp), "Next Wave(%d) in %.2f seconds", waveManager.currentWave, waveDelayTimeRemaining);
				waveCountdownText->text = tmp;
			}
			else
				waveCountdownText->text = "";

			if (mode == GameMode::None)
			{
				snprintf(tmp, sizeof(tmp), "Choose Mode:\n[F1] - Solo\n[F2] - Versus\n[F3] - Coop");
				optionsText->text = tmp;
			}
			else
				optionsText->text = "";

			snprintf(tmp, sizeof(tmp), "Player1 Lives: %d\n       Health: %d", player->GetRemainingLives(), player->GetHealth());
			player1Stats->text = tmp;

			if (player2 != nullptr)
			{
				snprintf(tmp, sizeof(tmp), "Player2 Lives: %d\n       Health: %d", player2->GetRemainingLives(), player2->GetHealth());
				player2Stats->text = tmp;
			}
			else
				player2Stats->text = "";
		}
	};
}

