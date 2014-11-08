#pragma once
#include "PowerUp.h"

namespace Arena
{
namespace PowerUps
{
	class Health : public PowerUp
	{
	public:

		unsigned int amount = 50;
		octet::mesh_instance *secondMesh;
		Health(SoundManager &sound) : PowerUp(sound)
		{
			Initialise(octet::vec3(0.0f, 0.0f, 0.0f));
		}

		virtual ~Health()
		{
		}

		virtual void Initialise(octet::vec3 position)
		{
			float width = 0.33f;
			float extents = 1.0f;

			//TODO - consolidate to one mesh to remove overlapping verts/faces
			octet::mesh *plusV = new octet::mesh_box(octet::vec3(width, width, extents));
			octet::mesh *plusH = new octet::mesh_box(octet::vec3(extents, width, width));

			btSphereShape *collisionShape = new btSphereShape(extents);
			mat = new octet::material(octet::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			
			PowerUp::Initialise(position, plusH, collisionShape, mat);
			
			secondMesh = new octet::mesh_instance(node, plusV, mat);
		}

		virtual void addPhysicsObjectToWorld(GameWorldContext& context) override
		{
			context.app_scene.add_mesh_instance(secondMesh);
			PowerUp::addPhysicsObjectToWorld(context);
		}

		virtual void ApplyPowerUp(Player& player)
		{
			player.SetHealth(player.GetHealth() + amount);
		}

		virtual void DestroyViaPool() override
		{
			objectPool->DestroyActiveHealthObject(this);
			Disable();
		}

	};
}
}

