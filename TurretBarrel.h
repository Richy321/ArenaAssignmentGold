#pragma once
#define OCTET_BULLET 1
#include "../../octet.h"
#include "PhysicsObject.h"

namespace Arena
{
	class TurretBarrel
	{
	private:
		float lastFireTime = -1.0f;
		octet::vec3 forward = octet::vec3(0.0f, 0.0f, -1.0f);
		float barrelOffset = 4.0f;
		octet::vec3 offset = octet::vec3(0.0f, 0.0f, 0.0f);
		octet::mesh_instance *mesh;
		octet::scene_node *node;
		octet::material *mat;
		PhysicsObject *owner;

	public:
		static const char* referenceName;
		float radius = 0.5f;
		float halfExtents = 2.25f;

		float fireForce = 1000.0f;
		float fireRate = 0.3f;

		TurretBarrel(GameWorldContext context, octet::scene_node* node, octet::material* mat, octet::vec3 position, PhysicsObject *owner)
		{
			this->node = node;
			this->mat = mat;
			this->owner = owner;
			offset = position;
			Initialise(position, context);
		}

		virtual ~TurretBarrel()
		{

		}

		void Initialise(octet::vec3 position, GameWorldContext context)
		{
			octet::mat4t modelToParentMatrix = octet::mat4t();
			modelToParentMatrix.translate(position.x(), position.y(), position.z());
			octet::mesh *barrelShape = new octet::mesh_cylinder(octet::math::zcylinder(octet::vec3(0.0f, 0.0f, 0.0f), radius, halfExtents), modelToParentMatrix);
			
			mesh = new octet::mesh_instance(node, barrelShape, mat);
			context.app_scene.add_mesh_instance(mesh);
		}

		void Fire(GameWorldContext& context)
		{
			float runningTime = context.timer.GetRunningTime();
			if (lastFireTime + fireRate < runningTime)
			{
				Projectile *proj = context.objectPool.GetProjectileObject();
				proj->owner = owner;

				octet::vec3 newPos = forward + offset;
				newPos += octet::vec3(0.0f, 0.0f, barrelOffset);

				newPos = newPos * node->access_nodeToParent().toQuaternion();

				octet::vec3 newFwd = forward * node->access_nodeToParent().toQuaternion();
				newFwd = newFwd.normalize();

				proj->SetWorldTransform(node->access_nodeToParent());
				proj->Translate(newPos);

				newFwd *= fireForce;
				btVector3 force = get_btVector3(newFwd);

				proj->GetRigidBody()->applyImpulse(force, btVector3(0.0f, 0.0f, 0.0f));

				lastFireTime = runningTime;
			}
		}

		octet::mesh_instance& GetMeshInstance() { return *mesh; }
	};

	const char *TurretBarrel::referenceName = "TurretBarrel";
}

