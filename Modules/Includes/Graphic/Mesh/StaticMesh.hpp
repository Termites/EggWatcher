#ifndef _MODULE_GRAPHIC_STATICMESH_HPP_
#define _MODULE_GRAPHIC_STATICMESH_HPP_

#include <Graphic/Mesh.hpp>

namespace Graphic
{
	class StaticMesh : public Mesh
	{
	    protected:
            size_t TrianglesCount;
			GLuint Buffers[2];
		public:
			StaticMesh();

			bool LoadFromFile(const std::string& File);

			void Draw(RenderPass);

			void Tick(float Dt) {}

			virtual ~StaticMesh();
	};

	typedef std::shared_ptr<StaticMesh> StaticMesh_Ptr;

}

#endif
