#pragma once
namespace octet
{
	class HUDText
	{
	public:
		// information for our text
		bitmap_font *font;
		// helper for drawing text
		ref<text_overlay> overlay;
		// text mesh object for overlay.
		ref<mesh_text> mesh;

		string text;
		// a texture for our text
		GLuint font_texture;
		HUDText(aabb *bounds = 0)
		{
			initialise(bounds);
		}

		~HUDText()
		{
		}


		void initialise(aabb *bounds = 0)
		{
			overlay = new text_overlay();
			font = overlay->get_default_font();
			text = "";

			mesh = new mesh_text(font, "initTestings", bounds);

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

