#pragma once
#include "Enemy.h"
namespace Arena
{
	class ExplodeEnemy : public Enemy
	{
	public:

		octet::ref<octet::param_uniform> progress;
		octet::ref<octet::param_uniform> fromExtrudeUniform;
		octet::ref<octet::param_uniform> toExtrudeUniform;

		float maxExtrude = 5.0f;
		float minExtrude = 0.0f;

		float fromExtrude = 0.0f;
		float toExtrude = 50.0f;

		ExplodeEnemy()
		{
			Initialise();
		}

		virtual ~ExplodeEnemy()
		{
		}

		virtual void Initialise() override
		{
			octet::vec3 position = octet::vec3(0.0f, 0.0f, 0.0f);
			octet::vec3 size = octet::vec3(2.5f);

			InitialiseExplodeShaderMaterial();

			octet::mesh *shape = new octet::mesh_box(size);
			btBoxShape *collisionShape = new btBoxShape(get_btVector3(size));

			PhysicsObject::Initialise(position, shape, collisionShape, mat);
			rigidBody->setActivationState(DISABLE_DEACTIVATION);

			Reset();
		}

		virtual void InitialiseExplodeShaderMaterial()
		{
			originalColour = octet::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			octet::param_shader *shader = new octet::param_shader("src/examples/arena/shaders/MeshTwitch.vs", "shaders/default_solid.fs");

			octet::atom_t atom_progress = octet::app_utils::get_atom("progress");
			octet::atom_t atom_toExtrude = octet::app_utils::get_atom("toExtrudeUniform");
			octet::atom_t atom_fromExtrude = octet::app_utils::get_atom("fromExtrudeUniform");

			mat = new octet::material(originalColour, shader);

			float val = 0.0f;
			progress = mat->add_uniform(&val, atom_progress, GL_FLOAT, 1, octet::param::stage_vertex);
			toExtrudeUniform = mat->add_uniform(&val, atom_toExtrude, GL_FLOAT, 1, octet::param::stage_vertex);
			fromExtrudeUniform = mat->add_uniform(&val, atom_fromExtrude, GL_FLOAT, 1, octet::param::stage_vertex);

		}
		virtual void Reset() override
		{
			baseSpeed = 10;
			speed = baseSpeed;
			maxSpeed = 30.0f;
			health = baseHealth;
			state = Alive;
		}

		virtual void Update(GameWorldContext& context) override
		{
			if (activeState == Enabled)
			{
				float progressValue = 0.0f;
				if (state == Dead)
				{
					float time = context.timer.GetRunningTime();
					progressValue = (time - deadTime) / deathDelay;

  					if (progressValue > 1.0f)
					{
						DestroyViaPool();
					}
				}
				mat->set_uniform(progress, &progressValue, sizeof(progressValue));
				mat->set_uniform(toExtrudeUniform, &toExtrude, sizeof(toExtrude));
				mat->set_uniform(fromExtrudeUniform, &fromExtrude, sizeof(fromExtrude));
			}
			Enemy::Update(context);
		}

		virtual void Die(GameWorldContext& context) override
		{
			state = Dead;
			deadTime = context.timer.GetRunningTime();
			mode = AIMode::Idle;
			this->rigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
			this->rigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
		}
	};
}

