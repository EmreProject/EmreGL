#ifndef my_camera_h
#define my_camera_h



#include <GL/glew.h>
#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Projection.h"
#include <math.h>

using namespace std;

namespace MyCamera {



	class BasicCamera 
    {
    public:

        BasicCamera() {}

        BasicCamera(int WindowWidth, int WindowHeight);

        BasicCamera(const MyProjection::PersProjInfo& persProjInfo, const MyVectorV3::Vector3f& Pos, const MyVectorV3::Vector3f& Target, const MyVectorV3::Vector3f& Up);

        BasicCamera(const MyProjection::OrthoProjInfo& orthoProjInfo, const MyVectorV3::Vector3f& Pos, const MyVectorV3::Vector3f& Target, const MyVectorV3::Vector3f& Up);

        void InitCamera(const MyProjection::PersProjInfo& persProjInfo, const MyVectorV3::Vector3f& Pos, const MyVectorV3::Vector3f& Target, const MyVectorV3::Vector3f& Up);

        void InitCamera(const MyProjection::OrthoProjInfo& orthoProjInfo, const MyVectorV3::Vector3f& Pos, const MyVectorV3::Vector3f& Target, const MyVectorV3::Vector3f& Up);

        void SetPosition(float x, float y, float z);

        void SetPosition(const MyVectorV3::Vector3f& pos);

        void SetTarget(float x, float y, float z);

        void SetTarget(const MyVectorV3::Vector3f& target);

        void SetUp(float x, float y, float z) { m_up.x = x; m_up.y = y; m_up.z = z; }

        bool OnKeyboard(int key);

        void OnMouse(int x, int y);
        void OnMouse(int x, int y, int offsetLeft, int offsetRight, int offsetTop, int offsetBottom,int winWidth,int winHeight);

        // Update the mouse position without moving the camera
        void UpdateMousePosSilent(int x, int y);

        void OnRender();

        MyMatrixV3::Matrix4f GetMatrix() const;

        const MyVectorV3::Vector3f& GetPos() const { return m_pos; }

        const MyVectorV3::Vector3f& GetTarget() const { return m_target; }

        const MyVectorV3::Vector3f& GetUp() const { return m_up; }

        const MyMatrixV3::Matrix4f& GetProjectionMat() const { return m_projection; }

        const MyProjection::PersProjInfo& GetPersProjInfo() const { return m_persProjInfo; }

        MyMatrixV3::Matrix4f GetViewProjMatrix() const;

        MyMatrixV3::Matrix4f GetViewMatrix() const { return GetMatrix(); }

        MyMatrixV3::Matrix4f GetViewportMatrix() const;

        void Print() const { printf("Pos "); m_pos.Print(); printf("Target "); m_target.Print(); }

        void SetSpeed(float Speed);

        void SetName(const std::string& Name) { m_name = Name; }

        const std::string& GetName() const { return m_name; }


        MyVectorV3::Vector2i m_mousePos;

        void setOffsetScreen(int xleft, int xright, int ytop, int ybottom);
        struct {
            int left=0, right=0, top=0, bottom=0;
            float verticalAngle = 0;
            float horizontalAngle = 0;
        }offset;
        float maxAngleH = 120, maxAngleV=120;
        float lastAngleH = 0, lastAngleV = 0;


    private:

        void InitInternal();
        void Update();

        std::string m_name;

        MyVectorV3::Vector3f m_pos;
        MyVectorV3::Vector3f m_target;
        MyVectorV3::Vector3f m_up;

        float m_speed = 5.0f;
        int m_windowWidth;
        int m_windowHeight;

        float m_AngleH;
        float m_AngleV;


        bool m_OnUpperEdge;
        bool m_OnLowerEdge;
        bool m_OnLeftEdge;
        bool m_OnRightEdge;

      

        MyProjection::PersProjInfo m_persProjInfo;
        MyMatrixV3::Matrix4f m_projection;
    };


    #define PI 3.14159265358979323846f
    //---https://andreasrohner.at/posts/Web%20Development/JavaScript/Simple-orbital-camera-controls-for-THREE-js/
    class OrbitalCamera  {

    private:

        //variables
        MyVectorV3::Vector3f m_pos;
        MyVectorV3::Vector3f m_target;
        MyVectorV3::Vector3f m_up;


        
        int m_windowWidth;
        int m_windowHeight;

        MyProjection::PersProjInfo m_persProjInfo;
        MyMatrixV3::Matrix4f m_projection;
     

        //private functions
        void setOrbitalCamera(float radius, float startPhi,
            float startTheta, float deltaRadian);

        //for converting angle coordinate to cartesian position vector
        MyVectorV3::Vector3f convertSphericalToCartesianCoordinate();

        struct RadianLimit {

        float MinRadian, MaxRadian;
        bool isActive = false;
        };

    public:

        MyVectorV3::Vector3f center;
        float radius;
        float deltaRadian;
        RadianLimit verticalLimit;
        RadianLimit horizontalLimit;
        float phi; //from y to horizontal plane, 0 means 0,1,0 full perpendicular
        float theta; //from z to x, 0 means 0,0,1 
        float dragLastX = -1, dragLastY = -1;
        float ratioZoom = 0.1;
        

        bool isMouseLeftButton = false;



        OrbitalCamera();
        OrbitalCamera(float radius, float startPhi,
            float startTheta, float deltaRadian);


         MyVectorV3::Vector3f& GetPos(){ return m_pos; }
         MyVectorV3::Vector3f& GetTarget(){ return m_target; }
         MyVectorV3::Vector3f& GetUp(){ return m_up; }

        MyMatrixV3::Matrix4f GetViewMatrix();
        MyMatrixV3::Matrix4f GetProjectionMatrix();
        MyMatrixV3::Matrix4f GetViewProjectionMatrix();

        void setCenterPoint(float x, float y, float z);
        void setCenterPoint(MyVectorV3::Vector3f center_);
        void setPerspectiveProjection(MyProjection::PersProjInfo& persProjInfo);
       
        void updateDrag(float x, float y); //update theta according the
        //mouse move when mouse is clicked. It should be in glfw mouse
        //event handler
        void updateCamera(); //main update function of camera view matrix

    };


    enum  class CameraType {
        BasicCamera, OrbitalCamera
    };



	//Modern OpenGL camera, later delete the up basic and orbital camera and use this ones
	//plus the camera controller in three js

    class Camera { //abstract class dont use it directly

    protected:


        MyMatrixV3::Matrix4f projectionMatrix;
        MyMatrixV3::Matrix4f viewMatrix;


        MyVectorV3::Vector3f position = MyVectorV3::Vector3f(0,0,1);
        MyVectorV3::Vector3f target= MyVectorV3::Vector3f(0, 0, -1);;//use target as direction vector 
        MyVectorV3::Vector3f up = MyVectorV3::Vector3f(0, 1, 0);

    public:

        Camera() {}

        MyVectorV3::Vector3f lookAt(MyVectorV3::Vector3f point);
		MyVectorV3::Vector3f lookAt(float x, float y, float z);

        void setPosition(float x, float y, float z);
        void setPosition(MyVectorV3::Vector3f position);
        void setDirection(float x, float y, float z);
        void setDirection(MyVectorV3::Vector3f dir);

        MyVectorV3::Vector3f GetPosition() { return position; }
        MyVectorV3::Vector3f GetDirection() { return target; }


        MyMatrixV3::Matrix4f GetViewMatrix();
        MyMatrixV3::Matrix4f GetProjectionMatrix();
		MyMatrixV3::Matrix4f GetViewProjectionMatrix();



        virtual void info() = 0;
    
   };

    class OrthographicCamera :public Camera {

    private:



    public:

		OrthographicCamera():Camera(){}
        OrthographicCamera(MyProjection::OrthoProjInfo orthographic);
        OrthographicCamera(float left, float right,
            float bottom, float top, float near_, float far_);
        OrthographicCamera(MyProjection::OrthoProjInfo orthographic, MyVectorV3::Vector3f position,
            MyVectorV3::Vector3f dir);

      

		void setOrthographicProjection(MyProjection::OrthoProjInfo orthographic);
		void setOrthographicProjection(float left, float right,
            float bottom, float top, float near_, float far_);

        
		void info() { cout << "Orthographic camera" << endl; }

    };


    class PerspectiveCamera :public Camera {


    private:


    public:


		PerspectiveCamera() :Camera() {}
		PerspectiveCamera(MyProjection::PersProjInfo perspective);
        PerspectiveCamera(float fov, float width, float height, float near_, float far_);
		PerspectiveCamera(MyProjection::PersProjInfo perspective, MyVectorV3::Vector3f position,
			MyVectorV3::Vector3f dir);


		void setPerspectiveProjection(MyProjection::PersProjInfo perspective);
		void setPerspectiveProjection(float fov, float width, float height,
			float near_, float far_);


		void info() { cout << "Perspective camera" << endl; }


    };


  
}






#endif