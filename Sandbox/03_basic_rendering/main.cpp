#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Camera.h"
#include "ogl_lightning_v2.h"
#include "ogl_texture.h"
#include "oglScene.h"
#include "oglRenderer.h"
#include "oglWindow.h"
#include "oglControls.h"



using namespace std;
	



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


		//camera
		MyCamera::PerspectiveCamera perspectiveCamera;
		perspectiveCamera.setPerspectiveProjection(persProjInfo);
		perspectiveCamera.setPosition(0, 0, 5);
		
		//Controls
		OglControls::OrbitalControls orbitalControls(&perspectiveCamera);	
	
		//lights

		MyOglLightV2::DirectionalLight dirLight1;
		dirLight1.shadow.castShadow = true;
		dirLight1.setColor(1, 1, 1);
		dirLight1.DiffuseIntensity = 3;
		dirLight1.AmbientIntensity = 0.2;
		dirLight1.setWorldDirection(0,-1,1);
		

		MyOglLightV2::PointLight pointLight1;
		pointLight1.setWorldPosition(0, 0, -1);
		pointLight1.setColor(1, 1, 1);
		pointLight1.AmbientIntensity = 0.5;
		pointLight1.DiffuseIntensity = 3;
		pointLight1.setLightAttenuation(1, 40, 0.12);


		MyOglLightV2::SpotLight spotLight1;
		spotLight1.setWorldPosition(1, 2, 0);
		spotLight1.setColor(1, 1, 1);
		spotLight1.AmbientIntensity = 0.5;
		spotLight1.DiffuseIntensity = 3;
		spotLight1.setLightAttenuation(1, 0.09, 0.032);
		spotLight1.setWorldDirection(-1, -1, 0);
		spotLight1.InnerCutoff = 10; //degree
		spotLight1.OuterCutoff = 15; //degree


		//Textures
		MyTextureV2::Texture brick(ENGINE_ASSETS_DIR "/images/brick1/map.png", GL_SRGB8_ALPHA8,
			GL_RGBA, GL_UNSIGNED_BYTE, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		MyTextureV2::Texture brickNormal(ENGINE_ASSETS_DIR "/images/brick1/normal.jpg");


		MyTextureV2::Texture ground(ENGINE_ASSETS_DIR "/images/ground1/map.png", GL_SRGB8_ALPHA8);
		MyTextureV2::Texture groundNormal(ENGINE_ASSETS_DIR "/images/ground1/normal.jpg");
		MyTextureV2::Texture groundDisplacement(ENGINE_ASSETS_DIR "/images/ground1/displacement.png"); //intensity is 0.1f

		//default meshes
		OglObject3D::MeshBasicMaterial material1;
		OglObject3D::MeshBasicMaterial material2;
		OglObject3D::MeshStandardMaterial material3;
		
		

		OglObject3D::PlaneGeometry plane1(0.5,0.5,100,100);
		material1.setColor(1, 0, 0, 1);
		OglObject3D::Mesh mesh(plane1, material1);
		mesh.material->setWireframe(false);
		mesh.position.x += 1;
		
		OglObject3D::PlaneGeometry plane2(0.5, 0.5, 10, 10);
		material2.setMap(brick);
		OglObject3D::Mesh mesh2(plane1, material2);
		mesh2.position.x = 1;
		mesh2.scale = 5;

		OglObject3D::BoxGeometry box1(0.2, 0.2, 0.2, 5, 5, 5);
		//material3.setMap("./ogldevTutorials/images/brick1/map.png");
		material3.setMap(brick);
		material3.setNormalMap(brickNormal);
		material3.shadow.castShadow = true;
		OglObject3D::Mesh mesh3(box1, material3);
		mesh3.material->setWireframe(false);
		mesh3.position.y = 0.2;
		mesh3.position.x = 0.8;


		OglObject3D::Mesh mesh4(plane1, material3);
		mesh4.position.y = 0;
		mesh4.position.x = -1;
		mesh4.rotation.x = 0;
		mesh4.scale = 1;

		//sphere
		OglObject3D::SphereGeometry sphereGeo(0.4, 16, 16);
		OglObject3D::Mesh sphere1(sphereGeo, material3);
		sphere1.position.y = 0.6;
		

		OglObject3D::PlaneGeometry terrain1(5, 5, 100, 100);
		OglObject3D::MeshStandardMaterial terrainMaterial1;
		terrainMaterial1.setMap(ground);
		terrainMaterial1.setNormalMap(groundNormal);
		terrainMaterial1.setDisplacementMap(groundDisplacement,0.1);
		terrainMaterial1.shadow.receiveShadow = true;
		OglObject3D::Mesh terrainMesh1(terrain1, terrainMaterial1);
		terrainMesh1.material->setWireframe(false);
		terrainMesh1.rotation.SetFromEuler(-90,0,0);

		//scene
		OglScene::Scene scene;
		
		scene.add(terrainMesh1);
		//scene.add(&mesh2);
		scene.add(mesh3);
		//scene.add(&mesh4);
		scene.add(dirLight1);
		scene.add(sphere1);
		//scene.add(spotLight1);



		//events

		OglWindow::Window::addEventListener(OglWindow::KeyDown, [&mesh3](int key, int action) {

			if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
				mesh3.rotation.x += 5;
			}

			});


		//gui
		//DAT::Gui gui(100,400,500);
		DAT::Gui gui(300);
		OglWindow::Window::AddGui(&gui);
		float intensity = 0.1;
		float minVariance = 0.00002;
		float bleedReduction = 0.2;
		/*
		gui.add("dispMap intensity", &intensity, 0.0f, 2.0f).onChange([&terrainMaterial1,&intensity]() {

			cout << intensity << endl;
			terrainMaterial1.setDisplacementMapIntensity(intensity);
			});;
			*/
	
		gui.add("dispMap intensity", &intensity, 0.0f, 2.0f).onChangeFinished([&terrainMesh1, &intensity]() {

			cout << intensity << endl;
			terrainMesh1.GetMaterial<OglObject3D::MeshStandardMaterial>()->setDisplacementMapIntensity(intensity);
			});;

		gui.add("dirLight x", &dirLight1.WorldDirection.x,-1.0f,1.0f);
		gui.add("dirLight y", &dirLight1.WorldDirection.y,-1.0f,1.0f);
		gui.add("dirLight z", &dirLight1.WorldDirection.z,-1.0f,1.0f) ;

		gui.add("boxPosition x", &mesh3.position.x, -10.0f, 10.0f);
		gui.add("boxPosition y", &mesh3.position.y, -10.0f, 10.0f);
		gui.add("boxPosition z", &mesh3.position.z, -10.0f, 10.0f);

		gui.add("minVariance", &minVariance, 0.0f, 0.0002f).onChange([&minVariance,&terrainMaterial1]() {

			terrainMaterial1.setUniform("minVariance", minVariance);

			});

		gui.add("bleedReduction", &bleedReduction, 0.0f, 1.0f).onChange([&bleedReduction, &terrainMaterial1]() {

			terrainMaterial1.setUniform("bleedReduction", bleedReduction);

			});

		bool dene3 = false;
		gui.add("checkbox1", &dene3).onChange([&dene3]() {

			cout << "dene3: " << dene3 << endl;

			});

		gui.add("dropdown1", { "A","B","C" }).onChange([](string choosed) {

			if (choosed == "A") {
				cout << "A is choosed" << endl;
			}
			else if (choosed == "B") {
				cout << "B is choosed" << endl;
			}
			else if (choosed == "C") {
				cout << "C is choosed" << endl;
			}

			});


		while (!glfwWindowShouldClose(OglWindow::Window::glfwWindow))
		{
			
			glfwPollEvents();


			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			glClearColor(0,0,0,1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		
	

			renderer.Render(scene, perspectiveCamera);

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



