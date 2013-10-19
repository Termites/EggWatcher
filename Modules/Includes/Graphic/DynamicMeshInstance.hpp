#ifndef _MODULE_GRAPHIC_DYNAMICMESHINSTANCE_HPP_
#define _MODULE_GRAPHIC_DYNAMICMESHINSTANCE_HPP_

#include <Graphic/MeshInstance.hpp>
#include <Graphic/Mesh/SkeletalMesh.hpp>

/*
	Class that represents a skeletal mesh on a scene.

	Inherits MeshInstance :
		 + AnimationIndex
		 + FrameIndex
*/

namespace Graphic
{
    class DynamicMeshInstance : public MeshInstance
    {
        private:
            size_t AnimationIndex;
            float FrameIndex;
            float Ratescale;
            SkeletalMesh_Ptr DynamicReference;

            std::vector < std::vector<Vertex> > TransformedVertices;

            //------------------------------------
            // Avoid call of theses functions
            void SetReference(const Mesh_Ptr& Ref);
            const Mesh_Ptr& GetReference() const;
            //------------------------------------
        public:
            DynamicMeshInstance();

            //--------------------------------------------
            // Reference mesh functions
            void SetDynamicReference(const SkeletalMesh_Ptr& Reference);

            const SkeletalMesh_Ptr& GetDynamicReference() const
            { return DynamicReference; }
            //--------------------------------------------

            void PlayAnimation(size_t Index, float Ratescale = 60.0f);

            void Tick(float Dt);

            void Draw(RenderPass);

            virtual ~DynamicMeshInstance();
    };
    typedef std::shared_ptr<DynamicMeshInstance> DynamicMeshInstance_Ptr;
}

#endif
