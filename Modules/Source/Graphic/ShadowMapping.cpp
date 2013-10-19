#include <Graphic/ShadowMapping.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

Graphic::ShadowMapping::ShadowMapping()
{
	IsBound = false;
	Resolution = 4096;

	FBO.reset(new Graphic::FramebufferObject(Resolution, Resolution,0, FBOFlags(FBO_Depth | FBO_DepthTexture )));

	RenderingShader.LoadFromFile("ShadowMap.vs","ShadowMap.fs");
	ProjectionMatrix_Location = RenderingShader.GetUniformLocation("uProjectionMatrix");
	ModelViewMatrix_Location = RenderingShader.GetUniformLocation("uModelViewMatrix");

	SetLightOrientation(glm::vec3(0,0.0001,-1));

	SetProjectionSize(glm::vec2(128,128));
}


void Graphic::ShadowMapping::SetResolution(int Resolution)
{
	this->Resolution = Resolution;
	FBO.reset(new Graphic::FramebufferObject(Resolution, Resolution,1, FBOFlags(FBO_Depth | FBO_DepthTexture )));
}

void Graphic::ShadowMapping::SetLightOrientation(const glm::vec3& Orientation)
{
	ViewMatrix = glm::lookAt(Orientation,glm::vec3(0.0f), glm::vec3(0,0,1));

}

void Graphic::ShadowMapping::SetProjectionSize(const glm::vec2& Size)
{
	ProjectionMatrix = glm::ortho(-Size.x/2.0f, Size.x/2.0f, Size.y/2.0f, -Size.y/2.0f,-1000.01f,1000.0f);

	//-----------------
	// Send Projection matrix to Shader
	RenderingShader.Bind();
	glUniformMatrix4fv(ProjectionMatrix_Location, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	RenderingShader.Unbind();
	//----------------
}

void Graphic::ShadowMapping::SetObjectMatrix(const glm::mat4& ObjectMatrix)
{
	assert(IsBound);
	glm::mat4 ModelViewMatrix = ViewMatrix * ObjectMatrix;

	//-----------------
	// Send ModelView matrix to Shader
	glUniformMatrix4fv(ModelViewMatrix_Location, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	//----------------
}

void Graphic::ShadowMapping::Bind()
{
	IsBound = true;

	//-------------------------------
	// Store current viewport setup
	// Matchs viewport with resolution
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0,0,Resolution,Resolution);
    //-------------------------------

    //-------------------------------
    // Bind and clear FBO
    FBO->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
    //-------------------------------

	RenderingShader.Bind();

}

void Graphic::ShadowMapping::Unbind()
{
    IsBound = false;

    //----------------------
    // Restore old viewport
    glPopAttrib();
    //----------------------

	RenderingShader.Unbind();
	FBO->Unbind();
}

Graphic::ShadowMapping::~ShadowMapping()
{}
