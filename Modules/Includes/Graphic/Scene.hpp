#ifndef _MODULE_GRAPHIC_SCENE_HPP_
#define _MODULE_GRAPHIC_SCENE_HPP_

#include <memory>
#include <list>

#include <Graphic/MeshInstance.hpp>
#include <Graphic/Shader.hpp>
#include <Graphic/Camera.hpp>
#include <Graphic/FramebufferObject.hpp>
#include <Graphic/Mesh/QuadMesh.hpp>
#include <Graphic/ShadowMapping.hpp>


namespace Graphic
{
    enum class SceneUniformIndex
    {
        ModelViewMatrix,
        ProjectionMatrix
    };

	class Scene
	{
		private:
            //----------------------------------------
            // Scene
            Camera SceneCamera;

            std::list<MeshInstance_Ptr> MeshInstances;
            glm::mat4 ProjectionMatrix;

            GLint FXAA_TexelSize_Location;

            ShadowMapping ShadowController;
            glm::vec3 LightDirection;
            glm::vec3 LightColor;
            //----------------------------------------

            //----------------------------------------
            // Shaders

            Shader FXAAShader;

            std::unique_ptr<FramebufferObject> MainBuffer;
            std::unique_ptr<FramebufferObject> LineBuffer;
            std::unique_ptr<FramebufferObject> FinalBuffer;

            struct ModelShaderData
            {
                Shader ModelShader;
                GLint ModelViewMatrix_Location;
                GLint ProjectionMatrix_Location;
                GLint ShadowMatrix_Location;
                GLint LightDirection_Location;
                GLint LightColor_Location;
                GLint OverlayCount_Location;

                GLint OverlayOffset0_Location;
                GLint OverlayOffset1_Location;

            } ModelShader;

            struct OutlineShaderData
            {
                Shader OutlineShader;
                GLint ModelViewMatrix_Location;
                GLint ProjectionMatrix_Location;
            } OutlineShader;

            struct LineTracerDatas
            {
                Shader TracerShader;
                Shader BlenderShader;
                GLint uTexelSize_Location;

            } LineTracer;
            //----------------------------------------

            std::unique_ptr<QuadMesh> PostProcessMesh;

            //--------------------------
            // Drawing functions
            // Each one match a pass
            void DrawShadows(); //RenderPass::Shadow
            void DrawOutline(); //RenderPass:Outline
            void DrawMeshes(); // RenderPass::Final

            void DrawPostProcessLine();
            void DrawPostProcessFXAA();

            void ClearMainBuffer();
            //--------------------------

		public:
			Scene();

			void Tick(float Dt);

			void AddMeshInstance(const MeshInstance_Ptr& NewMesh);

			void UpdatePerspective(float FOV, float Aspect);
			void Resize(int W, int H);

			void Draw();

			//-------------------
			// Getters / Setters
			void SetCamera(const Camera& NewCamera)
			{  SceneCamera = NewCamera; }

			const Camera& GetCamera() const
			{ return SceneCamera; }

			void SetLightDirection(const glm::vec3& Direction)
			{
			    LightDirection = glm::normalize(Direction);
			    ShadowController.SetLightOrientation(LightDirection);
            }

            void SetLightColor(const glm::vec3& Color)
            {
                LightColor = Color;
            }
            //-------------------

			~Scene();
	};
	typedef std::shared_ptr<Scene> Scene_Ptr;
}

#endif
