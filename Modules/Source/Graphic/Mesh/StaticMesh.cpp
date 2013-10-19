#include <Graphic/Mesh/StaticMesh.hpp>

#include <vector>
#include <map>

#include <fstream>
#include <iostream>
#include <sstream>

Graphic::StaticMesh::StaticMesh() : Mesh()
{
    //---------------------
	// Generate buffers
	glGenBuffers(2,Buffers);
	//---------------------

	TrianglesCount = 0;
}

bool Graphic::StaticMesh::LoadFromFile(const std::string& File)
{
	std::ifstream Input(File.c_str());

	if (!Input.is_open())
	{
		// todo : Exception
		std::cout<<"Error : Unable to open file "<<File<<std::endl;
		return false;
	}

	//-------------------------------------------
	// Helper to prevent from vertex duplications
	struct ObjFile_Indice
	{
		int LocID;
		int NrmID;
		int TxcID;

		bool operator<(const ObjFile_Indice & Other) const
		{ return LocID< Other.LocID ? true : NrmID < Other.NrmID ? true : TxcID < Other.TxcID ? true : false; }
	};
	//-----------------------------------------
	char Buffer[256];

	std::map<ObjFile_Indice, int> IDBinding;

	std::vector<glm::vec3> LocationList;
	std::vector<glm::vec3> NormalList;
	std::vector<glm::vec2 > TexCoordsList;

	//-------------------------------------------
	// Final mesh arrays
	std::vector< Graphic::Vertex > VertexList;
	std::vector< Graphic::Triangle > TriangleList;

	//===========================================
	// Load File
	// Get Vertices & triangles data, store on
	// VertexList & TriangleList arrays.
	while (Input.good())
	{
		Input.getline(Buffer,256);
		std::string CurrentLine = Buffer;

		if (CurrentLine.size()>0 && CurrentLine[0] != '#')
		{
		    std::string Head;
			std::stringstream SS;
			SS<<CurrentLine;
			SS>>Head;

			//--------------------------------------------
			// Header
			//--------------------------
			// v x y z : Vertex Location
			// vt u v w : Vertex TexCoords (no need to get w component)
			// vn x y z : Vertex Normal
			//--------------------------
			// f v0/t0/n0 v1/t1/n1 v2/t2/n2 : Triangle data (Vertex0_LocationID / Vertex0_TexCoordsID / Vertex0_NormalID ... )
			//--------------------------
			if (Head == "v")
			{
				glm::vec3 Location;
				SS>>Location.x>>Location.y>>Location.z;
				LocationList.push_back(Location);
			}
			else if (Head == "vn")
			{
				glm::vec3 Normal;
				SS>>Normal.x>>Normal.y>>Normal.z;
				NormalList.push_back(Normal);
			}
			else if (Head == "vt")
			{
				glm::vec2 TexCoords;
				SS>>TexCoords.x>>TexCoords.y;
				TexCoords.y = 1.0 - TexCoords.y;
				TexCoordsList.push_back(TexCoords);
			}
			else if (Head== "f")
			{
				// Create a new face (tris only)
				Graphic::Triangle T;

				for (auto& Indice : T.Indices)
				{
					//------------------------------------------
					// Extract current vertex's data
					// (Indices of Pos, Norm & UVs)
					// Make sure the vertex doesn't already exists
					// If it does, get it's ID, otherwise create a new one
					// and add it to the VertexList array.
					ObjFile_Indice OFI;
					char Sep;

					SS>>OFI.LocID>>Sep>>OFI.TxcID>>Sep>>OFI.NrmID;


					auto it = IDBinding.find(OFI);

					// Vertex exists
					if (it != IDBinding.end())
						Indice = it->second;
					else
					{
						// Create the new vertex
						IDBinding[OFI] = VertexList.size();
						Indice = VertexList.size();

						Graphic::Vertex V;
						// Obj format indices starts at 1.
						V.Location = LocationList[OFI.LocID - 1];
						V.Normal = NormalList[OFI.NrmID - 1];
						V.TexCoords = TexCoordsList[OFI.TxcID - 1];

						VertexList.push_back(V);
					}
				}
				//------------------------------------------

				TriangleList.push_back(T);
			}
		}
	}



	//===========================================

	//===========================================
	// Setup VBOs
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, VertexList.size() * sizeof(Graphic::Vertex), VertexList.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, TriangleList.size() * sizeof(Graphic::Triangle), TriangleList.data(), GL_STATIC_DRAW);
	//===========================================

	TrianglesCount = TriangleList.size();

	return true;
}

void Graphic::StaticMesh::Draw(Graphic::RenderPass RenderPass)
{
	//--------------------------
	// Bind buffers
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[1]);
	//--------------------------

	//--------------------------
	// Setup client state
	glEnableVertexAttribArray(GLuint( Graphic::MeshAttribute::Location) );
	if (RenderPass == Graphic::RenderPass::Final)
	{
        glEnableVertexAttribArray(GLuint( Graphic::MeshAttribute::Normal) );
        glEnableVertexAttribArray(GLuint( Graphic::MeshAttribute::TexCoords) );
	}
	//--------------------------

	//--------------------------
	// Setup attribut pointers
	glVertexAttribPointer(GLuint(Graphic::MeshAttribute::Location), 3, GL_FLOAT, GL_FALSE , sizeof(Graphic::Vertex), (void*)NULL);
	glVertexAttribPointer(GLuint(Graphic::MeshAttribute::Normal), 3, GL_FLOAT, GL_TRUE, sizeof(Graphic::Vertex), (void*)(sizeof(glm::vec3)));
	glVertexAttribPointer(GLuint(Graphic::MeshAttribute::TexCoords), 2, GL_FLOAT, GL_FALSE, sizeof(Graphic::Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec3) ));
	//--------------------------

	//--------------------------
	// Draw
	glDrawElements(GL_TRIANGLES, TrianglesCount * 3 , GL_UNSIGNED_INT, NULL);
	//--------------------------

	//--------------------------
	// Disable client state
	glDisableVertexAttribArray(GLint( Graphic::MeshAttribute::Location) );
	if (RenderPass == Graphic::RenderPass::Final)
	{
        glDisableVertexAttribArray(GLint( Graphic::MeshAttribute::Normal) );
        glDisableVertexAttribArray(GLint( Graphic::MeshAttribute::TexCoords) );
	}
	//--------------------------
}

Graphic::StaticMesh::~StaticMesh()
{
    //---------------------
    // Delete buffers
	glDeleteBuffers(2,Buffers);
	//---------------------
}
