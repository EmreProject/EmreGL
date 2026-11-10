#ifndef ogldevanatut_v2_h
#define ogldevanatut_v2_h


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





using namespace std;



namespace OglDevTutorial_V2 {




	class BaseTutorial {

	protected:

		MyMatrixV3::Matrix4f getViewandProjectionMatrix();

		int mouseLastX = 0;
		int mouseLastY = 0;


	public:
		
		static BaseTutorial* activeTutorial; //for setting callback functions

		BaseTutorial() {}

		//main window
		GLFWwindow* window;


		//general properties
		int windowWidth = 1500;//degistimrek istersen inherit ettigin tutorialda degis
		int windowHeight = 900;
		MyVectorV3::Vector4f rgba_background;

		//all cameras, use the one you want
		MyCamera::CameraType cameraType = MyCamera::CameraType::BasicCamera;
		MyCamera::BasicCamera* camera;
		MyCamera::OrbitalCamera* orbitalCamera;


		//!!!inherit ettigin class icinde ogl_shaders.h icindeki shaderdan ekle!!!!



		//callbacks BTTÝ(SADECE MOUSECB TUTORIAL31 ICIN) default olabilir
		virtual void KeyboardCB(int key, int action);
		virtual void PassiveMouseCB(int x, int y); 
		virtual void MouseCB(int button, int action, int x, int y);//sonra
		virtual void FrameBufferSizeCallback(int width, int height);
		virtual void ScrollCB(double xoffset, double yoffset);

		//main events to be called in order THIS IS THE ORDER
		virtual void setBasicSettingGlfw(); //default kullanabilirsin
		virtual void setCameraType();
		virtual void setCameraSettings();
		virtual void initCameraAndPerspectiveProjection(); //default olabilir
		virtual void initCallbacks(); //default olabilir
		virtual void setShaderUniformsLocation() = 0;
		virtual void RenderSceneGlfw() = 0; //o tutoriala özel



	};




}



#endif