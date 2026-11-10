#ifndef ogl_window_h
#define ogl_window_h


#include <GL/glew.h>
#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Projection.h"
#include "ogl_Camera.h"
#include "ogl_lightning_v2.h"
#include "ogl_model_v2.h"
#include "ogl_shader_v2.h"
#include "ogl_texture.h"
#include "ogl_World.h"
#include "ogl_default_meshes.h"
#include "oglMeshes.h"
#include "imgui_dat.h"



using namespace std;


namespace OglWindow {

	class Window {

	private:

		static void InitGLFWEventHandler();

		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void CursorPosCallback(GLFWwindow* window, double x, double y);
		static void MouseButtonCallback(GLFWwindow* window, int Button, int Action, int Mode);
		static void FrameBufferSizeCallback_(GLFWwindow* window, int width, int height);
		static void ScrollCallback_(GLFWwindow* window, double xoffset, double yoffset);

	
		static double previousX, previousY;
		

	public:

	
		//dont adjust outside these are for addeventlstener functions below the class
		static list<function<void(int key,int action)>> keyDownFuncs;
		static list<function<void( double x, double y)>> mouseMoveFunctions;
		static list<function<void(int button, int action, int x, int y)>> mouseClickFunctions;
		static list<function<void(int width, int height)>> resizeFunctions;
		static list<function<void(double xoffset, double yoffset)>> scrollFunctions;


		static GLFWwindow* glfwWindow;
		static list<DAT::Gui*> guiElements;
		static bool isOnGuiElement;


		static int width;
		static int height;


		static double getPreviousMouseX();
		static double getPreviousMouseY();
		static bool isMouseMove;


		static void CreateNewWindow(int width,int height);
		static void AddGui(DAT::Gui*);
		
		template<typename t,typename t2>
		requires is_convertible_v<t2,t>
		static void addEventListener(void(*eventName)(t), t2 eventFunc) {
			eventName(eventFunc);
		}

		
	};


	//addeventlistener ilk parametre
	void KeyDown(function<void( int key, int action)> keyFunc);
	void MouseMove(function<void( double x, double y)> mouseMoveFunc);
	void MouseClick(function<void( int button, int action, int x, int y)> mouseClickFunc);
	void Resize(function<void(int width, int height)> resizeFunc);
	void Scroll(function<void(double xoffset, double yoffset)> scrollFunc);


}



#endif