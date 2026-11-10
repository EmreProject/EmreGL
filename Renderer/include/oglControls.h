#ifndef ogl_controls_h
#define ogl_controls_h


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
#include "oglScene.h"
#include "oglRenderer.h"
#include "oglWindow.h"
#include "oglRenderTarget.h"



namespace OglControls {


	class Controls {

	protected:


		MyCamera::Camera* camera = nullptr; //ortho veya perspective olan


	public:

	
		Controls(MyCamera::Camera* cam) :camera(cam) {}
		
		virtual void resetDefaultSettings() = 0;
		virtual void Update() = 0;
	};



	class OrbitalControls : public Controls {


	public:

		struct SphericalCoordinate {
			float radius;
			float theta; //from z to x, 0 means 0,0,1 
			float phi; //from y to horizontal plane, 0 means 0,1,0 full perpendicular
		};;


	private:

	

		//for converting angle coordinate to cartesian position vector
		MyVectorV3::Vector3f convertSphericalToCartesianCoordinate(float radius, float theta, 
			float phi, MyVectorV3::Vector3f center);
		OrbitalControls::SphericalCoordinate convertCartesianToSphericalCoordinate(MyVectorV3::Vector3f camPosition,
			MyVectorV3::Vector3f center);

		struct RadianLimit {

			float MinRadian, MaxRadian;
			bool isActive = false;
		};

		void updateDrag(float x, float y); //update theta according the
		//mouse move when mouse is clicked


		MyVectorV3::Vector3f center;
		bool isUpdate = false;
		float dragLastX = -1, dragLastY = -1;
		bool isMouseLeftButton = false;
		

	public:

		 
		//modifiable settings
		float deltaRadian;
		RadianLimit verticalLimit;
		RadianLimit horizontalLimit;
		float ratioZoom = 0.1;
		bool isEnabled = true;


		OrbitalControls(MyCamera::Camera* cam);

		void resetDefaultSettings();

		void setCenterPoint(float x, float y, float z);
		void setCenterPoint(MyVectorV3::Vector3f center_);

		MyVectorV3::Vector3f getCenterPoint();

		void Update();//in each animation cycle

	};
} 

#endif // !ogl_controls_h
