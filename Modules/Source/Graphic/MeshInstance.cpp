#include <Graphic/MeshInstance.hpp>
#include <glm/gtc/matrix_transform.hpp>

Graphic::MeshInstance::MeshInstance()
{
    bUpdateWorldMatrix = false;
    WorldMatrix = glm::mat4(1.0);
    OverlaysCount = 0;

    OverlayOffsetSize[0] = glm::vec4(0,0,1,1);
    OverlayOffsetSize[1] = glm::vec4(0,0,1,1);
}

void Graphic::MeshInstance::BindTextures() const
{
    for (int i=0;i<3;++i)
    {
        if (Textures[i] != nullptr)
        {
            glActiveTexture(GL_TEXTURE1 + i );
            glBindTexture(GL_TEXTURE_2D, Textures[i]->GetTextureID());
        }
    }
}

void Graphic::MeshInstance::Draw(Graphic::RenderPass RenderPass)
{
 	if (ReferenceMesh != nullptr)
		ReferenceMesh->Draw(RenderPass);
}

void Graphic::MeshInstance::Tick(float Dt)
{
	if (bUpdateWorldMatrix)
	{
		WorldMatrix = glm::translate(glm::mat4(1.0), Location);

		WorldMatrix = glm::rotate(WorldMatrix, Orientation.x, glm::vec3(1,0,0));
		WorldMatrix = glm::rotate(WorldMatrix, Orientation.y, glm::vec3(0,1,0));
		WorldMatrix = glm::rotate(WorldMatrix, Orientation.z, glm::vec3(0,0,1));

		bUpdateWorldMatrix = false;
	}

	if (ReferenceMesh != nullptr)
		ReferenceMesh->Tick(Dt);
}
