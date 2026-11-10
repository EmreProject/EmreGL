
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ogl_Camera.h"
#include <iostream>


namespace MyCamera {


    static int MARGIN = 40;
    static float EDGE_STEP = 0.5f;

    BasicCamera::BasicCamera(int WindowWidth, int WindowHeight)
    {
        m_windowWidth = WindowWidth;
        m_windowHeight = WindowHeight;
        m_pos = MyVectorV3::Vector3f(0.0f, 0.0f, 0.0f);
        m_target = MyVectorV3::Vector3f(0.0f, 0.0f, 1.0f);
        m_up = MyVectorV3::Vector3f(0.0f, 1.0f, 0.0f);

        InitInternal();
    }


    BasicCamera::BasicCamera(const MyProjection::PersProjInfo& persProjInfo, const MyVectorV3::Vector3f& Pos, const MyVectorV3::Vector3f& Target, const MyVectorV3::Vector3f& Up)
    {
        InitCamera(persProjInfo, Pos, Target, Up);
    }


    BasicCamera::BasicCamera(const MyProjection::OrthoProjInfo& orthoProjInfo, const MyVectorV3::Vector3f& Pos, const MyVectorV3::Vector3f& Target, const MyVectorV3::Vector3f& Up)
    {
        InitCamera(orthoProjInfo, Pos, Target, Up);
    }


    void BasicCamera::InitCamera(const MyProjection::PersProjInfo& persProjInfo, const MyVectorV3::Vector3f& Pos, const MyVectorV3::Vector3f& Target, const MyVectorV3::Vector3f& Up)
    {
        m_persProjInfo = persProjInfo;
        m_projection.InitPersProjTransform(persProjInfo);
        m_windowWidth = (int)persProjInfo.Width;
        m_windowHeight = (int)persProjInfo.Height;
        m_pos = Pos;

        m_target = Target;
        m_target.Normalize();

        m_up = Up;
        m_up.Normalize();

        InitInternal();
    }


    void BasicCamera::InitCamera(const MyProjection::OrthoProjInfo& orthoProjInfo, const MyVectorV3::Vector3f& Pos, const MyVectorV3::Vector3f& Target, const MyVectorV3::Vector3f& Up)
    {
        m_projection.InitOrthoProjTransform(orthoProjInfo);
        m_windowWidth = (int)orthoProjInfo.Width;
        m_windowHeight = (int)orthoProjInfo.Height;
        m_pos = Pos;

        m_target = Target;
        m_target.Normalize();

        m_up = Up;
        m_up.Normalize();

        InitInternal();
    }


    void BasicCamera::InitInternal()
    {
        MyVectorV3::Vector3f HTarget(m_target.x, 0.0, m_target.z);
        HTarget.Normalize();

        /* The commented code is the original calculation described in
           the "Camera Rotation With Quaternion" video (https://youtu.be/MZuYmG1GBFk).
           As suggested by the youtube viewer @Brmngm I replaced it with the simpler
           code using atan2 below which does the same thing with less hassle.

          float Angle = ToDegree(asin(abs(HTarget.z)));

          if (HTarget.z >= 0.0f) {
              if (HTarget.x >= 0.0f) {
                  m_AngleH = 360.0f - Angle;
              } else {
                  m_AngleH = 180.0f + Angle;
              }
          } else {
              if (HTarget.x >= 0.0f) {
                  m_AngleH = Angle;
              } else {
                  m_AngleH = 180.0f - Angle;
              }
          }*/

        m_AngleH = -ToDegree(atan2(m_target.z, m_target.x));

        m_AngleV = -ToDegree(asin(m_target.y));

        m_OnUpperEdge = false;
        m_OnLowerEdge = false;
        m_OnLeftEdge = false;
        m_OnRightEdge = false;
        m_mousePos.x = m_windowWidth / 2;
        m_mousePos.y = m_windowHeight / 2;
    }



    void BasicCamera::SetPosition(float x, float y, float z)
    {
        m_pos.x = x;
        m_pos.y = y;
        m_pos.z = z;
    }


    void BasicCamera::SetPosition(const MyVectorV3::Vector3f& pos)
    {
        SetPosition(pos.x, pos.y, pos.z);
    }


    void BasicCamera::SetTarget(float x, float y, float z)
    {
        m_target.x = x;
        m_target.y = y;
        m_target.z = z;

        InitInternal();
    }


    void BasicCamera::SetTarget(const MyVectorV3::Vector3f& target)
    {
        SetTarget(target.x, target.y, target.z);
    }


    bool BasicCamera::OnKeyboard(int Key)
    {
        bool CameraChangedPos = false;

        switch (Key) {

        case GLFW_KEY_W:
            m_pos += (m_target * m_speed);
            CameraChangedPos = true;
            break;

        case GLFW_KEY_S:
            m_pos -= (m_target * m_speed);
            CameraChangedPos = true;
            break;

        case GLFW_KEY_A:
        {
            MyVectorV3::Vector3f Left = m_target.Cross(m_up);
            Left.Normalize();
            Left *= m_speed;
            m_pos += Left;
            CameraChangedPos = true;
        }
        break;

        case GLFW_KEY_D:
        {
            MyVectorV3::Vector3f Right = m_up.Cross(m_target);
            Right.Normalize();
            Right *= m_speed;
            m_pos += Right;
            CameraChangedPos = true;
        }
        break;

        case GLFW_KEY_UP:
            m_AngleV -= m_speed;
            Update();
            break;

        case GLFW_KEY_DOWN:
            m_AngleV += m_speed;
            Update();
            break;

        case GLFW_KEY_LEFT:
            m_AngleH -= m_speed;
            Update();
            break;

        case GLFW_KEY_RIGHT:
            m_AngleH += m_speed;
            Update();
            break;

        case GLFW_KEY_PAGE_UP:
            m_pos.y += m_speed;
            CameraChangedPos = true;
            break;

        case GLFW_KEY_PAGE_DOWN:
            m_pos.y -= m_speed;
            CameraChangedPos = true;
            break;

        case GLFW_KEY_KP_ADD:
            m_speed += 0.1f;
            printf("Speed changed to %f\n", m_speed);
            break;

        case GLFW_KEY_KP_SUBTRACT:
            m_speed -= 0.1f;
            if (m_speed < 0.1f) {
                m_speed = 0.1f;
            }
            printf("Speed changed to %f\n", m_speed);
            break;

        case GLFW_KEY_C:
            printf("Camera pos: "); m_pos.Print(); printf("\n");
            printf("Camera target: "); m_target.Print(); printf("\n");
            break;
        }

        if (CameraChangedPos) {
            //        printf("Camera pos: "); m_pos.Print(); printf("\n");
        }

        return CameraChangedPos;
    }

    void BasicCamera::setOffsetScreen(int xleft, int xright, int ytop, int ybottom) {

        this->offset.left = xleft;
        this->offset.right = xright;
        this->offset.top = ytop;
        this->offset.bottom = ybottom;
    }

    void BasicCamera::OnMouse(int x, int y)
    {

        //std::cout << "x: "<<x << ", y: " << y << std::endl; //0,0 sol üst
        /* ESKÝSÝ
        int DeltaX = x - m_mousePos.x;
        int DeltaY = y - m_mousePos.y;

        m_mousePos.x = x;
        m_mousePos.y = y;

        m_AngleH += (float)DeltaX / 20.0f;
        m_AngleV += (float)DeltaY / 20.0f;
        */

        float halfWidth = m_windowWidth / 2;
        float halfHeight = m_windowHeight / 2;
        
        m_AngleH += (( ((x - halfWidth) / this->m_windowWidth)* maxAngleH)-lastAngleH);
        m_AngleV += (((-1*(y*2.0-m_windowHeight) / this->m_windowWidth) * maxAngleV)-lastAngleV)*-1;
       // std::cout << "x: " << m_AngleH << ", y: " << m_AngleV << std::endl;

        lastAngleH =  ((x - halfWidth) / this->m_windowWidth) * maxAngleH;
        lastAngleV =  (-1 * (y * 2.0 - m_windowHeight) / this->m_windowWidth) * maxAngleV;



        /*
        if (x <= MARGIN) {
            m_OnLeftEdge = true;
            m_OnRightEdge = false;
        }
        else if (x >= (m_windowWidth - MARGIN)) {
            m_OnRightEdge = true;
            m_OnLeftEdge = false;
        }
        else {
            m_OnLeftEdge = false;
            m_OnRightEdge = false;
        }

        if (y <= MARGIN) {
            m_OnUpperEdge = true;
            m_OnLowerEdge = false;
        }
        else if (y >= (m_windowHeight - MARGIN)) {
            m_OnLowerEdge = true;
            m_OnUpperEdge = false;
        }
        else {
            m_OnUpperEdge = false;
            m_OnLowerEdge = false;
        }
        */
        Update();
    }


    void BasicCamera::OnMouse(int x, int y,int offsetLeft,int offsetRight,int offsetTop,int offsetBottom,int winWidth,int winHeight)
    {

        //std::cout << "m_target: " << m_target.x << ", " << m_target.y << ", " << m_target.z << std::endl;
        struct {
            int xLeft;
            int xRight;
            int yTop;
            int yBottom;


       }newCoordinate;

        newCoordinate.xLeft = offsetLeft;
        newCoordinate.xRight = winWidth - offsetRight;
        newCoordinate.yTop = offsetTop;
        newCoordinate.yBottom = winHeight - offsetBottom;

        
        if (x >= newCoordinate.xLeft && x <= newCoordinate.xRight &&
            y >= newCoordinate.yTop && y <= newCoordinate.yBottom) {

    
            //change newcoordinate system
            x = x - newCoordinate.xLeft;
            y = y - newCoordinate.yTop;
            m_mousePos.x -= newCoordinate.xLeft;
            m_mousePos.y -= newCoordinate.yTop;

        int DeltaX = x - m_mousePos.x;
        int DeltaY = y - m_mousePos.y;

        m_mousePos.x = x;
        m_mousePos.y = y;

        m_AngleH += (float)DeltaX / 20.0f;
        m_AngleV += (float)DeltaY / 20.0f;

        /*
        if (x <= MARGIN) {
            m_OnLeftEdge = true;
            m_OnRightEdge = false;
        }
        else if (x >= (m_windowWidth - MARGIN)) {
            m_OnRightEdge = true;
            m_OnLeftEdge = false;
        }
        else {
            m_OnLeftEdge = false;
            m_OnRightEdge = false;
        }

        if (y <= MARGIN) {
            m_OnUpperEdge = true;
            m_OnLowerEdge = false;
        }
        else if (y >= (m_windowHeight - MARGIN)) {
            m_OnLowerEdge = true;
            m_OnUpperEdge = false;
        }
        else {
            m_OnUpperEdge = false;
            m_OnLowerEdge = false;
        }
        */
        Update();
      }
    }


    void BasicCamera::UpdateMousePosSilent(int x, int y)
    {
        m_mousePos.x = x;
        m_mousePos.y = y;
    }


    void BasicCamera::OnRender()
    {
        bool ShouldUpdate = false;
        /*
        if (m_OnLeftEdge) {
            m_AngleH -= EDGE_STEP;
            ShouldUpdate = true;
        }
        else if (m_OnRightEdge) {
            m_AngleH += EDGE_STEP;
            ShouldUpdate = true;
        }

        if (m_OnUpperEdge) {
            if (m_AngleV > -90.0f) {
                m_AngleV -= EDGE_STEP;
                ShouldUpdate = true;
            }
        }
        else if (m_OnLowerEdge) {
            if (m_AngleV < 90.0f) {
                m_AngleV += EDGE_STEP;
                ShouldUpdate = true;
            }
        }
        */
       
            Update();
        
    }

    void BasicCamera::Update()
    {
        MyVectorV3::Vector3f Yaxis(0.0f, 1.0f, 0.0f);

        // Rotate the view vector by the horizontal angle around the vertical axis
        MyVectorV3::Vector3f View(1.0f, 0.0f, 0.0f);
        View.Rotate(m_AngleH, Yaxis);
        View.Normalize();

        // Rotate the view vector by the vertical angle around the horizontal axis
        MyVectorV3::Vector3f U = Yaxis.Cross(View);
        U.Normalize();
        View.Rotate(m_AngleV, U);

        m_target = View;
        m_target.Normalize();
       // std::cout << m_target.x << ", " << m_target.y << ", " << m_target.z << std::endl;

        m_up = m_target.Cross(U);
        m_up.Normalize();
    }



    MyMatrixV3::Matrix4f BasicCamera::GetMatrix() const
    {
        MyMatrixV3::Matrix4f CameraTransformation;
        CameraTransformation.InitCameraTransform(m_pos, m_target, m_up);

        return CameraTransformation;
    }



    MyMatrixV3::Matrix4f BasicCamera::GetViewProjMatrix() const
    {
        MyMatrixV3::Matrix4f View = GetMatrix();
        MyMatrixV3::Matrix4f Projection = GetProjectionMat();
        MyMatrixV3::Matrix4f ViewProj = Projection * View;
        return ViewProj;
    }


    MyMatrixV3::Matrix4f BasicCamera::GetViewportMatrix() const
    {
        float HalfW = m_windowWidth / 2.0f;
        float HalfH = m_windowHeight / 2.0f;

        MyMatrixV3::Matrix4f Viewport = MyMatrixV3::Matrix4f(HalfW, 0.0f, 0.0f, HalfW,
            0.0f, HalfH, 0.0f, HalfH,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        return Viewport;
    }


    void BasicCamera::SetSpeed(float Speed)
    {
        if (Speed <= 0.0f) {
            printf("Invalid camera speed %f\n", Speed);
            exit(0);
        }

        m_speed = Speed;
    }



    /*
    * 
    *   ORBITAL CAMERA
    * 
    */

  
    OrbitalCamera::OrbitalCamera() {
        setOrbitalCamera(-30, PI / 2, PI, PI / 450);
    }
    OrbitalCamera::OrbitalCamera(float radius, float startPhi,
        float startTheta, float deltaRadian) {

        setOrbitalCamera(radius, startPhi, startTheta, deltaRadian);

    }


    void OrbitalCamera::setOrbitalCamera(float radius, float startPhi,
        float startTheta, float deltaRadian) {

        this->verticalLimit.isActive = true;
        verticalLimit.MaxRadian = PI;
        verticalLimit.MinRadian = 0;


        this->horizontalLimit.isActive = false;

        center = MyVectorV3::Vector3f(0, 0, 0);
        this->radius = radius;
        this->phi = startPhi;
        this->theta = startTheta;
        this->deltaRadian = deltaRadian;

        m_pos = MyVectorV3::Vector3f(0, 0, -3);
        m_target = MyVectorV3::Vector3f(0, 0, 1);
        m_up = MyVectorV3::Vector3f(0, 1, 0);

    }

    void OrbitalCamera::setCenterPoint(float x, float y, float z) {
        center = MyVectorV3::Vector3f(x,y,z);
    }
    void OrbitalCamera::setCenterPoint(MyVectorV3::Vector3f center_) {
        center = center_;
    }

    void OrbitalCamera::setPerspectiveProjection(MyProjection::PersProjInfo& persProjInfo) {

        m_persProjInfo = persProjInfo;
        m_projection.InitPersProjTransform(persProjInfo);
        m_windowWidth = (int)persProjInfo.Width;
        m_windowHeight = (int)persProjInfo.Height;
    }

    void OrbitalCamera::updateDrag(float x, float y) {

        if (dragLastX == -1 || dragLastX == -1) {

            
            dragLastX = x;
            dragLastY = y;
        }

        if (this->isMouseLeftButton) {

        float deltaX = x - dragLastX;
        float deltaY = (y - dragLastY) ;

        if (!horizontalLimit.isActive) {

        float deltaRadianX_ = deltaX * deltaRadian;
        this->theta += deltaRadianX_;
        }
        else {

            float deltaRadianX_ = deltaX * deltaRadian;
            if (theta + deltaRadianX_ <= horizontalLimit.MaxRadian &&
                theta + deltaRadianX_ >= horizontalLimit.MinRadian) {

            this->theta += deltaRadianX_;
            }

        }

        if (!verticalLimit.isActive) {

        float deltaRadianY_ = deltaY * deltaRadian;
        this->phi -= deltaRadianY_;
       
        }else{

            float deltaRadianY_ = deltaY * deltaRadian;
            if (phi - deltaRadianY_ <= verticalLimit.MaxRadian &&
                phi - deltaRadianY_ >= verticalLimit.MinRadian) {

                this->phi -= deltaRadianY_;
            }
        }

        }

        dragLastX = x;
        dragLastY = y;

    }


    MyVectorV3::Vector3f OrbitalCamera::convertSphericalToCartesianCoordinate() {

        float x = radius * sinf(theta) * sinf(phi);
        float y = cosf(phi) * radius;
        float z = radius * cosf(theta) * sinf(phi);

        return MyVectorV3::Vector3f(x, y, z);
        
    }

    MyMatrixV3::Matrix4f OrbitalCamera::GetViewMatrix() {

        MyMatrixV3::Matrix4f CameraTransformation;
        CameraTransformation.InitCameraTransform(m_pos, m_target, m_up);

        return CameraTransformation;
    }
    MyMatrixV3::Matrix4f OrbitalCamera::GetProjectionMatrix() {

        return m_projection;
    }
    MyMatrixV3::Matrix4f OrbitalCamera::GetViewProjectionMatrix() {

        MyMatrixV3::Matrix4f viewMatrix = GetViewMatrix();
        return  m_projection * viewMatrix;
    }

    void OrbitalCamera::updateCamera() {

        MyVectorV3::Vector3f worldUp;
     
            worldUp=  MyVectorV3::Vector3f(0, 1, 0);
        
   

        m_pos = convertSphericalToCartesianCoordinate() + this->center;
        m_target =  (center-m_pos).Normalize();
        MyVectorV3::Vector3f cameraRight = worldUp.Cross(m_target);
        m_up = m_target.Cross(cameraRight);

        
    }   



    //Modern Camera

    MyVectorV3::Vector3f Camera::lookAt(MyVectorV3::Vector3f point) {

        this->target = point - this->position;
        this->target.Normalize();

        return this->target;

    }

    MyVectorV3::Vector3f Camera::lookAt(float x, float y, float z) {


		return this->lookAt(MyVectorV3::Vector3f(x, y, z));
    }

    void Camera::setPosition(float x, float y, float z) {
        this->position.x = x;
        this->position.y = y;
        this->position.z = z;
    }
    void Camera::setPosition(MyVectorV3::Vector3f position_) {
        this->position = position_;
    }
    void Camera::setDirection(float x, float y, float z) {
        this->target.x = x;
        this->target.y = y;
        this->target.z = z;

        this->target.Normalize();
    }
    void Camera::setDirection(MyVectorV3::Vector3f dir) {
        this->target = dir;
        this->target.Normalize();
    }

    MyMatrixV3::Matrix4f Camera::GetViewMatrix() {


        MyMatrixV3::Matrix4f CameraTranslation;
        CameraTranslation.InitTranslationTransform(-position.x, -position.y, -position.z);

        MyMatrixV3::Matrix4f CameraRotateTrans;
        CameraRotateTrans.InitCameraTransform(this->target, this->up);

        this->viewMatrix = CameraRotateTrans * CameraTranslation;;

		return this->viewMatrix;
    }

    MyMatrixV3::Matrix4f Camera::GetProjectionMatrix(){

        return this->projectionMatrix;

        }

    MyMatrixV3::Matrix4f Camera::GetViewProjectionMatrix(){

     

		return this->GetProjectionMatrix() * this->GetViewMatrix();

        }


	//OrthographicCamera

    OrthographicCamera::OrthographicCamera(MyProjection::OrthoProjInfo orthographic) :Camera() {


		this->setOrthographicProjection(orthographic);
    }

    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top,
        float near_, float far_) :Camera() {

        this->setOrthographicProjection(left, right, bottom, top, near_, far_);


    }

    OrthographicCamera::OrthographicCamera(MyProjection::OrthoProjInfo orthographic, 
        MyVectorV3::Vector3f position, MyVectorV3::Vector3f dir) :Camera() {

		this->setOrthographicProjection(orthographic);
		this->position = position;
		this->target = dir;

    }

   

    void OrthographicCamera::setOrthographicProjection(MyProjection::OrthoProjInfo orthographic) {

		this->projectionMatrix.InitOrthoProjTransform(orthographic);

    }

    void OrthographicCamera::setOrthographicProjection(float left, float right,
        float bottom, float top, float near_, float far_) {


        MyProjection::OrthoProjInfo orthographic;
		orthographic.l = left;
		orthographic.r = right;
		orthographic.b = bottom;
		orthographic.t = top;
		orthographic.n = near_;
		orthographic.f = far_;

        this->setOrthographicProjection(orthographic);


    }


    //Perspective camera

    PerspectiveCamera::PerspectiveCamera(MyProjection::PersProjInfo perspective) {

		setPerspectiveProjection(perspective);

    }
    PerspectiveCamera::PerspectiveCamera(float fov, float width, float height, float near_, float far_){

		setPerspectiveProjection(fov, width, height, near_, far_);

        }
    PerspectiveCamera::PerspectiveCamera(MyProjection::PersProjInfo perspective, MyVectorV3::Vector3f position_,
        MyVectorV3::Vector3f dir) {

		setPerspectiveProjection(perspective);
        this->target = dir;
		this->position = position_;

    }


    void PerspectiveCamera::setPerspectiveProjection(MyProjection::PersProjInfo perspective){

		this->projectionMatrix.InitPersProjTransform(perspective);

        }

    void PerspectiveCamera::setPerspectiveProjection(float fov, float width, float height,
        float near_, float far_){

        MyProjection::PersProjInfo perspective;
        perspective.FOV = fov;
        perspective.Width = width;
        perspective.Height = height;
        perspective.zNear = near_;
        perspective.zFar = far_;

        this->projectionMatrix.InitPersProjTransform(perspective);
        }

}