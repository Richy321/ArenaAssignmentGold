#pragma once
#include "Enemy.h"
namespace Arena
{
	///Enemy TNT crate that explodes on death.
	//Uses Explode.vs
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
		bool playingExplosionSound = false;

		ExplodeEnemy(GameWorldContext& context)
		{
			Initialise(context);
		}

		virtual ~ExplodeEnemy()
		{
		}

		virtual void Initialise(GameWorldContext& context)
		{
			octet::vec3 position = octet::vec3(0.0f, 0.0f, 0.0f);
			octet::vec3 size = octet::vec3(2.5f);

			InitialiseExplodeShaderMaterial(context);

			octet::mesh *shape = new octet::mesh_box(size);
			btBoxShape *collisionShape = new btBoxShape(get_btVector3(size));

			PhysicsObject::Initialise(position, shape, collisionShape, mat);
			rigidBody->setActivationState(DISABLE_DEACTIVATION);

			Reset();
		}

		///Initialise the Explode shader with progress and to/from values
		virtual void InitialiseExplodeShaderMaterial(GameWorldContext& context)
		{
			originalColour = octet::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			octet::image* img = context.objectPool.GetTexture("ExplodeCrate");
			octet::param_shader *shader = new octet::param_shader("src/examples/arena/shaders/Explode.vs", "shaders/default_textured.fs");

			octet::atom_t atom_progress = octet::app_utils::get_atom("progress");
			octet::atom_t atom_toExtrude = octet::app_utils::get_atom("toExtrudeUniform");
			octet::atom_t atom_fromExtrude = octet::app_utils::get_atom("fromExtrudeUniform");

			mat = new octet::material(img, NULL, shader);

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
			playingExplosionSound = false;
		}

		virtual void Update(GameWorldContext& context) override
		{
			if (activeState == Enabled)
			{
				float progressValue = 0.0f;
				if (state == Dead)
				{
					if (!playingExplosionSound)
					{
						context.soundManager.Play(context.soundManager.enemyExplode);
						playingExplosionSound = true;
					}
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
			rigidBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
			rigidBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
		}
	};
}

