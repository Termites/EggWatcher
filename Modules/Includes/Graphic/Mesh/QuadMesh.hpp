#ifndef _MODULE_GRAPHIC_QUADMESH_HPP_
#define _MODULE_GRAPHIC_QUADMESH_HPP_

#include <Graphic/Mesh/StaticMesh.hpp>

namespace Graphic
{
	class QuadMesh : public StaticMesh
	{
		private:

		public:
			QuadMesh() : StaticMesh()
			{
				//---------------------------
				// Generate vertices
				Graphic::Vertex VertexList[ 4 ];
				VertexList[0].Location = glm::vec3(-1.0,-1.0,0);
				VertexList[0].TexCoords = glm::vec2(0,0);
				VertexList[0].Normal = glm::vec3(0,0,1);

				VertexList[1].Location = glm::vec3(1.0,-1.0,0);
				VertexList[1].TexCoords = glm::vec2(1,0);
				VertexList[1].Normal = glm::vec3(0,0,1);

				VertexList[2].Location = glm::vec3(1.0,1.0,0);
				VertexList[2].TexCoords = glm::vec2(1,1);
				VertexList[2].Normal = glm::vec3(0,0,1);

				VertexList[3].Location = glm::vec3(-1.0,1.0,0);
				VertexList[3].TexCoords = glm::vec2(0,1);
				VertexList[3].Normal = glm::vec3(0,0,1);
				//---------------------------

				//---------------------------
				// Generate Indices
				GLuint Indices [ 6 ];

				Indices[0] = 0;
				Indices[1] = 1;
				Indices[2] = 2;

				Indices[3] = 3;
				Indices[4] = 0;
				Indices[5] = 2;

				TrianglesCount = 2;
				//---------------------------

				//---------------------------
				// Fill Buffers
				glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
				glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Graphic::Vertex), VertexList, GL_STATIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[1]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), Indices, GL_STATIC_DRAW);
				//---------------------------
			}

			~QuadMesh()
			{}
	};
}

#endif
