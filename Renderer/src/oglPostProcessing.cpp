#include "oglPostProcessing.h"


namespace OglPostProcessing {

	//Pass
	Pass::Pass() {

		orthographicCamera.setOrthographicProjection(-1, 1, -1, 1, 0.1, 1);
		orthographicCamera.setPosition(0, 0, 0.5);
		orthographicCamera.lookAt(0, 0, 0);

	}
	
	//Effect Composer

	EffectComposer::EffectComposer(OglRenderer::Renderer* renderer) {

		orthographicCamera.setOrthographicProjection(-1, 1, -1, 1, 0.1, 1);
		orthographicCamera.setPosition(0, 0, 0.5);
		orthographicCamera.lookAt(0, 0, 0);


		this->renderer = renderer;
		this->SetScreenQuad();
		this->screenScene.add(&this->screenQuad);
	}

	void EffectComposer::SetScreenQuad() {

		OglObject3D::PlaneGeometry* planeGeom=new OglObject3D::PlaneGeometry(2.0f, 2.0f);
		OglObject3D::RawShaderMaterial* quadMat=new OglObject3D::RawShaderMaterial();

		quadMat->setVertexShader(R"(#version 430
				layout (location = 2) in vec3 normal;
				layout (location = 1) in vec3 position;
				layout (location = 0) in vec2 uv;
				
				uniform mat4 viewMatrix;
				uniform mat4 projectionMatrix;
				uniform mat4 modelMatrix;

				out vec2 v_uv;

				void main()
				{
					mat4 bos=projectionMatrix * viewMatrix * modelMatrix;
					gl_Position = vec4(position, 1.0);
					v_uv = uv;
					
				}
		)");

		quadMat->setFragmentShader(R"(#version 430
				// shader outputs
				out vec4 frag;
				in vec2 v_uv;

				// input texture
				uniform sampler2D inputTexture;
				
				void main()
				{
					
					frag = texture(inputTexture, v_uv) ;
					
				}
		)");

		this->screenQuad.geometry = planeGeom;
		this->screenQuad.material = quadMat;
		

	}

	void EffectComposer::AddPass(Pass* pass) {
		this->passes.push_back(pass);
	}

	void EffectComposer::Render() {

		auto it = passes.begin();
		renderer->environmentMapRenderingEnabled = true;
		(*it)->Render(this->renderer);//first render pass
		renderer->environmentMapRenderingEnabled = false;
		it++;
		for (; it != passes.end(); it++) {

			//!!!
			//Set input texture from previous pass. I have to implement 
			// it later!!!!!!!!!!!
			//!!!
			//! 
			(*it)->Render(this->renderer);
		}

		it--; //turn back to last pass


		MyTextureV2::Texture finalTexture = (*it)->getOutputTexture();
		this->screenQuad.material->setUniform<MyTextureV2::Texture>("inputTexture", finalTexture);

		this->renderer->setRenderTarget(0, []() {

			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
			glDisable(GL_BLEND);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glViewport(0, 0, OglWindow::Window::width, OglWindow::Window::height);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


			}); //default framebuffer
		this->renderer->Render(this->screenScene, this->orthographicCamera);

		renderer->environmentMapRenderingEnabled = true;

	}


	//Render Pass

	RenderPass::RenderPass(OglScene::Scene& scene,
		MyCamera::PerspectiveCamera& camera) : Pass(),
		renderTarget(OglWindow::Window::width, OglWindow::Window::height){

		this->SetRenderTarget();
		this->scene = &scene;
		this->perspectiveCamera = &camera;

	}


	void RenderPass::SetRenderTarget() {

		this->renderTarget = OglRenderTarget::RenderTarget(OglWindow::Window::width, OglWindow::Window::height);
	
		OglRenderTarget::ColorTexture2DAttachment colorAttachment;

		renderTarget.AddColorBuffer(&colorAttachment, 1);
		renderTarget.SetDepthBuffer();
		renderTarget.Create();
	
	}


	MyTextureV2::Texture RenderPass::getOutputTexture() {

		return this->renderTarget.getColorTexture(0);
	}

	void RenderPass::Render(OglRenderer::Renderer* renderer) {

		renderer->setRenderTarget(&this->renderTarget);
		renderer->mouseEventRenderingEnabled = true;
		renderer->Render(*this->scene, *this->perspectiveCamera);
		renderer->mouseEventRenderingEnabled = false;

	}

	//Weighted Blended Transparency Render Pass, !Use it instead of normal render pass
	
	
	
	WeightedBlendedRenderPass::WeightedBlendedRenderPass(OglScene::Scene& scene,
		MyCamera::PerspectiveCamera& camera): Pass(), transparentRT(OglWindow::Window::width,OglWindow::Window::height),
		opaqueRT(OglWindow::Window::width, OglWindow::Window::height) 
	{
		this->SetRenderTargets();
		this->SetQuadMesh();
		this->SetPerspectiveCamera(&camera);
		this->wholeScene = &scene;
		this->SetScene(scene);
		this->opaqueScene.SetEnvironmentMap(scene.GetEnvironmentMapTexture());
		compositeScene.add(&quadMesh);
	}

	void WeightedBlendedRenderPass::SetRenderTargets() {

		transparentRT = OglRenderTarget::RenderTarget(OglWindow::Window::width, OglWindow::Window::height);
		opaqueRT = OglRenderTarget::RenderTarget(OglWindow::Window::width, OglWindow::Window::height);
	

		OglRenderTarget::DepthTexture2DAttachment depthAttachment(OglWindow::Window::width, OglWindow::Window::height);
		

		OglRenderTarget::ColorTexture2DAttachment colorAttachmentOpaque;
		colorAttachmentOpaque.internalFormat = GL_RGBA16F;
		colorAttachmentOpaque.format = GL_RGBA;
		colorAttachmentOpaque.type = GL_HALF_FLOAT;

		opaqueRT.AddColorBuffer(&colorAttachmentOpaque, 1);
		opaqueRT.SetDepthBuffer(&depthAttachment);
		opaqueRT.Create();



		OglRenderTarget::ColorTexture2DAttachment colorAttachmentTransparentAccum;
		colorAttachmentTransparentAccum.internalFormat = GL_RGBA16F;
		colorAttachmentTransparentAccum.format = GL_RGBA;
		colorAttachmentTransparentAccum.type = GL_HALF_FLOAT;

		OglRenderTarget::ColorTexture2DAttachment colorAttachmentTransparentRevealage;
		colorAttachmentTransparentRevealage.internalFormat = GL_R16F;
		colorAttachmentTransparentRevealage.format = GL_RED;
		colorAttachmentTransparentRevealage.type = GL_FLOAT;

		transparentRT.AddColorBuffer(&colorAttachmentTransparentAccum, 1);
		transparentRT.AddColorBuffer(&colorAttachmentTransparentRevealage, 1);
		transparentRT.SetDepthBuffer(opaqueRT);
		transparentRT.Create();
		
	}

	void WeightedBlendedRenderPass::SetPerspectiveCamera(MyCamera::PerspectiveCamera* camera) {

		this->perspectiveCamera = camera;
	}
	
	void WeightedBlendedRenderPass::SetQuadMesh() {

		OglObject3D::PlaneGeometry* planeGeom=new OglObject3D::PlaneGeometry(2.0f, 2.0f);
		OglObject3D::RawShaderMaterial* quadMat=new OglObject3D::RawShaderMaterial();

		quadMat->setVertexShader(R"(#version 430
				layout (location = 2) in vec3 normal;
				layout (location = 1) in vec3 position;
				layout (location = 0) in vec2 uv;
				
				uniform mat4 viewMatrix;
				uniform mat4 projectionMatrix;
				uniform mat4 modelMatrix;

				void main()
				{

					mat4 bos=projectionMatrix * viewMatrix * modelMatrix;
					gl_Position = vec4(position, 1.0);
					
				}
		)");

		quadMat->setFragmentShader(R"(#version 430

				// shader outputs
				layout (location = 0) out vec4 frag;
				
				// color accumulation buffer
				uniform sampler2D accum;
				
				// revealage threshold buffer
				uniform sampler2D reveal;
				
				// epsilon number
				const float EPSILON = 0.00001f;
				
				// calculate floating point numbers equality accurately
				bool isApproximatelyEqual(float a, float b)
				{
					return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
				}
				
				// get the max value between three values
				float max3(vec3 v) 
				{
					return max(max(v.x, v.y), v.z);
				}
				
				void main()
				{
					// fragment coordination
					ivec2 coords = ivec2(gl_FragCoord.xy);
					
					// fragment revealage
					float revealage = texelFetch(reveal, coords, 0).r;
					
					// save the blending and color texture fetch cost if there is not a transparent fragment
					if (isApproximatelyEqual(revealage, 1.0f)) 
						discard;
				 
					// fragment color
					vec4 accumulation = texelFetch(accum, coords, 0);
					
					// suppress overflow
					if (isinf(max3(abs(accumulation.rgb)))) 
						accumulation.rgb = vec3(accumulation.a);
				
					// prevent floating point precision bug
					vec3 average_color = accumulation.rgb / max(accumulation.a, EPSILON);
				
					// blend pixels
					frag = vec4(average_color, 1.0f - revealage);
					//frag=vec4(1.,1.,1.,0.5);
				}
		)");


		quadMesh.geometry = planeGeom;
		quadMesh.material = quadMat;

		
	}

	void WeightedBlendedRenderPass::ModifyTransparentMaterial(OglObject3D::MeshBasicMaterial* mat) {

		function<void(string*, string*)> modifyShaderFunc = [](string* vs, string* fs) {

			string replaceHook = "<out_fragment_color>";
			string replaceWith = R"(
				 vec4 FragColor;

				layout (location = 0) out vec4 accum;
				layout (location = 1) out float reveal;
			)";

			string::size_type find = fs->find(replaceHook);

			if (find != string::npos) {

				fs->replace(find, replaceHook.size(), replaceWith);
			}

		

			replaceHook = "<end_fragment_result_assimp>";
			replaceWith = R"(
				
				
				<end_fragment_result_environment_map>
				// weight function
				float weight = clamp(pow(min(1.0, FragColor.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
				
				// store pixel color accumulation
				accum = vec4(FragColor.rgb * FragColor.a, FragColor.a) * weight;
				
				// store pixel revealage threshold
				reveal = FragColor.a;

			)";

			find = fs->find(replaceHook);
			if (find != string::npos) {
				fs->replace(find, replaceHook.size(), replaceWith);
			}


			replaceHook = "<end_fragment_gamma_enabled>";
			replaceWith = R"(
				
				//...

			)";

			find = fs->find(replaceHook);
			if (find != string::npos) {
				fs->replace(find, replaceHook.size(), replaceWith);
			}

			};

			
		


		mat->onBeforeCompile(modifyShaderFunc);

	}

	void WeightedBlendedRenderPass::SetScene(OglScene::Scene& scene) {

		

		this->opaqueScene.directionalLights = scene.directionalLights;
		this->opaqueScene.pointLights = scene.pointLights;
		this->opaqueScene.spotLights = scene.spotLights;
		this->opaqueScene.models = scene.models;
	
		this->transparentScene.directionalLights = scene.directionalLights;
		this->transparentScene.pointLights = scene.pointLights;
		this->transparentScene.spotLights = scene.spotLights;
		this->transparentScene.models = scene.models;


		transparentScene.meshes.clear();
		opaqueScene.meshes.clear();

		for (auto mesh : scene.meshes) {
			if (mesh->material->transparent) {

				OglObject3D::MeshBasicMaterial* mat = dynamic_cast<OglObject3D::MeshBasicMaterial*>(mesh->material);
				if (mat) {
					ModifyTransparentMaterial(mat);
				}

				this->transparentScene.meshes.push_back(mesh);
			}
			else {
				this->opaqueScene.meshes.push_back(mesh);
			}
		}

	}


	MyTextureV2::Texture WeightedBlendedRenderPass::getOutputTexture() {
	
		return this->opaqueRT.getColorTexture(0);

	}

		
	void WeightedBlendedRenderPass::Render(OglRenderer::Renderer* renderer){
	

		this->SetScene(*wholeScene); //update scenes

		renderer->ApplyWorldMatrixAssimp(*wholeScene);
		renderer->mouseEventRenderingEnabled = false;
		renderer->environmentMapRenderingEnabled = false;;

		renderer->UpdateMeshMaterialIfEnvironmentMap(*wholeScene, perspectiveCamera->GetPosition());


		renderer->shadowEnabled = true;
		renderer->enableIfDirLightShadowMaps(opaqueScene); //set shadow map's dir light variable
		renderer->enableIfDirLightShadowMaps(transparentScene);
		renderer->enableIfSpotLightShadowMaps(opaqueScene); //set shadow map's spot light variable
		renderer->enableIfSpotLightShadowMaps(transparentScene);
		renderer->enableIfPointLightShadowMaps(opaqueScene); //set shadow map's point light variable
		renderer->enableIfPointLightShadowMaps(transparentScene); //set shadow map's point light variable
		

		

		//1. Render opaque scene
		renderer->setRenderTarget(&this->opaqueRT,ClearOpaqueFrameFunc);
		renderer->shadowEnabled = true;

		renderer->BindCurrentRenderTarget();
		//glViewport(0, 0, OglWindow::Window::width, OglWindow::Window::height);
		renderer->ClearCurrentRenderTarget();

		
		list<OglObject3D::Mesh*> opaqueMeshes = opaqueScene.GetOpaqueMeshes();
		for_each(opaqueMeshes.begin(), opaqueMeshes.end(), [this,&renderer](OglObject3D::Mesh* mesh) {
		
			
			renderer->RenderSingleMesh(mesh, opaqueScene, perspectiveCamera->GetPosition(), perspectiveCamera->GetViewMatrix(), perspectiveCamera->GetProjectionMatrix());


			});



		renderer->RenderEnvironmentMap(opaqueScene, perspectiveCamera->GetViewMatrix(), perspectiveCamera->GetProjectionMatrix());

		//renderer->Render(this->opaqueScene, *this->perspectiveCamera);

		//2. Render transparent scene
		renderer->setRenderTarget(&this->transparentRT, ClearTransparentFrameFunc);
		renderer->shadowEnabled = true;
		
		renderer->BindCurrentRenderTarget();
		renderer->ClearCurrentRenderTarget();

		
		list<OglObject3D::Mesh*> transparentMeshes = transparentScene.GetTransparentMeshes();
		auto it = transparentMeshes.begin();
		for (; it != transparentMeshes.end(); it++) {
			OglObject3D::Mesh* mesh = *it;
			//cout << i++ << ": " << "trans" << endl;	
			renderer->RenderSingleMesh(mesh, transparentScene, perspectiveCamera->GetPosition(), perspectiveCamera->GetViewMatrix(), perspectiveCamera->GetProjectionMatrix());

		}
		
		//->Render(this->transparentScene, *this->perspectiveCamera);

	
		//3. Composite pass
		renderer->setRenderTarget(&this->opaqueRT,ClearCompositeFrameFunc);
		this->quadMesh.material->setUniform<MyTextureV2::Texture>("accum", this->transparentRT.getColorTexture(0));
		this->quadMesh.material->setUniform<MyTextureV2::Texture>("reveal", this->transparentRT.getColorTexture(1));
		renderer->shadowEnabled = false;
		renderer->Render(this->compositeScene, this->orthographicCamera);
		
		
		renderer->mouseEventRenderingEnabled = true;
		renderer->RenderMouseEvent(*wholeScene, *perspectiveCamera);
		renderer->mouseEventRenderingEnabled = false;


		renderer->ApplyReverseWorldMatrixAssimp(*wholeScene);

		renderer->shadowEnabled = true;

		for_each(wholeScene->directionalLights.begin(), wholeScene->directionalLights.end(), [](MyOglLightV2::DirectionalLight* dirLight) {
			dirLight->shadowMap.ClearFBO();
			});
		renderer->RenderDirectionalLightShadowMap(opaqueScene);
		renderer->RenderDirectionalLightShadowMap(transparentScene);

		for_each(wholeScene->spotLights.begin(), wholeScene->spotLights.end(), [](MyOglLightV2::SpotLight* spot) {
			spot->shadowMap.ClearFBO();
			});
		renderer->RenderSpotLightShadowMaps(opaqueScene);
		renderer->RenderSpotLightShadowMaps(transparentScene);


		for_each(wholeScene->pointLights.begin(), wholeScene->pointLights.end(), [](MyOglLightV2::PointLight* point) {
			point->shadowMap.ClearFBO();
			});
		renderer->RenderPointLightShadowMaps(opaqueScene);
		renderer->RenderPointLightShadowMaps(transparentScene);

		
		renderer->environmentMapRenderingEnabled = true;;

	}


}