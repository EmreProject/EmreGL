#include "oglMouseCoord.h"

namespace OglMouseCoord {


	Mouse::Mouse(MyCamera::Camera* cam, OglControls::OrbitalControls* orbitControls_) {
		
		this->camera = cam;
		this->orbitControls = orbitControls_;
	

		function<void(double x, double y)> mouseMoveFunc = [this](double x, double y) {
			//convert to NDC
			float ndcX = (2.0f * x) / OglWindow::Window::width - 1.0f;
			float ndcY = 1.0f - (2.0f * y) / OglWindow::Window::height; //inverted y axis
			//NDC to clip space 2 points for near and far planes
			MyVectorV3::Vector4f clipNear = MyVectorV3::Vector4f(ndcX, ndcY, -1.0f, 1.0f);
			MyVectorV3::Vector4f clipFar = MyVectorV3::Vector4f(ndcX, ndcY, 1.0f, 1.0f);
			
			//clip to world space
			MyMatrixV3::Matrix4f inverse=this->camera->GetViewProjectionMatrix().Inverse();
			MyVectorV3::Vector4f worldNear = inverse * clipNear;
			MyVectorV3::Vector4f worldFar = inverse * clipFar;

			worldNear = worldNear * (1.0f / worldNear.w);
			worldFar = worldFar * (1.0f / worldFar.w);

			this->rayOrigin = MyVectorV3::Vector3f(worldNear.x, worldNear.y, worldNear.z);
			this->rayDirection = MyVectorV3::Vector3f(worldFar.x - worldNear.x, worldFar.y - worldNear.y, worldFar.z - worldNear.z);
			this->rayDirection.Normalize();


			if (this->isDragging && this->draggingObject != nullptr) {
				MyVectorV3::Vector3f rayOrigin = this->camera->GetPosition();
				MyVectorV3::Vector3f hitPoint;
				if (this->RayPlaneIntersection(rayOrigin, this->GetRayDirection(), this->draggingObject->dragStartWorld, this->draggingObject->dragPlaneNormal, hitPoint)) {

					MyVectorV3::Vector3f delta = hitPoint - this->draggingObject->dragStartWorld;
					MyVectorV3::Vector3f newPosition = this->draggingObject->objectStartPosition + delta;

					if (std::holds_alternative<OglObject3D::Mesh*>(this->draggingObject->object)) {
						get<OglObject3D::Mesh*>(this->draggingObject->object)->position = newPosition;
					}
					if (std::holds_alternative<OglAssimp::ModelLoader::ModelData*>(this->draggingObject->object)) {
						get<OglAssimp::ModelLoader::ModelData*>(this->draggingObject->object)->position = newPosition;
					}
				}
			}


			};



		OglWindow::Window::addEventListener(OglWindow::MouseMove, mouseMoveFunc);
		OglWindow::Window::addEventListener(OglWindow::MouseClick, [this](int button, int action, int x, int y) {
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && this->isDragging) {
				this->isWaitingForRelease = true;
				this->isDragging = false;
				this->draggingObject = nullptr;

				if (this->orbitControls != nullptr) {
					this->orbitControls->isEnabled = true; //disable orbit controls while dragging
				}

				thread([this]() {
					this_thread::sleep_for(chrono::milliseconds(200));
					this->isWaitingForRelease = false;
					}).detach();
			}
			
			});
	}


	bool Mouse::RayPlaneIntersection(MyVectorV3::Vector3f rayOrigin,
		MyVectorV3::Vector3f rayDir, MyVectorV3::Vector3f planePoint,
		MyVectorV3::Vector3f planeNormal, MyVectorV3::Vector3f& outPoint) {


		float denom = planeNormal.Dot(rayDir);

		if (abs(denom) > 1e-6) { //check if ray is parallel to plane
			
			MyVectorV3::Vector3f p0l0 = planePoint - rayOrigin;
			float t = p0l0.Dot(planeNormal) / denom;
			if (t >= 0) { //intersection point is in the ray direction
				outPoint = rayOrigin + rayDir * t;
				return true;
			}
			else {
				return false;
			}
		
		}
		else {
			return false;
		}
	}

	MyVectorV3::Vector3f Mouse::GetRayOrigin() {

		return this->rayOrigin;
	}


	MyVectorV3::Vector3f Mouse::GetRayDirection() {

		return this->rayDirection;
	}


}