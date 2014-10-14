#pragma once
namespace octet {
	namespace scene {

		class RoamCamera 
		{
		private:
			vec3 up;
			vec3 forward;
			vec3 right;
			
			scene_node* nodeY;
			
		public:
			camera_instance* camera;
			scene_node* nodeX;
			RoamCamera()
			{
				up = vec3(0, 1, 0);
				forward = vec3(0, 0, 1);
				right = vec3(1, 0, 0);

				//nodeX = new scene_node();
				//nodeY = new scene_node();
				camera = new camera_instance();

				//nodeX->add_child(nodeY);
				//camera->set_node(nodeY);
			}
			~RoamCamera() {}

			void InitialiseCamera(vec3 pos, ref<visual_scene> app_scene)
			{
				nodeX = app_scene->add_scene_node();
				nodeY = new scene_node(nodeX->get_nodeToParent(), atom_t(1));
				nodeX->add_child(nodeY);
				nodeX->access_nodeToParent().translate(pos.x(), pos.y(), pos.z());
				float f = pos.z() * 2, n = f * 0.001f;
				
				camera->set_node(nodeY);
				camera->set_perspective(0, 45, 1, n, f);
			}

			void RotateX(float angle)
			{
				//if (angle > 0)
					//nodeX->rotate(angle, right);
			}

			void RotateY(float angle)
			{
				//if (angle > 0)
					//nodeY->rotate(angle, up);
			}

			void RotateZ(float angle)
			{
				//this->get_node()->rotate(angle, forward);
			}
		};
	}
}
