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
#include "oglPostProcessing.h"

int main() {


	float windowWidth = 1000;
	float windowHeight = 800;

	float FOV = 45.0f;
	float zNear = 0.11f;
	float zFar = 100.0f;

	MyProjection::PersProjInfo persProjInfo = {
		FOV, windowWidth, windowHeight, zNear, zFar };



	//Scene
	OglScene::Scene scene;


	//window
	OglWindow::Window::CreateNewWindow(windowWidth, windowHeight);


	//renderer
	OglRenderer::Renderer renderer;
	renderer.shadowEnabled = true;



	//camera
	MyCamera::PerspectiveCamera perspectiveCamera;
	perspectiveCamera.setPerspectiveProjection(persProjInfo);
	perspectiveCamera.setPosition(0, 0, 3);


	//Controls
	OglControls::OrbitalControls orbitalControls(&perspectiveCamera);


	//Textures

	MyTextureV2::Texture ground(ENGINE_ASSETS_DIR "/images/ground1/map.png", GL_SRGB8_ALPHA8);
	MyTextureV2::Texture groundNormal(ENGINE_ASSETS_DIR "/images/ground1/normal.jpg");
	MyTextureV2::Texture groundDisplacement(ENGINE_ASSETS_DIR "/images/ground1/displacement.png"); //intensity is 0.1f




	//lights
	MyOglLightV2::DirectionalLight dirLight1;
	dirLight1.shadow.castShadow = true;
	dirLight1.setColor(1, 1, 1);
	dirLight1.DiffuseIntensity = 3;
	dirLight1.AmbientIntensity = 0.2;
	dirLight1.setWorldDirection(0, -1, -1);


	//Raw Shader Mesh
	OglObject3D::BoxGeometry boxGeometry(0.2, 0.2, 0.2);
	OglObject3D::RawShaderMaterial rawShaderMaterial1;
	string vertexShader1 = OglObject3D::RawShaderMaterial::LoadVertexShaderFromFile(SANDBOX_PATH "/07_instancing/shaders/basic.vert");
	string fragmentShader1 = OglObject3D::RawShaderMaterial::LoadFragmentShaderFromFile(SANDBOX_PATH "/07_instancing/shaders/basic.frag");
	rawShaderMaterial1.setVertexShader(vertexShader1);
	rawShaderMaterial1.setFragmentShader(fragmentShader1);

	OglObject3D::Mesh box1(boxGeometry, rawShaderMaterial1);

	/*
	//Model Loader
	OglAssimp::ModelLoader modelLoader;
	OglAssimp::AnimationMixer* mixer;
	OglAssimp::AnimationClip* animationZero = nullptr;

	modelLoader.Load(ENGINE_ASSETS_DIR "/test/models/paichan2/scene.gltf",
		[&animationZero, &scene, &mixer](OglAssimp::ModelLoader::ModelData& modelData) {

			modelData.position = MyVectorV3::Vector3f(1, -0.5, 0);
			scene.add(modelData);

		});


	modelLoader.Load(ENGINE_ASSETS_DIR "/test/models/epona/scene.gltf",
		[&animationZero, &scene, &mixer](OglAssimp::ModelLoader::ModelData& modelData) {


			mixer = new OglAssimp::AnimationMixer(modelData);
			animationZero = mixer->clipAction(0);
			animationZero->isInfinite = true;
			animationZero->Play();


			modelData.position = MyVectorV3::Vector3f(0, -0.5, 0);
			modelData.scale = 55;

			scene.add(modelData);

		});



	*/




	scene.add(dirLight1);
	scene.add(box1);


	OglPostProcessing::EffectComposer composer(&renderer);
	OglPostProcessing::WeightedBlendedRenderPass weightBlendedPass(scene, perspectiveCamera);

	composer.AddPass(&weightBlendedPass);


	DAT::Gui gui(300);
	OglWindow::Window::AddGui(&gui);
	bool pauseAnimation = false;


	gui.add("dirLight x", &dirLight1.WorldDirection.x, -1.0f, 1.0f);
	gui.add("dirLight y", &dirLight1.WorldDirection.y, -1.0f, 1.0f);
	gui.add("dirLight z", &dirLight1.WorldDirection.z, -1.0f, 1.0f);



	long long lastTime = OglAssimp::GetCurrentTimeAsMilliseconds();
	while (!glfwWindowShouldClose(OglWindow::Window::glfwWindow))
	{

		glfwPollEvents();
		/*
					glEnable(GL_CULL_FACE);
					glFrontFace(GL_CCW);
		*/

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		long long  currentTime = OglAssimp::GetCurrentTimeAsMilliseconds();
		long long deltaTime = currentTime - lastTime;
		lastTime = currentTime;


		//mixer->Update(deltaTime); //assuming 60 fps, call every frame with delta time in seconds
		//cout << "Delta time: " << deltaTime << " ms\n";



		composer.Render();

		orbitalControls.Update();


		gui.Render();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(OglWindow::Window::glfwWindow);


	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

}
