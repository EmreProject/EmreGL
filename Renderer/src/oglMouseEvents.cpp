#include "oglMouseEvents.h"
#include "oglWindow.h"

namespace OglMouseEvents{

	

	MouseEventHandler::MouseEventHandler(): mouseRenderTarget(OglWindow::Window::width, OglWindow::Window::height){

		createRenderTarget();
		setMouseEvents();
	}
	MouseEventHandler::~MouseEventHandler() {

		for_each(this->mouseEventMeshDataList.begin(), this->mouseEventMeshDataList.end(), [](MouseEventMeshData& meshData) {
			delete meshData.mouseEventMesh;
			});

		mouseEventMeshDataList.clear();
	}

	void MouseEventHandler::createRenderTarget() {

		mouseRenderTarget.AddColorBuffer(&objectIdTexture, 1);
		mouseRenderTarget.SetDepthBuffer();//default one, i can also make my own
		mouseRenderTarget.Create();
	}

	void MouseEventHandler::setMouseEvents() {
		function<void(double, double)> mouseMove = [this](double x, double y) {

			y = OglWindow::Window::height - y;

			x = std::max(0, std::min((int)x, OglWindow::Window::width - 1));
			y = std::max(0, std::min((int)y, OglWindow::Window::height - 1));

			float pixel[4];

			glBindFramebuffer(GL_READ_FRAMEBUFFER, this->mouseRenderTarget.getFBO());
			glReadBuffer(GL_COLOR_ATTACHMENT0);

			glReadPixels(
				x,
				y,
				1,
				1,
				GL_RGBA,
				GL_FLOAT,
				pixel
			);

			this->currentMouseTouchingObjectId = pixel[0];
			//cout <<"mousemove id: "<< clickedObjectId << endl;
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

			};
		OglWindow::Window::addEventListener(OglWindow::MouseMove, mouseMove);


		function<void(int button, int action, int x, int y)> mouseClick = [this](int button, int action, int x, int y) {


			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			{
				y = OglWindow::Window::height - y;
				
				x = std::max(0, std::min(x, OglWindow::Window::width - 1));
				y = std::max(0, std::min(y, OglWindow::Window::height - 1));

				float pixel[4];

				glBindFramebuffer(GL_READ_FRAMEBUFFER, this->mouseRenderTarget.getFBO());
				glReadBuffer(GL_COLOR_ATTACHMENT0);

				glReadPixels(
					x,
					y,
					1,
					1,
					GL_RGBA,
					GL_FLOAT,
					pixel
				);

				this->clickedObjectId = pixel[0];
				cout << "clicked id: "<<clickedObjectId << endl;
				glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

			}
		};
		OglWindow::Window::addEventListener(OglWindow::MouseClick, mouseClick);

	}
	MouseEventHandler::MouseEventMeshData MouseEventHandler::createMouseEventMesh(OglObject3D::Mesh& mesh,float id) {

		OglObject3D::Mesh* mouseEventMesh = new OglObject3D::Mesh(mesh);
		

		OglObject3D::RawShaderMaterial* rawShaderMaterial = mesh.GetMaterial<OglObject3D::RawShaderMaterial>();
		OglObject3D::MeshBasicMaterial* basicMaterial = mesh.GetMaterial<OglObject3D::MeshBasicMaterial>();
		OglObject3D::MeshStandardMaterial* standardMaterial = mesh.GetMaterial<OglObject3D::MeshStandardMaterial>();


		MouseEventMeshData meshData;
		meshData.originalMesh = &mesh;
		meshData.mouseEventMesh = mouseEventMesh;
		meshData.id = id;

		string fragmentShader = R"(

			#version 430

			uniform float id;
			layout (location = 0) out vec4 idTexture;
			void main()
			{           

			  idTexture=vec4(id,id,id,id);
			}

		)";

		
		string vertexShader;

		if (rawShaderMaterial != nullptr) {
			vertexShader = *rawShaderMaterial->getVertexShader();
		}
		if (basicMaterial != nullptr) {
			vertexShader = *basicMaterial->vertexShaderText;
		}
		if (standardMaterial != nullptr) {
			vertexShader = *standardMaterial->vertexShaderText;
		}

		OglObject3D::RawShaderMaterial* newMat = new OglObject3D::RawShaderMaterial();
		newMat->setVertexShader(vertexShader);
		newMat->setFragmentShader(fragmentShader);
		//newMat->uniforms.clear();
		newMat->isDebugModeUniformsAddress = false;
		newMat->uniforms=mesh.material->uniforms;
		newMat->setUniform("id", id);
			
			mouseEventMesh->material = newMat;
			
			


		
		
		
		return meshData;

	}

	
	void MouseEventHandler::setMouseEventScene(OglScene::Scene* scene_) {


		this->isSceneSet = true;

		auto itMesh = scene_->meshes.begin();
		for(; itMesh != scene_->meshes.end(); itMesh++) {
			OglObject3D::Mesh* mesh = *itMesh;
			if (mesh->mouseEvents.isMouseClickEventFuncSet || mesh->mouseEvents.isMouseEnterEventFuncSet || mesh->mouseEvents.isMouseMoveEventFuncSet || mesh->mouseEvents.isMouseLeaveEventFuncSet) {

			MouseEventMeshData meshData = createMouseEventMesh(*mesh, (float)this->mouseEventMeshDataList.size() + 1.0f);
			this->mouseEventMeshDataList.insert(this->mouseEventMeshDataList.end(), meshData);
			this->mouseEventScene.add(meshData.mouseEventMesh);
			}
		}

	}

	bool MouseEventHandler::isMouseEventSceneSet() {

		return this->isSceneSet;
	}


	OglScene::Scene& MouseEventHandler::getScene() {

		return this->mouseEventScene;
	}

	OglRenderTarget::RenderTarget& MouseEventHandler::getRenderTarget(){
		return this->mouseRenderTarget;
		}


	MyTextureV2::Texture MouseEventHandler::getTexture() {

		return mouseRenderTarget.getColorTexture(0);
	}
	
	void MouseEventHandler::Update() {

		for_each(this->mouseEventMeshDataList.begin(), mouseEventMeshDataList.end(), [](MouseEventMeshData& meshData) {


			meshData.mouseEventMesh->matrix = meshData.originalMesh->matrix;
			meshData.mouseEventMesh->position = meshData.originalMesh->position;
			meshData.mouseEventMesh->scale = meshData.originalMesh->scale;
			meshData.mouseEventMesh->rotation = meshData.originalMesh->rotation;




			});


		function<MouseEventMeshData(int)> getMouseMeshData = [this](int id) {

			auto it = this->mouseEventMeshDataList.begin();
			int index = id - 1;

			for (int i = 0; i < index; i++) {
				it++;
			}

			return *it;

			};


		if (currentMouseTouchingObjectId > 0 && currentMouseTouchingObjectId == oldMouseTouchingObjectId && OglWindow::Window::isMouseMove) {

			MouseEventMeshData data = getMouseMeshData(currentMouseTouchingObjectId);


			if (data.originalMesh->mouseEvents.isMouseMoveEventFuncSet) {
				
				for(auto& func : data.originalMesh->mouseEvents.mouseMoveEventFunc) {
					func();
				}
			
			}
		}

		if (oldMouseTouchingObjectId > 0 && oldMouseTouchingObjectId != currentMouseTouchingObjectId) {

			MouseEventMeshData data = getMouseMeshData(oldMouseTouchingObjectId);

			if (data.originalMesh->mouseEvents.isMouseLeaveEventFuncSet) {
				for(auto& func : data.originalMesh->mouseEvents.mouseLeaveEventFunc) {
					func();
				}
				
		
			}
		}

		if (currentMouseTouchingObjectId > 0 && currentMouseTouchingObjectId != oldMouseTouchingObjectId) {

			MouseEventMeshData data = getMouseMeshData(currentMouseTouchingObjectId);

			if (data.originalMesh->mouseEvents.isMouseEnterEventFuncSet) {
				for (auto& func : data.originalMesh->mouseEvents.mouseEnterEventFunc) {
					func();
				}

			}
		}

		if (this->clickedObjectId > 0) {

			MouseEventMeshData data = getMouseMeshData(clickedObjectId);

			if (data.originalMesh->mouseEvents.isMouseClickEventFuncSet) {
				
				for(auto& func :  data.originalMesh->mouseEvents.mouseClickEventFunc) {
					func();
				}
				
			}
		}

		this->oldMouseTouchingObjectId = currentMouseTouchingObjectId;
		this->clickedObjectId = -1;

	}

}