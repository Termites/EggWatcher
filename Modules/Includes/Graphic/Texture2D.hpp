#ifndef _MODULE_GRAPHIC_TEXTURE2D_HPP_
#define _MODULE_GRAPHIC_TEXTURE2D_HPP_

#include <gl/glew.h>
#include <memory>
#include <string>

namespace Graphic
{
	class Texture2D
	{
		private:
			GLuint TextureID;
			GLsizei Width;
			GLsizei Height;

		public:

			Texture2D();
			bool LoadFromFile(const std::string& File);

			//------------------------
			// Getters
			GLuint GetTextureID() const
			{ return TextureID; }

			GLsizei GetWidth() const
			{ return Width; }

			GLsizei GetHeight() const
			{ return Height; }
			//------------------------

			~Texture2D();

	};
	typedef std::shared_ptr<Texture2D> Texture2D_Ptr;
}

#endif
