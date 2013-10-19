#include <Graphic/DynamicMeshInstance.hpp>


Graphic::DynamicMeshInstance::DynamicMeshInstance() : MeshInstance()
{
	AnimationIndex = (size_t)-1;
	FrameIndex = 0.0f;
	Ratescale = 0.0f;
}

void Graphic::DynamicMeshInstance::PlayAnimation(size_t AnimationIndex, float Ratescale)
{
    this->AnimationIndex = AnimationIndex;
    this->Ratescale = Ratescale / 60.0;
    FrameIndex = 0.0f;
}

void Graphic::DynamicMeshInstance::SetDynamicReference(const Graphic::SkeletalMesh_Ptr& Reference)
{
    DynamicReference = Reference;

    //--------------------------------------
    // Allocate transformed vertices buffers
    // Copy static values (UV)
    const std::vector<Graphic::EDMMesh> & MeshList = DynamicReference->GetMeshList();

    TransformedVertices.resize(MeshList.size());

    for (size_t i=0;i<MeshList.size();++i)
    {
        TransformedVertices[i].resize(MeshList[i].EDMVertexList.size());
        for (size_t j=0;j<TransformedVertices[i].size();++j)
            TransformedVertices[i][j].TexCoords = MeshList[i].EDMVertexList[j].UV;
    }
    //--------------------------------------

}
void Graphic::DynamicMeshInstance::Tick(float Dt)
{
    Graphic::MeshInstance::Tick(Dt);

	if (DynamicReference == nullptr )
		return;

    if (AnimationIndex != (size_t)-1)
    {
        //----------------------------------------------------------------------
        // Update Frame Index
        const Graphic::EKPPackage_Ptr& Package = DynamicReference->GetPackage();

        const Graphic::EKPAnimation& Anim = Package->GetAnimation(AnimationIndex);

        FrameIndex += Dt * Ratescale * (Anim.Ratescale / 60.0f);
        if (floorf(FrameIndex ) >= Anim.Length )
            FrameIndex = 0.0f;
        //----------------------------------------------------------------------
    }
	//----------------------------------------------------------------------
	// Update bones data
    DynamicReference->UpdateSkeleton(AnimationIndex, FrameIndex);
    for (size_t i=0; i< TransformedVertices.size(); ++i)
        DynamicReference->Update(i, TransformedVertices[i]);
	//----------------------------------------------------------------------
}

void Graphic::DynamicMeshInstance::Draw(Graphic::RenderPass RenderPass)
{
    DynamicReference->PrepareBuffers(TransformedVertices);
	DynamicReference->Draw(RenderPass);
}

Graphic::DynamicMeshInstance::~DynamicMeshInstance()
{}
