#include "oglWindow.h"


namespace OglWindow {


	double Window::previousX = -1;
	double Window::previousY = -1;
	bool Window::isMouseMove = false;

	//EVENTs

	void KeyDown(function<void(int key, int action)> keyFunc) {

		Window::keyDownFuncs.insert(Window::keyDownFuncs.end(), keyFunc);

	}
	void MouseMove(function<void(double x, double y)> mouseMoveFunc) {

		Window::mouseMoveFunctions.insert(Window::mouseMoveFunctions.end(), mouseMoveFunc);

	}
	void MouseClick(function<void(int button, int action, int x, int y)> mouseClickFunc) {

		Window::mouseClickFunctions.insert(Window::mouseClickFunctions.end(), mouseClickFunc);

	}

	void Resize(function<void(int width, int height)> resizeFunc) {

		Window::resizeFunctions.insert(Window::resizeFunctions.end(), resizeFunc);

	}

	void Scroll(function<void(double xoffset, double yoffset)> scrollFunc) {

		Window::scrollFunctions.insert(Window::scrollFunctions.end(), scrollFunc);
	}

	//static members
	 list<function<void(int key, int action)>> Window::keyDownFuncs;
	 list<function<void(double x, double y)>> Window::mouseMoveFunctions;
	 list<function<void(int button, int action, int x, int y)>> Window::mouseClickFunctions;
	 list<function<void(int width, int height)>> Window::resizeFunctions;
	 list<function<void(double xoffset, double yoffset)>> Window::scrollFunctions;



	 GLFWwindow* Window::glfwWindow;
	 list<DAT::Gui*> Window::guiElements;
	 bool Window::isOnGuiElement=false;


	 int Window::width;
	 int Window::height;


	void Window::InitGLFWEventHandler() {

		glfwSetKeyCallback(Window::glfwWindow, Window::KeyCallback);
		glfwSetCursorPosCallback(Window::glfwWindow, Window::CursorPosCallback);
		glfwSetMouseButtonCallback(Window::glfwWindow, Window::MouseButtonCallback);
		glfwSetFramebufferSizeCallback(Window::glfwWindow, Window::FrameBufferSizeCallback_);
		glfwSetScrollCallback(Window::glfwWindow, Window::ScrollCallback_);

	}

	void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

		auto it = Window::keyDownFuncs.begin();

		for (; it != Window::keyDownFuncs.end(); it++) {

			(*it)(key, action);
		}

	}
	void Window::CursorPosCallback(GLFWwindow* window, double x, double y){

		if (Window::getPreviousMouseX() != x || Window::getPreviousMouseY() != y) {
			Window::isMouseMove = true;
		auto it = Window::mouseMoveFunctions.begin();

		for (; it != Window::mouseMoveFunctions.end(); it++) {

			(*it)(x,y);
		}

		}
		
		//cout << "current: " << x << ", " << y << " - previous: " << Window::getPreviousMouseX() << ", "<<Window::getPreviousMouseY() << endl;
		Window::previousX = x;
		Window::previousY = y;

		}
	void Window::MouseButtonCallback(GLFWwindow* window, int Button, int Action, int Mode){
		
		auto it = Window::mouseClickFunctions.begin();

		double x, y;

		glfwGetCursorPos(window, &x, &y);

		for (; it != Window::mouseClickFunctions.end(); it++) {

			(*it)(Button,Action, (int)(x), (int)(y));
		}

		}
	void Window::FrameBufferSizeCallback_(GLFWwindow* window, int width, int height){

		auto it = Window::resizeFunctions.begin();

		for (; it != Window::resizeFunctions.end(); it++) {

			(*it)(width, height);
		}

		}
	void Window::ScrollCallback_(GLFWwindow* window, double xoffset, double yoffset){
		
		auto it = Window::scrollFunctions.begin();

		for (; it != Window::scrollFunctions.end(); it++) {

			(*it)(xoffset,yoffset);
		}

		}

	double Window::getPreviousMouseX() {

		return previousX;
	 }
	double Window::getPreviousMouseY() {

		return previousY;
	 }

	
	void Window::AddGui(DAT::Gui* gui_) {
		
		int width, height;
		glfwGetWindowSize(Window::glfwWindow, &width, &height);
		gui_->setWindowSize(width, height);

		if (guiElements.size() == 0) {

			Window::addEventListener(MouseMove, [](double x, double y) {

				auto it = Window::guiElements.begin();

				Window::isOnGuiElement = false;
				for (; it != Window::guiElements.end(); it++) {


					float guiX, guiY, guiWidth, guiHeight;
					(*it)->GetSizeandPosition(&guiX, &guiY, &guiWidth, &guiHeight);
					
					if (x > guiX && x < guiX + guiWidth && y>guiY && y < guiY + guiHeight) {

						Window::isOnGuiElement = true;
					}
				}


				});
		}

		Window::guiElements.push_back(gui_);

	}


	void Window::CreateNewWindow(int width, int height) {


		Window::width = width;
		Window::height = height;

		if (glfwInit() != 1) {
			cout << "error initializing glfw" << endl;
			exit(1);
		}

		//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		const char* glsl_version = "#version 430";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // core profile
		// Optional: forward-compatible (for macOS)
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
		
		Window::glfwWindow = glfwCreateWindow(Window::width, Window::height, "Tutorial", NULL, NULL);
		if (!Window::glfwWindow)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();

		}
		glfwMakeContextCurrent(Window::glfwWindow);

		glewExperimental = GL_TRUE;
		GLenum res = glewInit();
		if (res != GLEW_OK) {
			cout << ((const char*)glewGetErrorString(res)) << endl;
			exit(1);

		}


		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	
		glDebugMessageCallback(
			[](GLenum src, GLenum type, GLuint id,
				GLenum severity, GLsizei, const GLchar* msg, const void*)
			{
				if (type == GL_DEBUG_TYPE_ERROR)
					std::cerr << " OpenGL error (" << std::hex << id << "): "
					<< msg << std::endl;
			}, nullptr);
		glEnable(GL_DEPTH_TEST);
		//glDebugMessageCallback(glDebugOutput, nullptr);
		//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);



		//stbi_set_flip_vertically_on_load(true);
		glfwSwapInterval(1);
		//glDisable(GL_CULL_FACE);

		
		Window::InitGLFWEventHandler();


		function<void(int, int)> frameBufferSizeCallback = [](int width, int height) {

			glViewport(0, 0, width, height);
			Window::width = width;
			Window::height = height;
		};
		Window::resizeFunctions.insert(Window::resizeFunctions.end(), frameBufferSizeCallback);


		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport / Platform Windows
		io.ConfigViewportsNoAutoMerge = true;
		io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(Window::glfwWindow, true);
		ImGui_ImplOpenGL3_Init(glsl_version);


		const GLubyte* version = glGetString(GL_VERSION);
		std::cout << "OpenGL version: " << version << std::endl;




	}



}