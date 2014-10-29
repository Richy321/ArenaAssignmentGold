#pragma once
namespace Arena
{
	class HUDText
	{
	public:
		
		octet::ref<octet::mesh_text> mesh;

		octet::string text;
		// a texture for our text
		GLuint font_texture;
		HUDText(octet::aabb *bounds, octet::bitmap_font *font)
		{
			initialise(bounds, font);
		}

		~HUDText()
		{
		}


		void initialise(octet::aabb *bounds, octet::bitmap_font *font)
		{
			text = "";
			mesh = new octet::mesh_text(font, "initTestings", bounds);
		}
		void draw()
		{
			mesh->clear();
			mesh->format(text);
			mesh->update();
		}
	};
}

