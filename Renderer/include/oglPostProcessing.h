#ifndef oglPostProcessing_h
#define oglPostProcessing_h



#include <GL/glew.h>
#include "oglScene.h"
#include "oglMeshes.h"
#include "oglRenderer.h"
#include "oglRenderTarget.h" 


namespace OglPostProcessing {

	/*
		EXAMPLE USAGE:

				const composer = new EffectComposer( renderer );
			// adding some passes
			const renderPass = new RenderPass( scene, camera );
			composer.addPass( renderPass );
			const glitchPass = new GlitchPass();
			composer.addPass( glitchPass );
			const outputPass = new OutputPass()
			composer.addPass( outputPass );
			function animate() {
				composer.render(); // instead of renderer.render()
			}

	*/


	//Base Pass class
	class Pass {
	protected:

		MyCamera::OrthographicCamera orthographicCamera;

	public:

		Pass();

		virtual MyTextureV2::Texture getOutputTexture() = 0;
		virtual void Render(OglRenderer::Renderer*) = 0;
	};


	//Effect Composer
	class EffectComposer {

	private:

			OglRenderer::Renderer* renderer;
			list<Pass*> passes;

			OglScene::Scene screenScene; //for screen quad
			OglObject3D::Mesh screenQuad;
			MyCamera::OrthographicCamera orthographicCamera;

			void SetScreenQuad();

	public:

		EffectComposer(OglRenderer::Renderer* renderer);
		void AddPass(Pass* pass);

		void Render();

	};


	//Render Pass
	class RenderPass :public Pass {
	private:

		OglRenderTarget::RenderTarget renderTarget;
		OglScene::Scene* scene;
		MyCamera::PerspectiveCamera* perspectiveCamera;

		void SetRenderTarget();

	public:

		RenderPass(OglScene::Scene& scene,
			MyCamera::PerspectiveCamera& camera);


		MyTextureV2::Texture getOutputTexture();
		void Render(OglRenderer::Renderer*); //done by effectcomposer!!!
		
	};

	//Weighted Blended Transparency Render Pass, !Use it instead of normal render pass
	class WeightedBlendedRenderPass : public Pass{
	private:
		//Render Targets
		OglRenderTarget::RenderTarget opaqueRT;
		OglRenderTarget::RenderTarget transparentRT;

		//Clear Frame Functions
		inline static void(*ClearOpaqueFrameFunc)() = []() {

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glViewport(0, 0, OglWindow::Window::width, OglWindow::Window::height);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			};

		inline static void(*ClearTransparentFrameFunc)() = []() {

			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunci(0, GL_ONE, GL_ONE);
			glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
			glBlendEquation(GL_FUNC_ADD);
			glViewport(0, 0, OglWindow::Window::width, OglWindow::Window::height);

			
			MyVectorV3::Vector4f zeroFillerVec(0.0f, 0.0f, 0.0f, 0.0f);
			MyVectorV3::Vector4f oneFillerVec(1.0f, 1.0f, 1.0f, 1.0f);
			glClearBufferfv(GL_COLOR, 0, &zeroFillerVec.x);
			glClearBufferfv(GL_COLOR, 1, &oneFillerVec.x);

			};

		inline static void(*ClearCompositeFrameFunc)() = []() {

			glDepthFunc(GL_ALWAYS);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glViewport(0, 0, OglWindow::Window::width, OglWindow::Window::height);

			};



		//Scene
		OglScene::Scene* wholeScene;
		OglScene::Scene opaqueScene;
		OglScene::Scene transparentScene;
		OglScene::Scene compositeScene;
	
		//Cameras perspective and ortho(from parent class)
		MyCamera::PerspectiveCamera* perspectiveCamera;

		//Quad for composite pass
		OglObject3D::Mesh quadMesh;

		void SetRenderTargets();
		void ModifyTransparentMaterial(OglObject3D::MeshBasicMaterial*);
		
		void SetQuadMesh();

		void SetPerspectiveCamera(MyCamera::PerspectiveCamera* camera);
		void SetScene(OglScene::Scene& scene);

	public:

		WeightedBlendedRenderPass(OglScene::Scene& scene, 
				MyCamera::PerspectiveCamera& camera);
	

		MyTextureV2::Texture getOutputTexture();
		void Render(OglRenderer::Renderer*);
		

	};

}
#endif