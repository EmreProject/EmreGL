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

	//https://github.com/mrdoob/three.js/blob/master/examples/webgpu_instance_mesh.html
	//https://github.com/mrdoob/three.js/blob/master/examples/webgl_buffergeometry_instancing.html

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
	perspectiveCamera.setPosition(0, 0, -3);


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

	list<OglObject3D::Mesh&> meshList;
//INSTANCING USING MESH WITH INSTANCE ATTRIBUTE ON GEOMETRY------------------------
	
	OglObject3D::BoxGeometry boxGeometry(0.01, 0.01, 0.01,10,10,10);

	vector<MyMatrixV3::Matrix4f> instanceMatrices;

	int gridSize = 10;
	float gridLength = 10;
	float gridSpacing = gridLength / (gridSize - 1);
	int instanceCount = pow(gridSize, 3);
	MyVectorV3::Vector3f gridOffset = MyVectorV3::Vector3f(-gridLength / 2.0f, -gridLength / 2.0f, -gridLength / 2.0f);

	for (int i = 0; i < instanceCount; i++) {
		int x = (i % (gridSize * gridSize)) % gridSize;
		int y = i / (gridSize * gridSize);
		int z = (i % (gridSize * gridSize)) / gridSize;
		MyVectorV3::Vector3f position = MyVectorV3::Vector3f(x * gridSpacing, y * gridSpacing, z * gridSpacing) + gridOffset;
		MyMatrixV3::Matrix4f instanceMatrix;
		instanceMatrix.InitTranslationTransform(position.x, position.y, position.z);
		instanceMatrices.push_back(instanceMatrix);
	}


	OglObject3D::InstancedAttribute instanceAttribute(instanceMatrices);
	boxGeometry.setInstanceCount(instanceCount);
	boxGeometry.setInstancedAttribute("instanceMatrix", instanceAttribute);
	
	//RawShaderMaterial
	OglObject3D::RawShaderMaterial rawShaderMaterial1;
	string vertexShader1 = OglObject3D::RawShaderMaterial::LoadVertexShaderFromFile(SANDBOX_PATH "/07_instancing/shaders/basic.vert");
	string fragmentShader1 = OglObject3D::RawShaderMaterial::LoadFragmentShaderFromFile(SANDBOX_PATH "/07_instancing/shaders/basic.frag");
	rawShaderMaterial1.setVertexShader(vertexShader1);
	rawShaderMaterial1.setFragmentShader(fragmentShader1);

	OglObject3D::Mesh box1(boxGeometry, rawShaderMaterial1);
	meshList.push_back(box1);

	float x = 0.0f, y = 0.0f, z = 0.0f;
	box1.rotation.SetFromEuler(x, y, z);



	//MeshBasicMaterial
	OglObject3D::MeshBasicMaterial meshBasicMaterial1;
	meshBasicMaterial1.setMap(ground);
	meshBasicMaterial1.onBeforeCompile([](string* vs, string* fs) {

		string replaceStr = "gl_Position = viewProjectionMatrix * modelMatrix * vec4(transformedPosition,1.0);";
		int index = vs->find(replaceStr);
		vs->replace(index,replaceStr.size(), R"(

			 gl_Position = viewProjectionMatrix * instanceMatrix * modelMatrix * vec4(transformedPosition,1.0);
   

		)");

		});
	OglObject3D::Mesh box2(boxGeometry, meshBasicMaterial1);
	meshList.push_back(box2);



	//MeshStandardMaterial
	OglObject3D::MeshStandardMaterial meshStandardMaterial1;
	meshStandardMaterial1.setMap(ground);
	meshStandardMaterial1.setNormalMap(groundNormal);
	meshStandardMaterial1.setDisplacementMap(groundDisplacement, 0.01);

	meshStandardMaterial1.onBeforeCompile([](string* vs, string* fs) {

		string replaceStr = "gl_Position = viewProjectionMatrix * modelMatrix * vec4(transformedPosition,1.0);";
		int index = vs->find(replaceStr);
		vs->replace(index, replaceStr.size(), R"(

			 gl_Position = viewProjectionMatrix * instanceMatrix * modelMatrix * vec4(transformedPosition,1.0);
   

		)");

		});
	OglObject3D::Mesh box3(boxGeometry, meshStandardMaterial1);
	meshList.push_back(box3);



	//Model
	OglAssimp::ModelLoader modelLoader;
	OglAssimp::AnimationMixer* mixer;
	OglAssimp::AnimationClip* animationZero = nullptr;
	/*
	modelLoader.Load(ENGINE_ASSETS_DIR "/test/models/paichan2/scene.gltf",
		[&](OglAssimp::ModelLoader::ModelData& modelData) {

			modelData.position = MyVectorV3::Vector3f(0, 0, 0);
			modelData.scale = MyVectorV3::Vector3f(0.3, 0.3, 0.3);
			scene.add(modelData);

			for_each(modelData.meshes.begin(), modelData.meshes.end(), [&](OglObject3D::Mesh& mesh) {

				OglObject3D::MeshStandardMaterial* material = dynamic_cast<OglObject3D::MeshStandardMaterial*>(mesh.material);
				mesh.geometry->setInstanceCount(instanceCount);
				mesh.geometry->setInstancedAttribute("instanceMatrix", instanceAttribute);
				meshList.push_back(mesh);

				material->onBeforeCompile([](string* vs, string* fs) {

					string replaceStr = "gl_Position = viewProjectionMatrix * modelMatrix * vec4(transformedPosition,1.0);";
					int index = vs->find(replaceStr);
					vs->replace(index, replaceStr.size(), R"(

					 gl_Position = viewProjectionMatrix * instanceMatrix * modelMatrix * vec4(transformedPosition,1.0);
   

					)");

					});

				}); });
	*/

	/*
	modelLoader.Load(ENGINE_ASSETS_DIR "/test/models/epona/scene.gltf",
		[&](OglAssimp::ModelLoader::ModelData& modelData) {


			mixer = new OglAssimp::AnimationMixer(modelData);
			animationZero = mixer->clipAction(0);
			animationZero->isInfinite = true;
			animationZero->Play();


			modelData.position = MyVectorV3::Vector3f(0, 0, 0);
			modelData.scale = 0.2;

			scene.add(modelData);

			for_each(modelData.meshes.begin(), modelData.meshes.end(), [&](OglObject3D::Mesh& mesh) {

				OglObject3D::MeshStandardMaterial* material = dynamic_cast<OglObject3D::MeshStandardMaterial*>(mesh.material);
				mesh.geometry->setInstanceCount(instanceCount);
				mesh.geometry->setInstancedAttribute("instanceMatrix", instanceAttribute);
				meshList.push_back(mesh);


				material->onBeforeCompile([](string* vs, string* fs) {

					string replaceStr = "gl_Position = viewProjectionMatrix * modelMatrix * vec4(transformedPosition,1.0);";
					int index = vs->find(replaceStr);
					vs->replace(index, replaceStr.size(), R"(

					 gl_Position = viewProjectionMatrix * instanceMatrix * modelMatrix * vec4(transformedPosition,1.0);
   

					)");

					});

				});

		});

		*/

	

	function<void()> UpdateParameters = [&]() {

		instanceMatrices.erase(instanceMatrices.begin(), instanceMatrices.end());

		gridSpacing = gridLength / (gridSize - 1);
		instanceCount = pow(gridSize, 3);
		MyVectorV3::Vector3f gridOffset = MyVectorV3::Vector3f(-gridLength / 2.0f, -gridLength / 2.0f, -gridLength / 2.0f);

		for (int i = 0; i < instanceCount; i++) {
			int x = (i % (gridSize * gridSize)) % gridSize;
			int y = i / (gridSize * gridSize);
			int z = (i % (gridSize * gridSize)) / gridSize;
			MyVectorV3::Vector3f position = MyVectorV3::Vector3f(x * gridSpacing, y * gridSpacing, z * gridSpacing) + gridOffset;
			MyMatrixV3::Matrix4f instanceMatrix;
			instanceMatrix.InitTranslationTransform(position.x, position.y, position.z);
			instanceMatrices.push_back(instanceMatrix);
		}

		OglObject3D::InstancedAttribute instanceAttributeNew(instanceMatrices);
		for_each(meshList.begin(), meshList.end(), [&](OglObject3D::Mesh& mesh) {

			mesh.geometry->setInstanceCount(instanceCount);
			mesh.geometry->setInstancedAttribute("instanceMatrix", instanceAttributeNew);
			});
	};


	scene.add(dirLight1);
	//scene.add(box1);
	//scene.add(box2);
	scene.add(box3);


	OglPostProcessing::EffectComposer composer(&renderer);
	OglPostProcessing::WeightedBlendedRenderPass weightBlendedPass(scene, perspectiveCamera);

	composer.AddPass(&weightBlendedPass);


	DAT::Gui gui(300);
	OglWindow::Window::AddGui(&gui);
	bool pauseAnimation = false;


	gui.add("dirLight x", &dirLight1.WorldDirection.x, -1.0f, 1.0f);
	gui.add("dirLight y", &dirLight1.WorldDirection.y, -1.0f, 1.0f);
	gui.add("dirLight z", &dirLight1.WorldDirection.z, -1.0f, 1.0f);

	gui.add("grid size", &gridSize, 5, 30);

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


		mixer->Update(deltaTime); //assuming 60 fps, call every frame with delta time in seconds
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
