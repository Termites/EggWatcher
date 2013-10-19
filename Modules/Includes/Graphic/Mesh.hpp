#ifndef _MODULE_GRAPHIC_MESH_HPP_
#define _MODULE_GRAPHIC_MESH_HPP_

#include <memory>
#include <gl/glew.h>
#include <glm/glm.hpp>

/*
	Abstract class that represents any kind of Mesh.

	todo :
        Change buffer organisation :

            Current : [ VertLoc | VertNorm | VertUV ] [ VertLoc | VertNorm | VertUV ] ...
            New : [VertLoc VertLoc ...] [VertNorm VertNorm ...] [VertUV VertUV ...]

                -> Avoid uploading full buffer ?
                    -> glBindBufferRange, OpenGL 3.2+ !

        < Switch to openGL 3.2 ? >
*/
namespace Graphic
{

    //-----------------------
    // Rendering pass for meshes
    // - Avoid sending useless datas
    enum class RenderPass
    {
        Shadow, // Send only Location
        Outline, // Send both location & normal (required ?)
        Final // Send all attributes


    };
    //-----------------------

    //-----------------------
	// Vertex Attributes enumeration
	enum class MeshAttribute
	{
		Location = 0,
		Normal = 1,
		TexCoords = 2
	};
    //-----------------------

	struct Vertex
	{
		glm::vec3 Location;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	struct Triangle
	{
		GLuint Indices[3];
	};

	class Mesh
	{
		protected:

		public:
			Mesh() {}

			virtual void Draw(RenderPass RP) = 0;

			virtual void Tick(float Dt) = 0;

			virtual ~Mesh() {}
	};

	typedef std::shared_ptr<Mesh> Mesh_Ptr;

}

#endif
