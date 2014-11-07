#pragma once
#include "Enemy.h"

namespace Arena
{
	class VaryingColourEnemy : public Enemy
	{
	public:

		octet::ref<octet::param_uniform> progress;
		octet::ref<octet::param_uniform> toColourParam;
		octet::ref<octet::param_uniform> fromColourParam;

		octet::vec4 fromColourValue = octet::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		octet::vec4 toColourValue = octet::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		float lastProgress = -1.0f;

		VaryingColourEnemy()
		{
			Initialise();
		}

		virtual ~VaryingColourEnemy()
		{
		}

		virtual void Initialise() override
		{
			octet::vec3 position = octet::vec3(0.0f, 0.0f, 0.0f);
			octet::vec3 size = octet::vec3(2.5f);

			originalColour = octet::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			octet::param_shader *shader = new octet::param_shader("shaders/default.vs", "src/examples/arena/shaders/variableColours.fs");

			octet::atom_t atom_progress = octet::app_utils::get_atom("progress");
			octet::atom_t atom_toColour = octet::app_utils::get_atom("toColour");
			octet::atom_t atom_fromColour = octet::app_utils::get_atom("fromColour");
			
			mat = new octet::material(originalColour, shader);

			float val = 0.0f;
			progress = mat->add_uniform(&val, atom_progress, GL_FLOAT, 1, octet::param::stage_fragment);
			toColourParam = mat->add_uniform(&toColourValue, atom_toColour, GL_FLOAT_VEC4, 1, octet::param::stage_fragment);
			fromColourParam = mat->add_uniform(&fromColourValue, atom_fromColour, GL_FLOAT_VEC4, 1, octet::param::stage_fragment);

			octet::mesh *shape = new octet::mesh_box(size);
			btBoxShape *collisionShape = new btBoxShape(get_btVector3(size));

			PhysicsObject::Initialise(position, shape, collisionShape, mat);
			rigidBody->setActivationState(DISABLE_DEACTIVATION);

			baseSpeed = 10;
			speed = baseSpeed;
			maxSpeed = 30.0f;
		}

		virtual void Update(GameWorldContext& context) override
		{
			if (activeState == Enabled)
			{
				float intpart = 0.0f;
				float progressValue = std::modf(context.timer.GetRunningTime() / 10, &intpart);
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
			Enemy::Update(context);
		}

		octet::vec4 GetRandomColour(octet::random &rnd)
		{
			return octet::vec4(rnd.get(0.0f, 1.0f), rnd.get(0.0f, 1.0f), rnd.get(0.0f, 1.0f), 1.0f);
		}
	};
}

