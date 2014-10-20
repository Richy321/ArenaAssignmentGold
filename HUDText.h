#pragma once
namespace Arena
{
	class HUDText
	{
	public:
		// information for our text
		octet::bitmap_font *font;
		// helper for drawing text
		octet::ref<octet::text_overlay> overlay;
		// text mesh object for overlay.
		octet::ref<octet::mesh_text> mesh;

		octet::string text;
		// a texture for our text
		GLuint font_texture;
		HUDText(octet::aabb *bounds = 0)
		{
			initialise(bounds);
		}

		~HUDText()
		{
		}


		void initialise(octet::aabb *bounds = 0)
		{
			overlay = new octet::text_overlay();
			font = overlay->get_default_font();
			text = "";

			mesh = new octet::mesh_text(font, "initTestings", bounds);

			overlay->add_mesh_text(mesh);
		}
		void draw(int vx, int vy)
		{
			mesh->clear();
			mesh->format(text);
			mesh->update();

			overlay->render(vx, vy);
		}
	};
}

