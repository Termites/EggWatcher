#ifndef _MODULE_GRAPHIC_SKELETALMESH_HPP_
#define _MODULE_GRAPHIC_SKELETALMESH_HPP_

#include <Graphic/Mesh.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <memory>

namespace Graphic
{
	class EKPPackage;
	class SkeletalMesh;

	typedef std::shared_ptr<SkeletalMesh> SkeletalMesh_Ptr;
	typedef std::shared_ptr<EKPPackage> EKPPackage_Ptr;

    //=======================================================
    // EDM File
	// 		Based on MD5 File format
    struct EDMVertex
    {
        glm::vec2 UV;
        int WeightStart;
        int WeightCount;

    };

    struct EDMWeight
    {
        int JointIndex;
        float Bias;
        glm::vec3 Location;
        glm::vec3 Normal;
    };

    struct EDMJoint
    {
      std::string Name;
      int ParentIndex;

      glm::vec3 Location;
      glm::quat Orientation;
    };

    struct EDMMesh
    {
        std::vector<EDMVertex> EDMVertexList;
        std::vector<EDMWeight> EDMWeightList;

        std::vector<Triangle> TriangleList;


        GLuint Buffers [ 2 ];
    };

    struct EDMSkeleton
    {
        std::vector<EDMJoint> JointList;
    };

    class SkeletalMesh : public Mesh
    {
        private:
            std::vector<EDMMesh> MeshList;
            EDMSkeleton BindPose;

            EDMSkeleton Skeleton;

            EKPPackage_Ptr Package;

            void ComputeLocalSpaceNormal(const EDMSkeleton& RefPose, EDMMesh& OutMesh);

        public:
            SkeletalMesh();

            bool LoadFromFile(const std::string& File);

            void UpdateSkeleton(size_t AnimationIndex, float FrameIndex);
            //------------------------
            // Generate Vertex array
            // from EDM informations
            void Update(size_t Index, std::vector<Vertex>& OutVertices);

            const std::vector<EDMMesh>& GetMeshList() const
            { return MeshList; }
            //------------------------

			//------------------------
			// Package function
			void SetPackage(const EKPPackage_Ptr& Package)
			{ this->Package = Package; }

			const EKPPackage_Ptr& GetPackage() const
			{ return Package; }
			//------------------------

            void Tick(float Dt) {}

            void PrepareBuffers(const std::vector< std::vector<Vertex>> &TransformedVertices);
            void Draw(RenderPass);

            ~SkeletalMesh();
    };

    extern void ComputeQuaternionW(glm::quat& Quat);
	//=======================================================

	//=======================================================
	// EKP File

    //------------------
    // EKP::KeyFrameData
    // Data available
    // on each joint's keyframes

	struct EKPKeyframeData
	{
	    uint32_t FrameIndex;
		glm::vec3 Location;
		glm::quat Orientation;
	};
    //------------------
    //-----------------------
    // EKP::FrameLinker
    // - Keyframes are not contiguous
    // - Only the first keyframe and last one are always presents.
    //
    // <-------------------------> Animation
    //  []             []      [] Bone0 Keyframes
    //  []       []  []  [][][][] Bone1 Keyframes
    //
    // In order to prevent from using associative containers (std::map)
    // EggWatcher EKP files uses a FrameLinker :
    // For each animation frames, we have the matching source & destination indexes.
    //
    // An example with 5 frames :
    //
    // <  |    |    |    |   | >  | <- Animation track
    //   [0]            [1] [2]   | <- Bone0 Track
    //  <0,1><0,1><0,1><1,2><2,0> | <- Bone0 FrameLinker
    struct EKPFrameLinker
    {
        uint32_t SourceKeyframeIndex;
        uint32_t DestKeyframeIndex;
    };
	//-----------------------

    //-----------------------
    // EKP::Track
    //  Joint's keyframe list
    //  Joint's frame linker list
    struct EKPTrack
    {
        std::vector<EKPKeyframeData> Keyframes;
        std::vector<EKPFrameLinker> LinkerList;
    };
    //-----------------------

    //-----------------------
    // EKPAnimation
    // Name
    // FramesCount
    // Speed (60.0 = 1.0x)
    // Joints track
	struct EKPAnimation
	{
        std::string Name;
		uint32_t Length;
		float Ratescale;

		std::vector<EKPTrack> AnimationData;
	};
    //-----------------------

    //-----------------------
    // EKPPackage
    // Contains a set of animations
    // Also holds JointsCount in order
    // to make sure SkeletalMesh's skeleton matchs package's skeleton
    // (i.e no crash because of unmatching joints count)
	class EKPPackage
	{
		private:
			std::vector<EKPAnimation> AnimationList;
			int JointsCount;

		public:
			EKPPackage();

			bool LoadFromFile(const std::string&);

			int GetJointsCount() const
			{ return JointsCount; }

			const EKPAnimation& GetAnimation(size_t Index) const;
			const EKPAnimation& GetAnimation(const std::string&) const;

			~EKPPackage();
	};
	//-----------------------

	//-----------------------
	// Helper functions
	extern EKPKeyframeData GetJointFrame(size_t FrameIndex,const EKPTrack & Track);

	//=======================================================
}

#endif
