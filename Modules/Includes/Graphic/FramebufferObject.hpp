#ifndef _MODULE_GRAPHIC_FRAMEBUFFER_HPP_
#define _MODULE_GRAPHIC_FRAMEBUFFER_HPP_

#include <gl/glew.h>
#include <vector>

namespace Graphic
{
    enum FBOFlags : uint8_t
    {
       FBO_Red = 0x01,
       FBO_Green = 0x02,
       FBO_Blue = 0x04,
       FBO_Alpha = 0x08,
       FBO_Depth = 0x10,

       FBO_DepthTexture = 0x20,
       FBO_Float = 0x40,

       // Shortcuts :
       FBO_RGB = 0x07,
       FBO_RGBA = 0x0F
    };

	class FramebufferObject
	{
		private:
			// Disable copy & default c-tor :
			FramebufferObject();
			FramebufferObject(const FramebufferObject& Other);
			FramebufferObject& operator = (const FramebufferObject& Other);

            std::vector<GLuint> ColorAttachments;

            GLuint DepthbufferID;
            GLuint DepthbufferTextureID;

            GLuint FramebufferID;

            GLsizei Width,Height;

		public:
			FramebufferObject(GLsizei W, GLsizei H, size_t AttachmentsCount,FBOFlags Flags);

            void Bind();
            void Unbind();

			GLuint GetTexture(size_t ID) const;

            GLsizei GetWidth() const
			{ return Width; }
            GLsizei GetHeight() const
			{ return Height; }


			~FramebufferObject();
	};
}

#endif
