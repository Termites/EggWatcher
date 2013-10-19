#include <Graphic/Scene.hpp>

Graphic::Scene::Scene()
{
    //========================
    // Setup Shaders

    //-------------------
    // Load Model Shader
    ModelShader.ModelShader.LoadFromFile("ModelShader.vs","ModelShader.fs");
    //-------------------

    //--------------------
    // Setup attributes
    glBindAttribLocation(ModelShader.ModelShader.GetProgramID(), GLuint(Graphic::MeshAttribute::Location), "aLocation");
    glBindAttribLocation(ModelShader.ModelShader.GetProgramID(), GLuint(Graphic::MeshAttribute::Normal), "aNormal");
    glBindAttribLocation(ModelShader.ModelShader.GetProgramID(), GLuint(Graphic::MeshAttribute::TexCoords), "aTexCoords");
    //---------------------

    //---------------------
    // Scene Uniforms
    ModelShader.ModelViewMatrix_Location = ModelShader.ModelShader.GetUniformLocation("uModelViewMatrix");
    ModelShader.ProjectionMatrix_Location  = ModelShader.ModelShader.GetUniformLocation("uProjectionMatrix");
    ModelShader.ShadowMatrix_Location = ModelShader.ModelShader.GetUniformLocation("uShadowMatrix");

	ModelShader.LightDirection_Location = ModelShader.ModelShader.GetUniformLocation("uLightDirection");
    ModelShader.LightColor_Location = ModelShader.ModelShader.GetUniformLocation("uLightColor");

	ModelShader.OverlayCount_Location = ModelShader.ModelShader.GetUniformLocation("uOverlayTexturesCount");
    ModelShader.OverlayOffset0_Location = ModelShader.ModelShader.GetUniformLocation("uOverlayTextureOffset0");
    ModelShader.OverlayOffset1_Location = ModelShader.ModelShader.GetUniformLocation("uOverlayTextureOffset1");

    ModelShader.ModelShader.Bind();
    glUniform1i(ModelShader.ModelShader.GetUniformLocation("uShadowTexture"),0);
    glUniform1i(ModelShader.ModelShader.GetUniformLocation("uDiffuseTexture"),1);
    glUniform1i(ModelShader.ModelShader.GetUniformLocation("uOverlayTextureSample0"),2);
    glUniform1i(ModelShader.ModelShader.GetUniformLocation("uOverlayTextureSample1"),3);
    ModelShader.ModelShader.Unbind();
    //---------------------

    //-------------------
    // Load Outline Shader
    OutlineShader.OutlineShader.LoadFromFile("OutlineShader.vs","OutlineShader.fs");
    //-------------------

    //--------------------
    // Setup attributes
    glBindAttribLocation(OutlineShader.OutlineShader.GetProgramID(), GLuint(Graphic::MeshAttribute::Location), "aLocation");
    glBindAttribLocation(OutlineShader.OutlineShader.GetProgramID(), GLuint(Graphic::MeshAttribute::Normal), "aNormal");
    glBindAttribLocation(OutlineShader.OutlineShader.GetProgramID(), GLuint(Graphic::MeshAttribute::TexCoords), "aTexCoords");
    //---------------------

    //---------------------
    // Scene Uniforms
    OutlineShader.ModelViewMatrix_Location = OutlineShader.OutlineShader.GetUniformLocation("uModelViewMatrix");
    OutlineShader.ProjectionMatrix_Location  = OutlineShader.OutlineShader.GetUniformLocation("uProjectionMatrix");
    //---------------------

    //---------------------
    // FramebufferObject
    MainBuffer.reset(new Graphic::FramebufferObject(800,600,2,FBOFlags( FBO_RGBA | FBO_Depth | FBO_DepthTexture) ));
    FinalBuffer.reset(new Graphic::FramebufferObject(800,600,1,FBO_RGB));
    //---------------------

    //-------------------
    // Load Line tracing shader | Line blending shader
    LineTracer.TracerShader.LoadFromFile("QuadShader.vs","LineTracer.fs");
    LineTracer.BlenderShader.LoadFromFile("QuadShader.vs","LineBlender.fs");
    //-------------------

    //---------------------
    // FramebufferObject
    LineBuffer.reset(new Graphic::FramebufferObject(800,600,1,FBOFlags(FBO_Red) ));
    //---------------------

    //--------------------
    // Setup attributes
    glBindAttribLocation(LineTracer.TracerShader.GetProgramID(), GLuint(Graphic::MeshAttribute::Location), "aLocation");
    glBindAttribLocation(LineTracer.TracerShader.GetProgramID(), GLuint(Graphic::MeshAttribute::Normal), "aNormal");
    glBindAttribLocation(LineTracer.TracerShader.GetProgramID(), GLuint(Graphic::MeshAttribute::TexCoords), "aTexCoords");

    LineTracer.uTexelSize_Location = LineTracer.TracerShader.GetUniformLocation("uTexelSize");

    LineTracer.TracerShader.Bind();
    glm::vec2 TexelSize = glm::vec2(1.0 / 800.0, 1.0 / 600.0);
    glUniform2f(LineTracer.uTexelSize_Location, TexelSize.x, TexelSize.y);
    glUniform1i(LineTracer.TracerShader.GetUniformLocation("uDepthBuffer"), 0);
    glUniform1i(LineTracer.TracerShader.GetUniformLocation("uNormalBuffer"), 1);
    LineTracer.TracerShader.Unbind();


    LineTracer.BlenderShader.Bind();
    glUniform1i(LineTracer.BlenderShader.GetUniformLocation("uColorBuffer"), 0);
    glUniform1i(LineTracer.BlenderShader.GetUniformLocation("uLineBuffer"), 1);
    LineTracer.BlenderShader.Unbind();
    //---------------------

    //---------------
    // FXAA Shader
    FXAAShader.LoadFromFile("QuadShader.vs","FXAA.fs");
    FXAA_TexelSize_Location = FXAAShader.GetUniformLocation("uTexelSize");

    FXAAShader.Bind();
    glUniform2f(FXAA_TexelSize_Location, TexelSize.x, TexelSize.y);
    FXAAShader.Unbind();
    //---------------
    //========================

	LightDirection = glm::vec3(0,0,1);
	LightColor = glm::vec3(1,1,1);

    PostProcessMesh.reset(new Graphic::QuadMesh);
}

void Graphic::Scene::Resize(int Width, int Height)
{
    if (Width == 0)
        Width = 1;

    if (Height == 0)
        Height = 1;

    glViewport(0,0,Width,Height);
    float Aspect = static_cast<float>(Width) / static_cast<float>(Height);
    UpdatePerspective(60.0,Aspect);

    //---------------------
    // FramebufferObject
    MainBuffer.reset(new Graphic::FramebufferObject(Width,Height,2,FBOFlags( FBO_RGBA | FBO_Depth | FBO_DepthTexture) ));
    LineBuffer.reset(new Graphic::FramebufferObject(Width,Height,1,FBOFlags(FBO_Red) ));
    FinalBuffer.reset(new Graphic::FramebufferObject(Width,Height,1,FBO_RGB));
    //---------------------

    LineTracer.TracerShader.Bind();
    glm::vec2 TexelSize = glm::vec2(1.0 / Width, 1.0 / Height);
    glUniform2f(LineTracer.uTexelSize_Location, TexelSize.x, TexelSize.y);
    LineTracer.TracerShader.Unbind();


    FXAAShader.Bind();
    glUniform2f(FXAA_TexelSize_Location, TexelSize.x, TexelSize.y);
    FXAAShader.Unbind();

}

void Graphic::Scene::UpdatePerspective(float FOV, float Aspect)
{
    ProjectionMatrix = glm::perspective(FOV, Aspect, 0.1f, 1000.0f);

    //--------------------------------------
    // Update ModelShader's projection matrix
    ModelShader.ModelShader.Bind();
    glUniformMatrix4fv(ModelShader.ProjectionMatrix_Location, 1, GL_FALSE, &ProjectionMatrix[0][0]);
    ModelShader.ModelShader.Unbind();
    //--------------------------------------

    //--------------------------------------
    // Update OutlineShader's projection matrix
    OutlineShader.OutlineShader.Bind();
    glUniformMatrix4fv(OutlineShader.ProjectionMatrix_Location, 1, GL_FALSE, &ProjectionMatrix[0][0]);
    OutlineShader.OutlineShader.Unbind();
    //--------------------------------------
}


void Graphic::Scene::Tick(float Dt)
{
    for (auto& I : MeshInstances)
    { I->Tick(Dt); }
}

void Graphic::Scene::AddMeshInstance(const MeshInstance_Ptr& NewMesh)
{
    MeshInstances.push_back(NewMesh);
}

void Graphic::Scene::ClearMainBuffer()
{

    MainBuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    MainBuffer->Unbind();
}

void Graphic::Scene::DrawShadows()
{
    //-------------------------------------------
    // Shadow Map Pass
    // Color mask : Only alpha (in order to add alpha-tested shadow support)
    glCullFace(GL_FRONT);

    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_TRUE);
    ShadowController.Bind();

    for (auto& mInst : MeshInstances)
    {
        ShadowController.SetObjectMatrix(mInst->GetWorldMatrix());
        mInst->Draw(Graphic::RenderPass::Final);
    }
    ShadowController.Unbind();
    glCullFace(GL_BACK);

    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    //-------------------------------------------
}


void Graphic::Scene::DrawOutline()
{
    //-------------------------------------------
    // Outline Pass
    glFrontFace(GL_CW);

    glEnable(GL_POLYGON_OFFSET_FILL);

    glPolygonOffset(-2.5,-2.5);

    MainBuffer->Bind();

    OutlineShader.OutlineShader.Bind();

    const glm::mat4& CameraMatrix = SceneCamera.GetMatrix();

    for (auto& mInst : MeshInstances)
    {
        mInst->BindTextures();
        glm::mat4 ModelViewMatrix = CameraMatrix * mInst->GetWorldMatrix();

        //----------------------
        // Setup uniforms
        glUniformMatrix4fv(OutlineShader.ModelViewMatrix_Location, 1, GL_FALSE, &ModelViewMatrix[0][0]);
        //----------------------
        mInst->Draw(Graphic::RenderPass::Final);
    }

    OutlineShader.OutlineShader.Unbind();

    MainBuffer->Unbind();

    glDisable(GL_POLYGON_OFFSET_FILL);
    glFrontFace(GL_CCW);
    //-------------------------------------------
}


void Graphic::Scene::DrawMeshes()
{
    //-------------------------------------------
    // Model Pass
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ShadowController.GetTexture());

    MainBuffer->Bind();

    ModelShader.ModelShader.Bind();

    const glm::mat4& CameraMatrix = SceneCamera.GetMatrix();

	glm::vec4 TransformedLightDirection = (CameraMatrix * glm::vec4(LightDirection,0.0));

	glUniform3f(ModelShader.LightDirection_Location, TransformedLightDirection.x,TransformedLightDirection.y, TransformedLightDirection.z);

    glUniform3f(ModelShader.LightColor_Location, LightColor.x, LightColor.y, LightColor.z);

	const glm::mat4 BiasMatrix(
	0.5,0.0,0.0,0.0,
	0.0,0.5,0.0,0.0,
	0.0,0.0,0.5,0.0,
	0.5,0.5,0.5,1.0
	);

    for (auto& mInst : MeshInstances)
    {
        int OverlaysCount = mInst->GetOverlaysCount();

        glUniform1i(ModelShader.OverlayCount_Location, OverlaysCount);

        if (OverlaysCount > 0)
        {
            const glm::vec4& OverlayOffsetSize = mInst->GetOverlayOffsetSize(0);
            glUniform4f(ModelShader.OverlayOffset0_Location,OverlayOffsetSize.x,OverlayOffsetSize.y,OverlayOffsetSize.z,OverlayOffsetSize.w);
        }

        if (OverlaysCount > 1)
        {
            const glm::vec4& OverlayOffsetSize = mInst->GetOverlayOffsetSize(1);
            glUniform4f(ModelShader.OverlayOffset1_Location,OverlayOffsetSize.x,OverlayOffsetSize.y,OverlayOffsetSize.z,OverlayOffsetSize.w);
        }

        mInst->BindTextures();
        glm::mat4 ModelViewMatrix = CameraMatrix * mInst->GetWorldMatrix();

        glm::mat4 ShadowMatrix;
        ShadowMatrix = BiasMatrix * ShadowController.GetProjectionMatrix() * ShadowController.GetViewMatrix() * mInst->GetWorldMatrix() ;

        //----------------------
        // Setup uniforms
        glUniformMatrix4fv(ModelShader.ModelViewMatrix_Location, 1, GL_FALSE, &ModelViewMatrix[0][0]);
        glUniformMatrix4fv(ModelShader.ShadowMatrix_Location, 1, GL_FALSE, &ShadowMatrix[0][0]);
        //----------------------
        mInst->Draw(Graphic::RenderPass::Final);
    }

    ModelShader.ModelShader.Unbind();

    MainBuffer->Unbind();
    //-------------------------------------------
}

void Graphic::Scene::DrawPostProcessLine()
{
    //----------------------
    // Trace lines [ Tabula-Rasa edge-detect filter ]
    LineBuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    LineTracer.TracerShader.Bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, MainBuffer->GetTexture((size_t)-1));

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, MainBuffer->GetTexture(1));

    PostProcessMesh->Draw(Graphic::RenderPass::Final);

    LineTracer.TracerShader.Unbind();
    LineBuffer->Unbind();
    //----------------------

    //----------------------
    // Blend lines
    FinalBuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    LineTracer.BlenderShader.Bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, MainBuffer->GetTexture(0));

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, LineBuffer->GetTexture(0));

    PostProcessMesh->Draw(Graphic::RenderPass::Final);

    LineTracer.BlenderShader.Unbind();
    FinalBuffer->Unbind();
    //----------------------
}

void Graphic::Scene::DrawPostProcessFXAA()
{
    //----------------------
    // FXAA Shader
    // Todo : Check pipeline
    // Bind FinalBuffer texture if Line is process
    // Otherwise, bind MainBuffer

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, FinalBuffer->GetTexture(0));

    FXAAShader.Bind();
    PostProcessMesh->Draw(Graphic::RenderPass::Final);
    FXAAShader.Unbind();
    //----------------------
}

void Graphic::Scene::Draw()
{
    ClearMainBuffer();

    SceneCamera.UpdateMatrix();

    DrawShadows();
    //DrawOutline();
    DrawMeshes();
    DrawPostProcessLine();
    DrawPostProcessFXAA();

}

Graphic::Scene::~Scene()
{}
