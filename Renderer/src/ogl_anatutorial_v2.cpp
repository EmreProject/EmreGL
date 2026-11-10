#include "ogl_anatutorial_v2.h"


namespace OglDevTutorial_V2 {


	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void CursorPosCallback(GLFWwindow* window, double x, double y);
	void MouseButtonCallback(GLFWwindow* window, int Button, int Action, int Mode);
	void FrameBufferSizeCallback_(GLFWwindow* window, int width, int height);
	void ScrollCallback_(GLFWwindow* window, double xoffset, double yoffset);



	BaseTutorial* BaseTutorial::activeTutorial;



	void BaseTutorial::setBasicSettingGlfw() {

		if (glfwInit() != 1) {
			cout << "error initializing glfw" << endl;
			exit(1);
		}

		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

		window = glfwCreateWindow(windowWidth, windowHeight, "Tutorial", NULL, NULL);
		if (!window)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();

		}
		glfwMakeContextCurrent(window);

		glewExperimental = GL_TRUE;
		GLenum res = glewInit();
		if (res != GLEW_OK) {
			cout << ((const char*)glewGetErrorString(res)) << endl;
			exit(1);
		}


		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		//glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);

		//stbi_set_flip_vertically_on_load(true);
		glfwSwapInterval(1);

	}

	void BaseTutorial::initCallbacks() {

		//BaseTutorial::activeTutorial = this; //AKTÝF YAPAR!!!!CALLBACKLER BU CLASSA ÖZEL OLUR

		glfwSetKeyCallback(window, KeyCallback);
		glfwSetCursorPosCallback(window, CursorPosCallback);
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback_);
		glfwSetScrollCallback(window, ScrollCallback_);

	}
	void BaseTutorial::KeyboardCB(int key, int action) {

		camera->OnKeyboard(key);
		//ACTION KULLANMADIM BÜYÜK IHTIMAL GL_PRESS GÝBÝ EKLE BÝTTÝKTEN SONRA

	}
	void BaseTutorial::PassiveMouseCB(int x, int y) {

		this->mouseLastX = x;
		this->mouseLastY = y;
		camera->OnMouse(x, y);		
		orbitalCamera->updateDrag(x, y);
		
	}
	void BaseTutorial::FrameBufferSizeCallback(int width, int height) {

		glViewport(0, 0, width, height);
		windowWidth = width;
		windowHeight = height;

		
	}
	void BaseTutorial::MouseCB(int button, int action, int x, int y) {

		//cout << "mouse X: " << x << ", " << "mouse Y: " << y << endl;

		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (action == GLFW_PRESS) {
				orbitalCamera->isMouseLeftButton = true;
				std::cout << "Left mouse button pressed" << std::endl;
			}
			else if (action == GLFW_RELEASE) {
				orbitalCamera->isMouseLeftButton = false;
				std::cout << "Left mouse button released" << std::endl;
			}
		}
	}

	void BaseTutorial::ScrollCB(double xoffset, double yoffset) {

		if (cameraType == MyCamera::CameraType::OrbitalCamera) {

				
			//yakýnlasma
			if (yoffset == 1) {

				orbitalCamera->radius *= (1 - orbitalCamera->ratioZoom);
			}
			else if (yoffset == -1) { //uzaklasma

				orbitalCamera->radius *= (1 + orbitalCamera->ratioZoom);
			}


			

		}

	}

	void BaseTutorial::initCameraAndPerspectiveProjection() {

		MyVectorV3::Vector3f CameraPos(0.0f, 5.0f, -8.0f);
		MyVectorV3::Vector3f CameraTarget(0.0f, 0.0f, 1.0f);
		MyVectorV3::Vector3f CameraUp(0.0f, 1.0f, 0.0f);


		
		float FOV = 45.0f;
		float zNear = 0.1f;
		float zFar = 1000.0f;
		MyProjection::PersProjInfo persProjInfo = { FOV, (float)windowWidth, (float)windowHeight, zNear, zFar };


		orbitalCamera = new MyCamera::OrbitalCamera();
		orbitalCamera->setPerspectiveProjection(persProjInfo);

		camera = new MyCamera::BasicCamera(persProjInfo, CameraPos, CameraTarget, CameraUp);

	}

	void BaseTutorial::setCameraType() {

		this->cameraType = MyCamera::CameraType::BasicCamera;
		//this->cameraType = MyCamera::CameraType::OrbitalCamera;

	}

	void BaseTutorial::setCameraSettings() {

		orbitalCamera->deltaRadian = PI / 450;
		camera->SetSpeed(1);
	}

	MyMatrixV3::Matrix4f BaseTutorial::getViewandProjectionMatrix() {

		if (this->cameraType == MyCamera::CameraType::BasicCamera) {
			return camera->GetViewProjMatrix();
		}
		else if (this->cameraType == MyCamera::CameraType::OrbitalCamera) {

			return orbitalCamera->GetViewProjectionMatrix();
		}


	}




	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

		OglDevTutorial_V2::BaseTutorial::activeTutorial->KeyboardCB(key, action);

	}
	void CursorPosCallback(GLFWwindow* window, double x, double y) {

		OglDevTutorial_V2::BaseTutorial::activeTutorial->PassiveMouseCB((int)x, (int)y);
	}

	void FrameBufferSizeCallback_(GLFWwindow* window, int width, int height) {

		OglDevTutorial_V2::BaseTutorial::activeTutorial->FrameBufferSizeCallback(width, height);
	}

	void MouseButtonCallback(GLFWwindow* window, int Button, int Action, int Mode) {
		//DAHA YAPMADIM TUTORIAL 31
		double x, y;

		glfwGetCursorPos(window, &x, &y);

		OglDevTutorial_V2::BaseTutorial::activeTutorial->MouseCB(Button, Action, (int)x, (int)y);

	}
	void ScrollCallback_(GLFWwindow* window, double xoffset, double yoffset) {

		OglDevTutorial_V2::BaseTutorial::activeTutorial->ScrollCB(xoffset,yoffset);

	}


}





