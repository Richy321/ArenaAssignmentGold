#pragma once
namespace octet {
	class IPhysicsCharacter
	{
	public:
		material* mat;
		bool isDynamic = true;
		btRigidBody* rigidBody;
		scene_node *node;
		mesh_box *box;
		vec3 size;
		mesh_instance* mesh;

		virtual ~IPhysicsCharacter() = 0;

		virtual material &GetMaterial() = 0;
		virtual btRigidBody &GetRigidBody() = 0;
		virtual mesh_box &GetMesh() = 0;
		virtual scene_node &GetNode() = 0;
	};
}
