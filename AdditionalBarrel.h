#pragma once
#include "PowerUp.h"

namespace Arena
{
namespace PowerUps
{
	///Represents a powerup that gives the player an extra turret barrel (and therefore extra projectiles)
	///on thier turret base. Uses variableColours.fs to interpolate between random two colours every x seconds
	class AdditionalBarrel : public PowerUp
	{
	private:
		octet::ref<octet::param_uniform> progress;
		octet::ref<octet::param_uniform> toColourParam;
		octet::ref<octet::param_uniform> fromColourParam;
		octet::vec4 fromColourValue = octet::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		octet::vec4 toColourValue = octet::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		float lastProgress = -1.0f;

	public:
		AdditionalBarrel(SoundManager& sound) : PowerUp(sound)
		{
			Initialise(octet::vec3(0.0f, 0.5f, 0.0f));
		}

		virtual ~AdditionalBarrel()
		{
		}

		virtual void Initialise(octet::vec3 position)
		{
			float radius = 0.5f;
			float halfExtents = 2.25f;
			octet::vec4 originalColour = octet::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			octet::mesh *barrelShape = new octet::mesh_cylinder(octet::math::zcylinder(octet::vec3(0.0f, 0.0f, 0.0f), radius, halfExtents));
			btSphereShape *collisionShape = new btSphereShape(halfExtents);

			octet::param_shader *shader = new octet::param_shader("shaders/default.vs", "src/examples/arena/shaders/variableColours.fs");

			octet::atom_t atom_progress = octet::app_utils::get_atom("progress");
			octet::atom_t atom_toColour = octet::app_utils::get_atom("toColour");
			octet::atom_t atom_fromColour = octet::app_utils::get_atom("fromColour");

			mat = new octet::material(originalColour, shader);

			float val = 0.0f;
			progress = mat->add_uniform(&val, atom_progress, GL_FLOAT, 1, octet::param::stage_fragment);
			toColourParam = mat->add_uniform(&toColourValue, atom_toColour, GL_FLOAT_VEC4, 1, octet::param::stage_fragment);
			fromColourParam = mat->add_uniform(&fromColourValue, atom_fromColour, GL_FLOAT_VEC4, 1, octet::param::stage_fragment);

			PowerUp::Initialise(position, barrelShape, collisionShape, mat);
		}

		virtual const char* GetReferenceType()
		{
			return AdditionalBarrel::referenceName;
		}

		virtual void ApplyPowerUp(Player& player)
		{
			player.AddBarrel();
		}

		virtual void DestroyViaPool() override
		{
			objectPool->DestroyActiveAdditionalBarrelObject(this);
			Disable();
		}

		virtual void Update(GameWorldContext& context) override
		{
			if (activeState == Enabled)
			{
				float intpart = 0.0f;
				float progressValue = std::modf(context.timer.GetRunningTime() / 2.5f, &intpart);
				mat->set_uniform(progress, &progressValue, sizeof(progressValue));

				if (progressValue < lastProgress)
				{
					fromColourValue = octet::vec4(toColourValue);
					toColourValue = GetRandomColour(context.rnd);
				}

				mat->set_uniform(toColourParam, &toColourValue, sizeof(toColourValue));
				mat->set_uniform(fromColourParam, &fromColourValue, sizeof(fromColourValue));
				lastProgress = progressValue;
			}
			PowerUp::Update(context);
		}

		octet::vec4 GetRandomColour(octet::random &rnd)
		{
			return octet::vec4(rnd.get(0.0f, 1.0f), rnd.get(0.0f, 1.0f), rnd.get(0.0f, 1.0f), 1.0f);
		}
	};
}
}

