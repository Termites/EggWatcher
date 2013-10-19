#ifndef _MODULE_GRAPHIC_MESHINSTANCE_HPP_
#define _MODULE_GRAPHIC_MESHINSTANCE_HPP_

#include <Graphic/Texture2D.hpp>
#include <Graphic/Mesh.hpp>

#include <glm/glm.hpp>

#include <vector>

/*
	Class that represents a mesh on a scene.

	Mesh : Mesh to draw (i.e reference)
	Holds the location and orientation

	Can be inherited to support other kind of meshes.

	Todo : Add textures support
*/

namespace Graphic
{
    //-------------------
    // Texture slots for texture
    enum class TextureSlot
    {
        Diffuse =0,
        Overlay0 =1,
        Overlay1 = 2
    };
    //-------------------

	class MeshInstance
	{
		protected:
			//---------------
			// Mesh
			Mesh_Ptr ReferenceMesh;
            //---------------

            //---------------
            // Textures
			Texture2D_Ptr Textures[3];
			int OverlaysCount;

			glm::vec4 OverlayOffsetSize[2];
            //---------------

			//---------------
			// World transformation
			glm::vec3 Location;
			glm::vec3 Orientation;

			glm::mat4 WorldMatrix;

			bool bUpdateWorldMatrix;
			//---------------



		public:
			MeshInstance();

            //--------------------------------------------
            // Texture functions
			//void AddTexture(const Texture2D_Ptr& NewTexture)
			//{ Textures.push_back(NewTexture); }

			void SetTexture (TextureSlot Slot,const Texture2D_Ptr& Texture)
			{
			    OverlaysCount = std::max<int> (OverlaysCount, int(Slot));
			    Textures[ int(Slot) ] = Texture;
			}

			int GetOverlaysCount() const
			{ return OverlaysCount; }

			const glm::vec4& GetOverlayOffsetSize(int Index) const
			{ return OverlayOffsetSize[Index]; }

			void SetOverlayOffsetSize(int Index, const glm::vec4& OffsetSize)
			{  OverlayOffsetSize[Index] = OffsetSize; }

			void BindTextures() const;
            //--------------------------------------------

            //--------------------------------------------
            // Transformation functions
			void SetLocation(const glm::vec3& Location)
			{
				this->Location = Location;
				bUpdateWorldMatrix = true;
			}

			void SetOrientation( const glm::vec3& Orientation)
			{
				this->Orientation = Orientation;
				bUpdateWorldMatrix = true;

			}

			const glm::mat4& GetWorldMatrix() const
			{ return WorldMatrix; }
			//--------------------------------------------

            //--------------------------------------------
            // Reference mesh functions
			void SetReferenceMesh(const Mesh_Ptr& NewMesh)
			{ this->ReferenceMesh = NewMesh; }

			const Mesh_Ptr& GetReferenceMesh()
			{ return ReferenceMesh; }
            //--------------------------------------------



			virtual void Tick(float Dt);

			virtual void Draw(RenderPass);

			virtual ~MeshInstance() {}
	};
	typedef std::shared_ptr<MeshInstance> MeshInstance_Ptr;
}

#endif
