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
	float zFar = 100.0f;

	MyProjection::PersProjInfo persProjInfo = {
		FOV, windowWidth, windowHeight, zNear, zFar };



	//window
	OglWindow::Window::CreateNewWindow(windowWidth, windowHeight);




	//renderer
	OglRenderer::Renderer renderer;
	renderer.shadowEnabled = true; //enable shadow map rendering


	//particle parameters
	struct {

		int count = 10000;
		int countWidth = sqrt(this->count);
		int countHeight = ceilf((float)this->count / (float)this->countWidth);
	} particles;



	function<float(float)> pixelToRenderSpace = [windowHeight](float pixel) {


		float pixelSize = 2 / windowHeight; //convert pixel to size in render space, 1 pixel = 2/minSize in render space
		return pixelSize * pixel; //convert pixel to render space
		};

	//particles geometry
	OglObject3D::BufferGeometry geometry;
	geometry.position.values = vector<float>(particles.count * 3, 400.0f);//400 frustum culling? var m� diye deneme
	geometry.texcoord.values = vector<float>(particles.count * 2, 0.0f);
	geometry.normal.values = vector<float>(particles.count * 3, 0.0f);

	OglObject3D::BufferAttribute idAttribute;
	idAttribute.shaderName = "id";
	idAttribute.count = 1;
	idAttribute.values.resize(particles.count);
	int n = 0;
	generate(idAttribute.values.begin(), idAttribute.values.end(), [n]()mutable {
		return n++; });
	geometry.setAttribute("id", idAttribute);


	//Textures
	MyTextureV2::Texture brick(ENGINE_ASSETS_DIR "/images/brick1/map.png", GL_SRGB8_ALPHA8,
		GL_RGBA, GL_UNSIGNED_BYTE, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
	MyTextureV2::Texture brickNormal(ENGINE_ASSETS_DIR "/images/brick1/normal.jpg");
	MyTextureV2::Texture brickDisplacement(ENGINE_ASSETS_DIR "/images/brick1/displacement.png");


	MyTextureV2::Texture ground(ENGINE_ASSETS_DIR "/images/ground1/map.png", GL_SRGB8_ALPHA8);
	MyTextureV2::Texture groundNormal(ENGINE_ASSETS_DIR "/images/ground1/normal.jpg");
	MyTextureV2::Texture groundDisplacement(ENGINE_ASSETS_DIR "/images/ground1/displacement.png");

	MyTextureV2::Texture particleTexture(ENGINE_ASSETS_DIR "/images/particles/transparent/light_01.png");
	particleTexture.wrapS = GL_CLAMP_TO_EDGE;
	particleTexture.wrapT = GL_CLAMP_TO_EDGE;


	//camera
	MyCamera::PerspectiveCamera perspectiveCamera;
	perspectiveCamera.setPerspectiveProjection(persProjInfo);
	perspectiveCamera.setPosition(0, 0, 5);

	//Controls
	OglControls::OrbitalControls orbitalControls(&perspectiveCamera);

	//ortho camera
	MyCamera::OrthographicCamera orthoCamera;
	orthoCamera.setOrthographicProjection(-1, 1, -1, 1, 0.1, 1);
	orthoCamera.setPosition(0, 0, 0.5);
	orthoCamera.lookAt(0, 0, 0);


	//scenes
	OglScene::Scene preScene;
	OglScene::Scene updateScene;
	OglScene::Scene finalScene;

	//lights
	MyOglLightV2::DirectionalLight dirLight1;
	dirLight1.setColor(1, 1, 1);
	dirLight1.DiffuseIntensity = 3;
	dirLight1.AmbientIntensity = 0.5;
	dirLight1.WorldDirection = MyVectorV3::Vector3f(0, -1, 5);
	dirLight1.shadow.castShadow = true;




	//extra meshes for testing
	OglObject3D::SphereGeometry sphereGeometry(1, 10, 10);
	OglObject3D::MeshStandardMaterial sphereMaterial;
	sphereMaterial.shadow.castShadow = true;
	sphereMaterial.setNormalMap(brickNormal);
	sphereMaterial.setMap(brick);
	sphereMaterial.setDisplacementMap(brickDisplacement, 0.1f);


	OglObject3D::PlaneGeometry planeGeometry(20, 20, 100, 100);
	OglObject3D::MeshStandardMaterial planeMaterial;
	planeMaterial.shadow.receiveShadow = true;
	planeMaterial.setNormalMap(groundNormal);
	planeMaterial.setMap(ground);
	planeMaterial.setDisplacementMap(groundDisplacement, 0.1f);


	OglObject3D::Mesh sphere1(sphereGeometry, sphereMaterial);
	OglObject3D::Mesh plane1(planeGeometry, planeMaterial);
	plane1.rotation.x = -90;
	plane1.position.y = -1.5f;



	//Init--------------------------------------------------------------------------------


	OglObject3D::RawShaderMaterial preMaterial;
	string preVertexShader = OglObject3D::RawShaderMaterial::LoadVertexShaderFromFile(SANDBOX_PATH "/05_mrt_example/shaders/init.vert");
	string preFragmentShader = OglObject3D::RawShaderMaterial::LoadFragmentShaderFromFile(SANDBOX_PATH "/05_mrt_example/shaders/init.frag");
	preMaterial.setVertexShader(preVertexShader);
	preMaterial.setFragmentShader(preFragmentShader);


	preMaterial.setUniform("uTextureWidth", (float)particles.countWidth);
	preMaterial.setUniform("uTextureHeight", (float)particles.countHeight);
	preMaterial.setUniform("uParticleCount", (float)particles.count);
	preMaterial.setUniform("uRadius", 1.0f);
	preMaterial.setUniform("uStartVelocityMagnitude", pixelToRenderSpace(1));
	preMaterial.setUniform("uStartVelocityRotateRadian", PI / 4.0f);

	OglObject3D::Points preMesh(geometry, preMaterial);
	preMesh.position = MyVectorV3::Vector3f(0, -1, 0);
	preMesh.scale = 3;

	preScene.add(preMesh);//also &preMesh works no difference



	OglRenderTarget::DataTexture2DAttachment dataTextureAttachment;
	dataTextureAttachment.magFilter = GL_NEAREST;
	dataTextureAttachment.minFilter = GL_NEAREST;
	OglRenderTarget::RenderTarget initRenderTarget(particles.countWidth, particles.countHeight);
	initRenderTarget.AddColorBuffer(&dataTextureAttachment, 3);
	initRenderTarget.SetDepthBuffer();//default one, i can also make my own
	cout << "initRenderTarget status: " << initRenderTarget.checkStatus() << endl;
	cout << "initRenderTarget FBO: " << initRenderTarget.getFBO() << endl;
	initRenderTarget.Create();


	//Update

	OglObject3D::PlaneGeometry updatePlane(2, 2);

	OglObject3D::RawShaderMaterial updateMaterial;
	string updateVertexShader = OglObject3D::RawShaderMaterial::LoadVertexShaderFromFile(SANDBOX_PATH "/05_mrt_example/shaders/update.vert");
	string updateFragmentShader = OglObject3D::RawShaderMaterial::LoadFragmentShaderFromFile(SANDBOX_PATH "/05_mrt_example/shaders/update.frag");
	updateMaterial.setVertexShader(updateVertexShader);
	updateMaterial.setFragmentShader(updateFragmentShader);
	OglObject3D::Mesh updateMesh(updatePlane, updateMaterial);

	updateMesh.material->setUniform("uCurrentPosition", nullptr);
	updateMesh.material->setUniform("uCurrentVelocity", nullptr);
	updateMesh.material->setUniform("uTargetPosition", nullptr);

	updateMesh.material->setUniform("uMaxForce", pixelToRenderSpace(0.2));
	updateMesh.material->setUniform("uMaxSpeed", pixelToRenderSpace(8));
	updateMesh.material->setUniform("uRange", pixelToRenderSpace(60));

	updateScene.add(updateMesh);
	updateScene.add(updateMesh);//since same mesh it is not added, if i want
	//to add i have to create new mesh with copy constructor or operator=
	//to updateMesh, or new mesh with same geometry and material


	OglRenderTarget::RenderTarget updateRenderTargetA(particles.countWidth, particles.countHeight);
	updateRenderTargetA.AddColorBuffer(&dataTextureAttachment, 2);
	updateRenderTargetA.SetDepthBuffer();//default one, i can also make my own
	cout << "updateRenderTargetA status: " << updateRenderTargetA.checkStatus() << endl;
	cout << "updateRenderTargetA FBO: " << updateRenderTargetA.getFBO() << endl;

	OglRenderTarget::RenderTarget updateRenderTargetB(particles.countWidth, particles.countHeight);
	updateRenderTargetB.AddColorBuffer(&dataTextureAttachment, 2);
	updateRenderTargetB.SetDepthBuffer();//default one, i can also make my own
	cout << "updateRenderTargetB status: " << updateRenderTargetB.checkStatus() << endl;
	cout << "updateRenderTargetB FBO: " << updateRenderTargetB.getFBO() << endl;

	updateRenderTargetA.Create();
	updateRenderTargetB.Create();

	//Final------------------------------------------------------------



	OglObject3D::RawShaderMaterial finalMaterial;
	finalMaterial.transparent = true;


	string finalVertexShader = OglObject3D::RawShaderMaterial::LoadVertexShaderFromFile(SANDBOX_PATH "/05_mrt_example/shaders/final.vert");
	string finalFragmentShader = OglObject3D::RawShaderMaterial::LoadFragmentShaderFromFile(SANDBOX_PATH "/05_mrt_example/shaders/final.frag");
	finalMaterial.setVertexShader(finalVertexShader);
	finalMaterial.setFragmentShader(finalFragmentShader);
	finalMaterial.setUniform("uColor", MyVectorV3::Vector4f(0, 1, 0, 0.3));
	finalMaterial.setUniform("uPositionData", nullptr);
	finalMaterial.setUniform("uTextureWidth", (float)particles.countWidth);
	finalMaterial.setUniform("uTextureHeight", (float)particles.countHeight);
	finalMaterial.setUniform("uTextureParticle", particleTexture);

	OglObject3D::Points finalMesh(geometry, finalMaterial);
	finalMesh.material->setUniform("uColor", MyVectorV3::Vector4f(0, 0, 1, 1.));

	finalScene.add(finalMesh);



	OglObject3D::Points finalMesh2(finalMesh);
	finalMesh2.position.y = 1;

	finalScene.add(finalMesh2);

	OglObject3D::Points finalMesh3;
	finalMesh3 = finalMesh;
	finalMesh3.position.x = -1;
	finalScene.add(finalMesh3);


	OglObject3D::Points finalMesh4(geometry, finalMaterial);
	finalMesh4.position.x = 1;

	finalScene.add(finalMesh4);
	finalScene.add(sphere1);
	finalScene.add(plane1);
	finalScene.add(dirLight1);


	//renderer.setRenderTarget(0);
	//material2.setUniform("positionData", initRenderTarget.getColorTexture(0), 0, GL_TEXTURE0);


	bool isFirstRender = true;
	string currentFilledUpdateRenderTarget = "A";
	while (!glfwWindowShouldClose(OglWindow::Window::glfwWindow))
	{

		glfwPollEvents();


		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




		if (isFirstRender) {
			renderer.setRenderTarget(&initRenderTarget);

			renderer.Render(preScene, orthoCamera);

			renderer.setRenderTarget(&updateRenderTargetA);
			updateMesh.material->setUniform("uCurrentPosition", initRenderTarget.getColorTexture(0));
			updateMesh.material->setUniform("uCurrentVelocity", initRenderTarget.getColorTexture(1));
			updateMesh.material->setUniform("uTargetPosition", initRenderTarget.getColorTexture(2));

			renderer.Render(updateScene, orthoCamera);

			finalMesh.material->setUniform("uPositionData", updateRenderTargetA.getColorTexture(0));
			finalMesh2.material->setUniform("uPositionData", updateRenderTargetA.getColorTexture(0));
			finalMesh3.material->setUniform("uPositionData", updateRenderTargetA.getColorTexture(0));
			finalMesh4.material->setUniform("uPositionData", updateRenderTargetA.getColorTexture(0));



			isFirstRender = false;
		}
		else {

			if (currentFilledUpdateRenderTarget == "A") {
				renderer.setRenderTarget(&updateRenderTargetB);
				updateMesh.material->setUniform("uCurrentPosition", updateRenderTargetA.getColorTexture(0));
				updateMesh.material->setUniform("uCurrentVelocity", updateRenderTargetA.getColorTexture(1));
				updateMesh.material->setUniform("uTargetPosition", initRenderTarget.getColorTexture(2));

				renderer.Render(updateScene, orthoCamera);


				finalMesh.material->setUniform("uPositionData", updateRenderTargetB.getColorTexture(0));

				currentFilledUpdateRenderTarget = "B";
			}
			else if (currentFilledUpdateRenderTarget == "B") {
				renderer.setRenderTarget(&updateRenderTargetA);
				updateMesh.material->setUniform("uCurrentPosition", updateRenderTargetB.getColorTexture(0));
				updateMesh.material->setUniform("uCurrentVelocity", updateRenderTargetB.getColorTexture(1));
				updateMesh.material->setUniform("uTargetPosition", initRenderTarget.getColorTexture(2));
				//cout << MyTextureV2::Texture::AvailableTextureUnits.size() << endl;
				renderer.Render(updateScene, orthoCamera);

				finalMesh.material->setUniform("uPositionData", updateRenderTargetA.getColorTexture(0));
				finalMesh2.material->setUniform("uPositionData", updateRenderTargetA.getColorTexture(0));
				finalMesh3.material->setUniform("uPositionData", updateRenderTargetA.getColorTexture(0));
				finalMesh4.material->setUniform("uPositionData", updateRenderTargetA.getColorTexture(0));

				currentFilledUpdateRenderTarget = "A";
			}


		}


		renderer.setRenderTarget(0);
		renderer.Render(finalScene, perspectiveCamera);
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
