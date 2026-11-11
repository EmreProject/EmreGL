#include "oglRenderTarget.h"
#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Camera.h"
#include "ogl_lightning_v2.h"
#include "ogl_texture.h"
#include "oglScene.h"
#include "oglRenderer.h"
#include "oglWindow.h"
#include "oglControls.h"




	int main() {


		float windowWidth = 1000;
		float windowHeight = 800;

		float FOV = 45.0f;
		float zNear = 0.1f;
		float zFar = 1000.0f;

		MyProjection::PersProjInfo persProjInfo = {
			FOV, windowWidth, windowHeight, zNear, zFar };



		//window
		OglWindow::Window::CreateNewWindow(windowWidth, windowHeight);




		//renderer
		OglRenderer::Renderer renderer;
		renderer.shadowEnabled = true;



		//Textures
		MyTextureV2::Texture brick(ENGINE_ASSETS_DIR "/images/brick1/map.png", GL_SRGB8_ALPHA8,
			GL_RGBA, GL_UNSIGNED_BYTE, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		MyTextureV2::Texture brickNormal(ENGINE_ASSETS_DIR "/images/brick1/normal.jpg");


		MyTextureV2::Texture ground(ENGINE_ASSETS_DIR "/images/ground1/map.png", GL_SRGB8_ALPHA8);
		MyTextureV2::Texture groundNormal(ENGINE_ASSETS_DIR "/images/ground1/normal.jpg");
		MyTextureV2::Texture groundDisplacement(ENGINE_ASSETS_DIR "/images/ground1/displacement.png"); //intensity is 0.1f



		//camera
		MyCamera::PerspectiveCamera perspectiveCamera;
		perspectiveCamera.setPerspectiveProjection(persProjInfo);
		//perspectiveCamera.setPosition(0, 0, -5);


		//Controls
		OglControls::OrbitalControls orbitalControls(&perspectiveCamera);

		//lights

		MyOglLightV2::DirectionalLight dirLight1;
		dirLight1.shadow.castShadow = true;
		dirLight1.setColor(1, 1, 1);
		dirLight1.DiffuseIntensity = 3;
		dirLight1.AmbientIntensity = 0.5;
		dirLight1.setWorldDirection(0, -1, 5);


		//default meshes
		OglObject3D::PlaneGeometry terrain1(5, 5, 100, 100);
		OglObject3D::MeshBasicMaterial terrainMaterial1;
		terrainMaterial1.setMap(ground);



		OglObject3D::Mesh terrainMesh1(terrain1, terrainMaterial1);
		terrainMesh1.material->setWireframe(false);
		terrainMesh1.rotation.x = -90;


		//Rawshader material
		OglObject3D::RawShaderMaterial material2;
		string vertexShader1 = OglObject3D::RawShaderMaterial::LoadVertexShaderFromFile(SANDBOX_PATH "/04_basic_rendering/shaders/final.vert");
		string fragmentShader1 = OglObject3D::RawShaderMaterial::LoadFragmentShaderFromFile(SANDBOX_PATH "/04_basic_rendering/shaders/final.frag");
		material2.setVertexShader(vertexShader1);
		material2.setFragmentShader(fragmentShader1);
		material2.setWireframe(false);
		//material2.setUniform("texture1", "./ogldevTutorials/images/ground1/map.png", 1, GL_TEXTURE1);


		OglObject3D::BoxGeometry boxGeometry1(0.2, 0.2, 0.2, 20, 20, 20);
		OglObject3D::Mesh box1(boxGeometry1, material2);
		box1.position.y = 0;



		//scene
		OglScene::Scene scene;

		//scene.add(&terrainMesh1);
		scene.add(box1);
		scene.add(dirLight1);

		std::cout << glGetString(GL_VERSION) << '\n';
		std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';





		//ortho camera
		MyCamera::OrthographicCamera orthoCamera;
		orthoCamera.setOrthographicProjection(-1, 1, -1, 1, 0.1, 1);
		orthoCamera.setPosition(0, 0, 0.5);
		orthoCamera.lookAt(0, 0, 0);

		//prescene and pre plane
		OglScene::Scene preScene;

		OglObject3D::PlaneGeometry prePlane(2, 2);

		OglObject3D::RawShaderMaterial preMaterial;
		string preVertexShader = OglObject3D::RawShaderMaterial::LoadVertexShaderFromFile(SANDBOX_PATH "/04_basic_rendering/shaders/init.vert");
		string preFragmentShader = OglObject3D::RawShaderMaterial::LoadFragmentShaderFromFile(SANDBOX_PATH "/04_basic_rendering/shaders/init.frag");
		preMaterial.setVertexShader(preVertexShader);
		preMaterial.setFragmentShader(preFragmentShader);
		OglObject3D::Mesh preMesh(prePlane, preMaterial);


		preScene.add(preMesh);


		//init RenderTarget
		OglRenderTarget::DataTexture2DAttachment dataTextureAttachment;
		dataTextureAttachment.magFilter = GL_LINEAR;
		dataTextureAttachment.minFilter = GL_LINEAR;
		OglRenderTarget::RenderTarget initRenderTarget(21, 21);
		initRenderTarget.AddColorBuffer(&dataTextureAttachment, 1);
		initRenderTarget.SetDepthBuffer();//default one, i can also make my own
		cout << "initRenderTarget status: " << initRenderTarget.checkStatus() << endl;
		cout << "initRenderTarget FBO: " << initRenderTarget.getFBO() << endl;
		renderer.setRenderTarget(&initRenderTarget);
		renderer.Render(preScene, orthoCamera);


		renderer.setRenderTarget(0);
		material2.setUniform("positionData", initRenderTarget.getColorTexture(0));


		//example 2 - MRT-------------------------------------------------------------------------

		OglScene::Scene preScene2;


		OglObject3D::RawShaderMaterial preMaterial2;
		string preVertexShader2 = OglObject3D::RawShaderMaterial::LoadVertexShaderFromFile(SANDBOX_PATH "/04_basic_rendering/shaders/init.vert");
		string preFragmentShader2 = OglObject3D::RawShaderMaterial::LoadFragmentShaderFromFile(SANDBOX_PATH "/04_basic_rendering/shaders/init.frag");
		preMaterial2.setVertexShader(preVertexShader2);
		preMaterial2.setFragmentShader(preFragmentShader2);
		OglObject3D::Mesh preMesh2(prePlane, preMaterial2);
		preMesh2.material->setUniform("maxHeight", 0.5f);
		preMesh2.material->setUniform("minRadius", 0.2f);
		preMesh2.material->setUniform("maxRadius", 1.f);

		preScene2.add(preMesh2);



		OglScene::Scene scene2;


		OglObject3D::PlaneGeometry planeGeometry(2, 2, 100, 100);
		OglObject3D::RawShaderMaterial preMaterial3;
		string preVertexShader3 = OglObject3D::RawShaderMaterial::LoadVertexShaderFromFile(SANDBOX_PATH "/04_basic_rendering/shaders/final.vert");
		string preFragmentShader3 = OglObject3D::RawShaderMaterial::LoadFragmentShaderFromFile(SANDBOX_PATH "/04_basic_rendering/shaders/final.frag");
		preMaterial3.setVertexShader(preVertexShader3);
		preMaterial3.setFragmentShader(preFragmentShader3);

		OglObject3D::Points preMesh3(planeGeometry, preMaterial3);

		scene2.add(preMesh3);


		//init RenderTarget
		OglRenderTarget::DataTexture2DAttachment dataTextureAttachment2;
		dataTextureAttachment2.magFilter = GL_NEAREST;
		dataTextureAttachment2.minFilter = GL_NEAREST;
		OglRenderTarget::RenderTarget initRenderTarget2(100, 100);
		initRenderTarget2.AddColorBuffer(&dataTextureAttachment2, 1);
		initRenderTarget2.SetDepthBuffer();//default one, i can also make my own
		cout << "initRenderTarget status: " << initRenderTarget2.checkStatus() << endl;
		cout << "initRenderTarget FBO: " << initRenderTarget2.getFBO() << endl;
		renderer.setRenderTarget(&initRenderTarget2);
		renderer.Render(preScene2, orthoCamera);


		renderer.setRenderTarget(0);
		preMesh3.material->setUniform("positionData", initRenderTarget2.getColorTexture(0));

		while (!glfwWindowShouldClose(OglWindow::Window::glfwWindow))
		{

			glfwPollEvents();


			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




			renderer.Render(scene2, perspectiveCamera);
			//renderer.Render(scene, orbitalCamera);


			orbitalControls.Update();


			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(OglWindow::Window::glfwWindow);


		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwTerminate();
	}


