#ifndef _MODULE_GRAPHIC_SHADER_HPP_
#define _MODULE_GRAPHIC_SHADER_HPP_

#include <gl/glew.h>
#include <memory>
#include <string>
#include <map>

namespace Graphic
{
	class Shader
	{
		private:
			GLuint ProgramID;
			GLuint VertexShader;
			GLuint FragmentShader;

			// Associative maps :
			// <variable_name -> location>
			std::map<std::string, GLint> UniformLocations;

			//--------------------------------
			// Non-copyable
			Shader(const Shader&);
			Shader& operator= (const Shader&);
			//--------------------------------

		public:
			Shader();

			bool LoadFromFile(const std::string& Vert, const std::string& Frag);

            //--------------------------
            // Program-related
			void Bind() const
			{ glUseProgram(ProgramID); }
			void Unbind() const
			{  glUseProgram(0); }

			GLuint GetProgramID() const
			{ return ProgramID; }
            //--------------------------

			//------------------------------------------
			// Uniform setup functions
			GLint GetUniformLocation(const std::string&);

			void SetUniformIndex(const std::string&, GLint Index);
			void SetUniformIndex(GLint UnifIndex, GLint Index);
			//------------------------------------------


			void DeleteShader();

			~Shader();
	};

	typedef std::shared_ptr<Shader> Shader_Ptr;
}

#endif
