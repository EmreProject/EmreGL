#include "oglControls.h"

namespace OglControls {

	//Controls --------------------------------------------------------




	//OrbitalControls --------------------------------------------------------
	

	OrbitalControls::OrbitalControls(MyCamera::Camera* cam) : Controls(cam) {
		setCenterPoint(0, 0, 0);
		this->resetDefaultSettings();


		OglWindow::Window::addEventListener(OglWindow::MouseMove, [this](double x, double y)->void {

			if (this->isEnabled == false) return;

			if (dragLastX == -1 || dragLastX == -1) {


				dragLastX = x;
				dragLastY = y;
			}

			if (this->isUpdate) {
				this->updateDrag(x, y);
			}

			this->dragLastX = x;
			this->dragLastY = y;

			this->isUpdate = false;

			});


		OglWindow::Window::addEventListener(OglWindow::MouseClick, [this](int button, int action, int x, int y)->void {

			if (this->isEnabled == false) return;

			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				if (action == GLFW_PRESS && OglWindow::Window::isOnGuiElement == false) {
					this->isMouseLeftButton = true;
					std::cout << "Left mouse button pressed" << std::endl;
				}
				else if (action == GLFW_RELEASE) {
					this->isMouseLeftButton = false;
					std::cout << "Left mouse button released" << std::endl;
				}
			}
			});

		OglWindow::Window::addEventListener(OglWindow::Scroll, [this](double xoffset, double yoffset) {

			if (this->isEnabled == false) return;
			OrbitalControls::SphericalCoordinate sphericalCoordinate = convertCartesianToSphericalCoordinate(camera->GetPosition(), center);

			if (this->isUpdate) {


				//yakýnlasma
				if (yoffset == 1) {

					sphericalCoordinate.radius *= (1 - this->ratioZoom);
				}
				else if (yoffset == -1) { //uzaklasma

					sphericalCoordinate.radius *= (1 + this->ratioZoom);
				}


				MyVectorV3::Vector3f newCameraPosition = convertSphericalToCartesianCoordinate(
					sphericalCoordinate.radius, sphericalCoordinate.theta, sphericalCoordinate.phi, center);
				camera->setPosition(
					newCameraPosition.x, newCameraPosition.y, newCameraPosition.z
				);

				camera->setDirection(
					center.x - newCameraPosition.x,
					center.y - newCameraPosition.y,
					center.z - newCameraPosition.z
				);
			}

			this->isUpdate = false;
			});

	}

	void OrbitalControls::resetDefaultSettings() {

		this->verticalLimit.isActive = true;
		verticalLimit.MaxRadian = PI-0.0001;
		verticalLimit.MinRadian = 0.0001;


		this->horizontalLimit.isActive = false;
		this->ratioZoom = 0.1f; 
		this->deltaRadian = PI / 450; //0.007f
	}

	//for converting angle coordinate to cartesian position vector
	MyVectorV3::Vector3f OrbitalControls::convertSphericalToCartesianCoordinate(float radius_,float theta_,float phi_, MyVectorV3::Vector3f center) {

		float x = radius_ * sinf(theta_) * sinf(phi_);
		float y = cosf(phi_) * radius_;
		float z = radius_ * cosf(theta_) * sinf(phi_);

		return MyVectorV3::Vector3f(x, y, z) + center;


	}

	OrbitalControls::SphericalCoordinate OrbitalControls::convertCartesianToSphericalCoordinate(MyVectorV3::Vector3f camPosition, MyVectorV3::Vector3f center) {

		OrbitalControls::SphericalCoordinate sphericalCoordinate;

		MyVectorV3::Vector3f relativePosition = camPosition - center;
		sphericalCoordinate.radius  = relativePosition.Length();

		if (sphericalCoordinate.radius == 0) {

			sphericalCoordinate.theta = 0;
			sphericalCoordinate.phi = 0;
			return sphericalCoordinate; //avoid division by zero
		}
		sphericalCoordinate.theta  = atan2(relativePosition.x, relativePosition.z); //from z to x
		float cosPhi = std::clamp(relativePosition.y / sphericalCoordinate.radius, -1.0f, 1.0f);
		sphericalCoordinate.phi = std::acos(cosPhi);

		return sphericalCoordinate;
	}

	void OrbitalControls::setCenterPoint(float x, float y, float z) {
		center = MyVectorV3::Vector3f(x, y, z);
		MyVectorV3::Vector3f direction =center -  camera->GetPosition() ;
		direction.Normalize();
		this->camera->setDirection(direction.x, direction.y, direction.z); //set camera direction to center point
	}

	void OrbitalControls::setCenterPoint(MyVectorV3::Vector3f center_) {
		this->setCenterPoint(center_.x, center_.y, center_.z);
	}

	MyVectorV3::Vector3f OrbitalControls::getCenterPoint() {
		return this->center;
	}

	void OrbitalControls::updateDrag(float x, float y) {

		OrbitalControls::SphericalCoordinate sphericalCoordinate = convertCartesianToSphericalCoordinate(camera->GetPosition(), center);
	

		if (this->isMouseLeftButton) {

			float deltaX = x - dragLastX;
			float deltaY = (y - dragLastY);

			if (!horizontalLimit.isActive) {

				float deltaRadianX_ = deltaX * deltaRadian;
				sphericalCoordinate.theta += deltaRadianX_;
			}
			else {

				float deltaRadianX_ = deltaX * deltaRadian;
				if (sphericalCoordinate.theta + deltaRadianX_ <= horizontalLimit.MaxRadian &&
					sphericalCoordinate.theta + deltaRadianX_ >= horizontalLimit.MinRadian) {

					sphericalCoordinate.theta += deltaRadianX_;
				}

			}

			if (!verticalLimit.isActive) {

				float deltaRadianY_ = deltaY * deltaRadian;
				sphericalCoordinate.phi -= deltaRadianY_;

			}
			else {

				float deltaRadianY_ = deltaY * deltaRadian;
				if (sphericalCoordinate.phi - deltaRadianY_ <= verticalLimit.MaxRadian &&
					sphericalCoordinate.phi - deltaRadianY_ >= verticalLimit.MinRadian) {

					sphericalCoordinate.phi -= deltaRadianY_;
				}
			}

		}

		

		MyVectorV3::Vector3f newCameraPosition = convertSphericalToCartesianCoordinate(
			sphericalCoordinate.radius, sphericalCoordinate.theta, sphericalCoordinate.phi, center);
		camera->setPosition(
			newCameraPosition.x, newCameraPosition.y, newCameraPosition.z
		);

		camera->setDirection(
			center.x - newCameraPosition.x,
			center.y - newCameraPosition.y,
			center.z - newCameraPosition.z
		);

	

	}
	void OrbitalControls::Update() {

		this->isUpdate = true;

	}

} 