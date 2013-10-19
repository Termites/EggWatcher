#ifndef _MODULE_GRAPHIC_SHADOWMAPPING_HPP_
#define _MODULE_GRAPHIC_SHADOWMAPPING_HPP_

/*
	Class that controls the shadow map of a scene
*/
#include <glm/glm.hpp>

#include <Graphic/FramebufferObject.hpp>
#include <Graphic/Shader.hpp>

namespace Graphic
{
	class ShadowMapping
	{
		private:
			size_t Resolution;
			bool IsBound;

			//------------------------
			// Transformation
			glm::mat4 ProjectionMatrix;
			glm::mat4 ViewMatrix;
			//------------------------

			//------------------------
			// Shader
			std::unique_ptr<FramebufferObject> FBO;
			Shader RenderingShader;
			GLint ProjectionMatrix_Location;
			GLint ModelViewMatrix_Location;
			//------------------------

		public:
			ShadowMapping();

			void SetResolution(int Resolution);

			void SetLightOrientation(const glm::vec3& Orientation);
			void SetProjectionSize(const glm::vec2& size);

			const glm::mat4& GetProjectionMatrix() const
			{ return ProjectionMatrix; }

			const glm::mat4& GetViewMatrix() const
			{ return ViewMatrix; }

			GLuint GetTexture() const
			{ return FBO->GetTexture(-1); }

			void Bind();
			void SetObjectMatrix(const glm::mat4& ObjectMatrix);
			void Unbind();

			~ShadowMapping();
	};
}

#endif
