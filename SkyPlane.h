#pragma once
#include "GameWorldContext.h"
#include "../../octet.h"

namespace Arena
{
	///Background sky box
	class SkyPlane
	{

	private:
		octet::material* mat;
		octet::scene_node* node;
		octet::mesh_instance* mesh;
	public:

		SkyPlane(float width, float height, octet::vec3 position, GameWorldContext& context)
		{
			octet::mat4t modelToWorld = octet::mat4t();
			modelToWorld.translate(position.x(), position.y(), position.z());

			octet::image *img = context.objectPool.GetTexture("SkyTexture");
			mat = new octet::material(img);

			octet::mesh *shape = new octet::mesh_box(octet::vec3(width*4, width*4, height*4));
			
			node = new octet::scene_node(modelToWorld, octet::atom_);
			mesh = new octet::mesh_instance(node, shape, mat);

			context.app_scene.add_scene_node(node);
			context.app_scene.add_mesh_instance(mesh);
		}

		virtual ~SkyPlane()
		{
		}
	};
}

