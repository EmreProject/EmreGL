#ifndef ogl_renderer_h
#define ogl_renderer_h



#include <GL/glew.h>
#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Projection.h"
#include "ogl_Camera.h"
#include "ogl_lightning_v2.h"
#include "ogl_model_v2.h"
#include "ogl_shader_v2.h"
#include "ogl_texture.h"
#include "ogl_World.h"
#include "ogl_default_meshes.h"
#include "oglScene.h"
#include "oglShadowMap.h"
#include "oglRenderTarget.h"
#include <type_traits>
#include <variant>
#include "oglMouseEvents.h"

using namespace std;

namespace OglPostProcessing {

	class WeightedBlendedRenderPass;
	class RenderPass;
	class EffectComposer;
}

namespace OglRenderer {

	

	class Renderer {

	private:

		
		
		
		
		void RenderDirectionalLightShadowMap(OglScene::Scene& scene);
		void RenderSpotLightShadowMaps(OglScene::Scene& scene);
		void RenderPointLightShadowMaps(OglScene::Scene& scene);


		void ApplyWorldMatrixAssimp(OglScene::Scene& scene);
		void ApplyReverseWorldMatrixAssimp(OglScene::Scene& scene);
		
		void meshStandardMaterialSpecific(OglScene::Scene* scene, OglObject3D::Mesh*,MyVectorV3::Vector3f);
		void RenderMeshes(OglScene::Scene& scene,MyVectorV3::Vector3f camWorldPos,MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix);
		void RenderSingleMesh(OglObject3D::Mesh* mesh, OglScene::Scene& scene, MyVectorV3::Vector3f camWorldPos, MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix);
	
		//Render Target
		variant<OglRenderTarget::RenderTarget*, GLuint> currentRenderTarget=(GLuint)0;
		void BindCurrentRenderTarget();
		function<void()> ClearCurrentRenderTarget = []() {
			glViewport(0, 0, OglWindow::Window::width, OglWindow::Window::height);
			glClearColor(0, 0, 0, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			};
		bool isDefaultRenderTarget = true;


		
		void enableIfDirLightShadowMaps(OglScene::Scene& scene);
		void enableIfSpotLightShadowMaps(OglScene::Scene& scene);
		void enableIfPointLightShadowMaps(OglScene::Scene& scene);
		void clearShadowMaps(OglScene::Scene& scene);

		OglMouseEvents::MouseEventHandler mouseEventHandler;
		void RenderMouseEvent(OglScene::Scene& scene, MyCamera::BasicCamera& cam);//bu render k�sm�nday�m...
		void RenderMouseEvent(OglScene::Scene& scene, MyCamera::OrbitalCamera& cam);
		void RenderMouseEvent(OglScene::Scene& scene, MyCamera::Camera& cam); //ortho or perspective camera
		void RenderMouseEventMeshes(OglScene::Scene& scene, MyVectorV3::Vector3f camWorldPos, MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix);
		void RenderMouseEventSingleMesh(OglObject3D::Mesh* mesh, OglScene::Scene& scene, MyVectorV3::Vector3f camWorldPos, MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix);


		void UpdateMeshMaterialIfEnvironmentMap(OglScene::Scene& scene,MyVectorV3::Vector3f camWorldPos);
		bool isUpdateMeshMaterialIfEnvironmentMap = false;
		void RenderEnvironmentMap(OglScene::Scene& scene, MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix);

		void ResetDebugLog();

	public:


		Renderer();
		~Renderer();

		bool shadowEnabled = false;
		bool weightTransparencyEnabled = false;
		bool mouseEventRenderingEnabled = true;
		bool environmentMapRenderingEnabled = true;

		template<typename t>
			requires is_same_v<GLuint, t> || is_same_v<int,t> || is_same_v< OglRenderTarget::RenderTarget*, t>
		void setRenderTarget(t renderTarget);

		template<>
		void setRenderTarget<GLuint>(GLuint renderTarget) {
			glBindFramebuffer(GL_FRAMEBUFFER, renderTarget);
			
			this->ClearCurrentRenderTarget = []() {

				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);
				glDepthMask(GL_TRUE);
				glDisable(GL_BLEND);

				glViewport(0, 0, OglWindow::Window::width, OglWindow::Window::height);
				glClearColor(0, 0, 0, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				};
		
			if(renderTarget==0) {
				this->isDefaultRenderTarget = true;
			}
			else {
				this->isDefaultRenderTarget = false;
			}

			this->currentRenderTarget=renderTarget;
		}
		template<>
		void setRenderTarget<int>(int renderTarget) {
			glBindFramebuffer(GL_FRAMEBUFFER, renderTarget);

			this->ClearCurrentRenderTarget = []() {

				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);
				glDepthMask(GL_TRUE);
				glDisable(GL_BLEND);

				glViewport(0, 0, OglWindow::Window::width, OglWindow::Window::height);
				glClearColor(0, 0, 0, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				};

			if (renderTarget == 0) {
				this->isDefaultRenderTarget = true;
			}
			else {
				this->isDefaultRenderTarget = false;
			}

			this->currentRenderTarget=(GLuint)renderTarget;
		}


		template<>
		void setRenderTarget<OglRenderTarget::RenderTarget*>(OglRenderTarget::RenderTarget* renderTarget) {
			

			this->currentRenderTarget = renderTarget;
			int fbo = renderTarget->getFBO();
			//cout << "FBO: "<<fbo<< endl;
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			this->ClearCurrentRenderTarget = [this]() {
				
				get<OglRenderTarget::RenderTarget*>(currentRenderTarget)->Clear();

				};
			
			this->isDefaultRenderTarget = false;

		}


		template<typename t>
			requires is_same_v<GLuint, t> || is_same_v<int, t> || is_same_v< OglRenderTarget::RenderTarget*, t>
		void setRenderTarget(t renderTarget, void(*ClearFrameFunc)()) {

			this->setRenderTarget<t>(renderTarget);

			this->ClearCurrentRenderTarget = ClearFrameFunc;

		}


		OglMouseEvents::MouseEventHandler& getMouseEventHandler();//for debug purpose

		void Render(OglScene::Scene& scene, MyCamera::BasicCamera& cam);//bu render k�sm�nday�m...
		void Render(OglScene::Scene& scene, MyCamera::OrbitalCamera& cam);
		void Render(OglScene::Scene& scene, MyCamera::Camera& cam); //ortho or perspective camera

		friend class OglPostProcessing::WeightedBlendedRenderPass;
		friend class OglPostProcessing::RenderPass;
		friend class OglPostProcessing::EffectComposer;
	};



	


}




#endif