#include <Graphic/Mesh/SkeletalMesh.hpp>
#include <Utility/RegularExpression.hpp>
#include <Utility/StringConversion.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include <map>

//============================================================================
// Skeletal Mesh
//
Graphic::SkeletalMesh::SkeletalMesh() : Mesh()
{

}

void Graphic::ComputeQuaternionW(glm::quat& Quat)
{
    float T = 1.0 - (Quat.x * Quat.x) - (Quat.y * Quat.y) - (Quat.z * Quat.z);

    if (T < 0.0)
        Quat.w = 0.0f;
    else
        Quat.w = -glm::sqrt(T);
}

bool Graphic::SkeletalMesh::LoadFromFile(const std::string& File)
{
	//--------------------------------------------------
	//	File Format
	//		EWDM | numJoints | numMeshes
	//		[ Joint List ]
	//			Name | ParentID | Location XYZ | Orientation XYZ
	//		[ Meshes List ]
	//			NumVerts
	//			NumWeights
	//			NumTris
	// 			[Vertex List ]
	//				UV XY | WeightStart | WeightCount
	//			[ Weight List ]
	//				JointIndex | Bias | Location XYZ
	//			[ Triangle List ]
	//				Vertex0 Vertex1 Vertex2
	//--------------------------------------------------

    std::ifstream Input(File.c_str(),std::ios::binary);

    if (!Input.is_open())
    {
        std::cout<<"Invalid header "<<std::endl;
        return false;
    }

	int32_t Identifier;

	Input.read(reinterpret_cast<char*>(&Identifier),4);

	if (Identifier != 0x4D445745)
	{
	    // Invalid header
	    // todo : exception
	    std::cout<<"Invalid header "<<std::endl;
	    return false;
	}

	uint32_t NumJoints = 0;
	uint32_t NumMeshes = 0;

	Input.read(reinterpret_cast<char*>(&NumJoints),4);
	Input.read(reinterpret_cast<char*>(&NumMeshes),4);

	BindPose.JointList.resize(NumJoints);

	MeshList.resize(NumMeshes);

	for (auto& Joint : BindPose.JointList)
	{
	    uint32_t NameLength = 0;

	    Input.read(reinterpret_cast<char*>(&NameLength),4);
	    Joint.Name.resize(NameLength);
	    Input.read(&Joint.Name[0], NameLength);

	    Input.read(reinterpret_cast<char*>(&Joint.ParentIndex),4);

		Input.read(reinterpret_cast<char*>(&Joint.Location.x),4);
		Input.read(reinterpret_cast<char*>(&Joint.Location.y),4);
		Input.read(reinterpret_cast<char*>(&Joint.Location.z),4);

		Input.read(reinterpret_cast<char*>(&Joint.Orientation.x),4);
		Input.read(reinterpret_cast<char*>(&Joint.Orientation.y),4);
		Input.read(reinterpret_cast<char*>(&Joint.Orientation.z),4);

        Graphic::ComputeQuaternionW(Joint.Orientation);

	}

	Skeleton = BindPose;

	for (auto& CurrentMesh: MeshList)
	{
	    //-----------------
	    // Allocate buffers
	    glGenBuffers(2, CurrentMesh.Buffers);
	    //-----------------
		uint32_t NumVerts = 0;
		uint32_t NumWeights = 0;
		uint32_t NumTris = 0;

		Input.read(reinterpret_cast<char*>(&NumVerts),4);
		Input.read(reinterpret_cast<char*>(&NumWeights),4);
		Input.read(reinterpret_cast<char*>(&NumTris),4);

		CurrentMesh.EDMVertexList.resize(NumVerts);
		CurrentMesh.EDMWeightList.resize(NumWeights);

		CurrentMesh.TriangleList.resize(NumTris);

		for (auto& Vertex: CurrentMesh.EDMVertexList)
		{
			Input.read(reinterpret_cast<char*>(&Vertex.UV.x),4);
			Input.read(reinterpret_cast<char*>(&Vertex.UV.y),4);
			Input.read(reinterpret_cast<char*>(&Vertex.WeightStart),4);
			Input.read(reinterpret_cast<char*>(&Vertex.WeightCount),4);

		}

		for (auto& Weight: CurrentMesh.EDMWeightList)
		{
			Input.read(reinterpret_cast<char*>(&Weight.JointIndex),4);
			Input.read(reinterpret_cast<char*>(&Weight.Bias),4);
			Input.read(reinterpret_cast<char*>(&Weight.Location.x),4);
			Input.read(reinterpret_cast<char*>(&Weight.Location.y),4);
			Input.read(reinterpret_cast<char*>(&Weight.Location.z),4);
		}

		for (auto& Tri: CurrentMesh.TriangleList)
		{
			Input.read(reinterpret_cast<char*>(&Tri.Indices[2]),4);
			Input.read(reinterpret_cast<char*>(&Tri.Indices[1]),4);
			Input.read(reinterpret_cast<char*>(&Tri.Indices[0]),4);


		}

        ComputeLocalSpaceNormal(BindPose, CurrentMesh);

		//-----------------------------
		// Prepare buffers
		glBindBuffer(GL_ARRAY_BUFFER, CurrentMesh.Buffers[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CurrentMesh.Buffers[1]);

		glBufferData(GL_ARRAY_BUFFER, NumVerts * sizeof(Graphic::Vertex),NULL, GL_DYNAMIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumTris * sizeof(Graphic::Triangle), CurrentMesh.TriangleList.data(), GL_STATIC_DRAW);

		//-----------------------------
	}

    return true;
}

void Graphic::SkeletalMesh::Update(size_t MeshIndex, std::vector<Graphic::Vertex>& OutVertices)
{
    //------------------------------
    // Compute vertices datas
    const EDMMesh& CurrentMesh = MeshList[ MeshIndex ];

    for (size_t i=0;i<CurrentMesh.EDMVertexList.size();++i)
    {
        glm::vec3 FinalLocation (0.0);
        glm::vec3 Normal (0.0);

        const Graphic::EDMVertex& Vertex = CurrentMesh.EDMVertexList[i];

        for (int j=0;j<Vertex.WeightCount;++j)
        {
           const Graphic::EDMWeight& Weight = CurrentMesh.EDMWeightList[ Vertex.WeightStart + j ];
           const Graphic::EDMJoint& Joint = Skeleton.JointList [ Weight.JointIndex ];

           glm::vec3 RotPos = Joint.Orientation * Weight.Location;

           Normal  += Joint.Orientation * Weight.Normal;

           FinalLocation += (Joint.Location + RotPos) * Weight.Bias;

        }

        OutVertices[ i ].Location =  FinalLocation;
        OutVertices[ i ].Normal = Normal;
    }
    //------------------------------
}

void Graphic::SkeletalMesh::UpdateSkeleton(size_t AnimationIndex, float FrameIndex)
{
    if (Package == nullptr  || AnimationIndex == (size_t)-1)
    {
        Skeleton = BindPose;
        return;
    }
    const EKPAnimation& CurrentAnimation = Package->GetAnimation(AnimationIndex);

    int CurrentFrameIndex = static_cast<int> ( floorf(FrameIndex) );

    int NextFrameIndex = (CurrentFrameIndex + 1 ) % CurrentAnimation.Length;

    float Delta = FrameIndex - static_cast<float>(CurrentFrameIndex);


    for (int i=0;i<Package->GetJointsCount();++i)
    {
        EDMJoint&Joint = Skeleton.JointList[i];
        Joint.Location = glm::vec3(0);
        Joint.Orientation = glm::quat(0.f,0.0f,0.0f,0.0f);

        Graphic::EKPKeyframeData CurFrameData = GetJointFrame(CurrentFrameIndex, CurrentAnimation.AnimationData[i]);
        Graphic::EKPKeyframeData NxtFrameData = GetJointFrame(NextFrameIndex, CurrentAnimation.AnimationData[i]);

        Graphic::EKPKeyframeData FrameData;

        FrameData.Location = CurFrameData.Location + Delta * (NxtFrameData.Location - CurFrameData.Location);
        FrameData.Orientation = glm::slerp(CurFrameData.Orientation, NxtFrameData.Orientation, Delta);

        if (Joint.ParentIndex <0)
        {
            Joint.Location = FrameData.Location;
            Joint.Orientation = FrameData.Orientation;
        }
        else
        {
            EDMJoint& ParentJoint = Skeleton.JointList[Joint.ParentIndex];
            glm::vec3 rPos = ParentJoint.Orientation * FrameData.Location;

            Joint.Location = ParentJoint.Location + rPos;

            Joint.Orientation = ParentJoint.Orientation * FrameData.Orientation;
            glm::normalize(Joint.Orientation);

        }
    }
}

void Graphic::SkeletalMesh::PrepareBuffers(const std::vector< std::vector<Vertex>> & TransformedVertices)
{
    //------------------------------------------
    // make sure transformed datas are valid
    assert( TransformedVertices.size() == MeshList.size());
    //------------------------------------------

    for (size_t i = 0 ; i< TransformedVertices.size();++i)
    {
        const EDMMesh& CurrentMesh = MeshList[i];

        //-----------------------------
        // Bind buffer | update buffer
        glBindBuffer(GL_ARRAY_BUFFER, CurrentMesh.Buffers[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, TransformedVertices[i].size() * sizeof(Graphic::Vertex), TransformedVertices[i].data());
        //------------------------------

    }
}

void Graphic::SkeletalMesh::Draw(Graphic::RenderPass RenderPass)
{

	//--------------------------
	// Setup client state
	glEnableVertexAttribArray(GLuint( Graphic::MeshAttribute::Location) );
	if (RenderPass == Graphic::RenderPass::Final)
	{
        glEnableVertexAttribArray(GLuint( Graphic::MeshAttribute::Normal) );
        glEnableVertexAttribArray(GLuint( Graphic::MeshAttribute::TexCoords) );
	}
	//--------------------------


    for (const auto& CurrentMesh : MeshList)
    {
        //--------------------
        // Bind buffers
        glBindBuffer(GL_ARRAY_BUFFER, CurrentMesh.Buffers[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CurrentMesh.Buffers[1]);
        //--------------------

        //--------------------------
        // Setup attribut pointers
        glVertexAttribPointer(GLuint(Graphic::MeshAttribute::Location), 3, GL_FLOAT, GL_FALSE , sizeof(Graphic::Vertex), (void*)NULL);
        glVertexAttribPointer(GLuint(Graphic::MeshAttribute::Normal), 3, GL_FLOAT, GL_TRUE, sizeof(Graphic::Vertex), (void*)(sizeof(glm::vec3)));
        glVertexAttribPointer(GLuint(Graphic::MeshAttribute::TexCoords), 2, GL_FLOAT, GL_FALSE, sizeof(Graphic::Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec3) ));
        //--------------------------

        //--------------------------
        // Draw
        glDrawElements(GL_TRIANGLES, CurrentMesh.TriangleList.size() * 3  , GL_UNSIGNED_INT, NULL);
        //--------------------------
    }


    //--------------------------
    // Disable client state
    glDisableVertexAttribArray(GLint( Graphic::MeshAttribute::Location) );
	if (RenderPass == Graphic::RenderPass::Final)
	{
        glDisableVertexAttribArray(GLuint( Graphic::MeshAttribute::Normal) );
        glDisableVertexAttribArray(GLuint( Graphic::MeshAttribute::TexCoords) );
	}
    //--------------------------
}

void Graphic::SkeletalMesh::ComputeLocalSpaceNormal(const Graphic::EDMSkeleton& RefPose, Graphic::EDMMesh& OutMesh)
{

    std::vector<glm::vec3> BindPoseVertices;
    std::vector<glm::vec3> ObjectSpaceNormals;
    std::vector<glm::vec3> ObjectSpaceNormalsAvg;

    BindPoseVertices.resize(OutMesh.EDMVertexList.size());
    ObjectSpaceNormals.resize(OutMesh.EDMVertexList.size());
    ObjectSpaceNormalsAvg.resize(OutMesh.EDMVertexList.size());

    //------------------------------
    // Compute bindpose vertices position
    for (size_t i=0;i<OutMesh.EDMVertexList.size();++i)
    {
        const Graphic::EDMVertex& Vertex = OutMesh.EDMVertexList[i];

        for (int j=0;j<Vertex.WeightCount;++j)
        {
           const Graphic::EDMWeight& Weight = OutMesh.EDMWeightList[ Vertex.WeightStart + j ];
           const Graphic::EDMJoint& Joint = RefPose.JointList [ Weight.JointIndex ];

           glm::vec3 RotPos = Joint.Orientation *Weight.Location ;

           BindPoseVertices[ i ] += (Joint.Location + RotPos) * Weight.Bias;
        }
    }
    //------------------------------

    //------------------------------
    // Compute Object-space normal
    for (size_t i=0; i< OutMesh.TriangleList.size();++i)
    {
        const Triangle& T = OutMesh.TriangleList[i];

        const glm::vec3& V1 = BindPoseVertices [ T.Indices[0] ];
        const glm::vec3& V2 = BindPoseVertices [ T.Indices[1] ];
        const glm::vec3& V3 = BindPoseVertices [ T.Indices[2] ];

        glm::vec3 A = V2-V1;
        glm::vec3 B = V3-V1;
        glm::vec3 AxB = glm::normalize ( glm::cross(A,B) );

        ObjectSpaceNormals[T.Indices[0]] += AxB;
        ObjectSpaceNormals[T.Indices[1]] += AxB;
        ObjectSpaceNormals[T.Indices[2]] += AxB;
    }
    //------------------------------

    //------------------------------
    // Accumulate normals
    for (size_t i=0;i< OutMesh.EDMVertexList.size();++i)
    {
        const glm::vec3& A = BindPoseVertices[i];
        for (size_t j=0;j< OutMesh.EDMVertexList.size();++j)
        {
            const glm::vec3& B = BindPoseVertices[j];

            if (glm::length(A - B) <= 1e-4)
                ObjectSpaceNormalsAvg[i] += ObjectSpaceNormals[j];
        }
    }
    //------------------------------

    //------------------------------
    // Normalize values
    for (size_t i=0;i< OutMesh.EDMVertexList.size();++i)
        ObjectSpaceNormalsAvg[i] = glm::normalize(ObjectSpaceNormalsAvg[i]);
    //------------------------------

    //------------------------------
    // Compute weights
    for (size_t i=0;i<OutMesh.EDMVertexList.size();++i)
    {
        const Graphic::EDMVertex& Vertex = OutMesh.EDMVertexList[i];

        for (int j=0;j<Vertex.WeightCount;++j)
        {
           Graphic::EDMWeight& Weight = OutMesh.EDMWeightList[ Vertex.WeightStart + j ];
           const Graphic::EDMJoint& Joint = RefPose.JointList [ Weight.JointIndex ];

           Weight.Normal =  glm::inverse(Joint.Orientation) * ObjectSpaceNormalsAvg[i];

        }
    }
}

Graphic::SkeletalMesh::~SkeletalMesh()
{
    for (auto& CurrentMesh: MeshList)
    {
        //-----------------
        // Allocate buffers
        glGenBuffers(2, CurrentMesh.Buffers);
        //-----------------
    }
}
//
//============================================================================

//============================================================================
// Animation Package
//
Graphic::EKPPackage::EKPPackage()
{
    JointsCount = 0;
}

bool Graphic::EKPPackage::LoadFromFile(const std::string& File)
{
	std::ifstream Input(File.c_str(), std::ios::binary);

    if (!Input.is_open())
    {
        std::cout<<"Invalid header "<<std::endl;
        return false;
    }

	int32_t Identifier;

	Input.read(reinterpret_cast<char*>(&Identifier),4);

	if (Identifier != 0x504B5745)
	{
	    //-----------------
	    // Invalid header
	    // todo : exception
	    std::cout<<"Invalid header "<<std::endl;
	    return false;
	    //-----------------
	}

	uint32_t AnimationsCount = 0;

	Input.read(reinterpret_cast<char*>(&AnimationsCount),4);
	Input.read(reinterpret_cast<char*>(&JointsCount),4);

	AnimationList.resize(AnimationsCount);

	for (auto& Anim : AnimationList)
	{
		uint32_t NameLength;

        //--------------------------------------------------
        // Load animation header
		Input.read(reinterpret_cast<char*>(&NameLength),4);
		Anim.Name.resize(NameLength);
		Input.read(&Anim.Name[0], NameLength);
		Input.read(reinterpret_cast<char*>(&Anim.Length),4);
		Input.read(reinterpret_cast<char*>(&Anim.Ratescale),4);
        //--------------------------------------------------

        //--------------------------------------------------
        // Allocate animation components
		Anim.AnimationData.resize(JointsCount);
        //--------------------------------------------------

        //--------------------------------------------------
        // Load joints track
		for (auto& Track : Anim.AnimationData)
		{
			uint32_t FramesCount = 0;
			Input.read(reinterpret_cast<char*>(&FramesCount),4);

			Track.Keyframes.resize(FramesCount);
			Track.LinkerList.resize(Anim.Length);

			uint32_t LastFrameIndex = 0;

			for (auto& TrackFrame : Track.Keyframes)
			{

				Input.read(reinterpret_cast<char*>(&TrackFrame.FrameIndex),4);

                //-----------------
                // Fill FrameLinker
                std::fill(Track.LinkerList.begin() +LastFrameIndex, Track.LinkerList.begin() + TrackFrame.FrameIndex + 1, Graphic::EKPFrameLinker {TrackFrame.FrameIndex,0} );
                LastFrameIndex = TrackFrame.FrameIndex + 1;
                //-----------------

				Input.read(reinterpret_cast<char*>(&TrackFrame.Location.x),4);
				Input.read(reinterpret_cast<char*>(&TrackFrame.Location.y),4);
				Input.read(reinterpret_cast<char*>(&TrackFrame.Location.z),4);

				Input.read(reinterpret_cast<char*>(&TrackFrame.Orientation.x),4);
				Input.read(reinterpret_cast<char*>(&TrackFrame.Orientation.y),4);
				Input.read(reinterpret_cast<char*>(&TrackFrame.Orientation.z),4);

				Graphic::ComputeQuaternionW(TrackFrame.Orientation);
			}

		}
		//--------------------------------------------------
	}

	return true;
}

const Graphic::EKPAnimation& Graphic::EKPPackage::GetAnimation(size_t Index) const
{
    //------------------------------
    // todo : Prevent from out of range Index
    return AnimationList [ Index ];
    //------------------------------
}

Graphic::EKPPackage::~EKPPackage()
{
    JointsCount = 0;
}
//============================================================================

//============================================================================
// Helper functions
Graphic::EKPKeyframeData Graphic::GetJointFrame(size_t FrameIndex,const Graphic::EKPTrack & Track)
{
    const Graphic::EKPFrameLinker & Link = Track.LinkerList [ FrameIndex ];

    // Get source & dest values
    const Graphic::EKPKeyframeData& SrcKeyFrame = Track.Keyframes[Link.SourceKeyframeIndex];
    const Graphic::EKPKeyframeData& DstKeyFrame = Track.Keyframes[Link.DestKeyframeIndex];

    if (SrcKeyFrame.FrameIndex == FrameIndex)
        return SrcKeyFrame;

    float Delta = static_cast<float>(FrameIndex - Link.SourceKeyframeIndex) / static_cast<float> (Link.DestKeyframeIndex - Link.SourceKeyframeIndex);

    Graphic::EKPKeyframeData Interpolated;

    Interpolated.Location = SrcKeyFrame.Location + Delta * (DstKeyFrame.Location - SrcKeyFrame.Location);
    Interpolated.Orientation = glm::slerp(SrcKeyFrame.Orientation,DstKeyFrame.Orientation,Delta);

    return Interpolated;
}
//============================================================================
