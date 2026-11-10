#ifndef oglMouseCoord_h
#define oglMouseCoord_h


#include "oglScene.h"
#include "oglMeshes.h"
#include "oglRenderTarget.h" 
#include "oglControls.h"
#include <thread>
#include <chrono>

namespace OglMouseCoord {

	class Mouse {
	private:

	
		MyCamera::Camera* camera;
		OglControls::OrbitalControls* orbitControls = nullptr;

		MyVectorV3::Vector3f rayOrigin;
		MyVectorV3::Vector3f rayDirection;

		
		struct MouseMoveObject {

			std::variant<OglObject3D::Mesh*, OglAssimp::ModelLoader::ModelData*> object;
			MyVectorV3::Vector3f dragStartPoint;
			MyVectorV3::Vector3f dragPlaneNormal;
			MyVectorV3::Vector3f objectStartPosition;
			MyVectorV3::Vector3f dragStartWorld;

		};

		bool isDragging = false;
		bool isWaitingForRelease = false;
		list<MouseMoveObject> mouseMoveObjects; 
		MouseMoveObject* draggingObject = nullptr;

		bool RayPlaneIntersection(MyVectorV3::Vector3f rayOrigin,
			MyVectorV3::Vector3f rayDir, MyVectorV3::Vector3f planePoint,
			MyVectorV3::Vector3f planeNormal, MyVectorV3::Vector3f& outPoint);
	public:

		Mouse( MyCamera::Camera*,OglControls::OrbitalControls* orbitControls_=nullptr);
		

		MyVectorV3::Vector3f GetRayOrigin();
		MyVectorV3::Vector3f GetRayDirection();

		template<typename t>
			requires is_same_v<t, OglObject3D::Mesh*> || is_same_v<t, OglAssimp::ModelLoader::ModelData*>
		void addDraggableObject(t object) {

			
			this->mouseMoveObjects.emplace_back();
			auto& item = this->mouseMoveObjects.back();
			item.object = object;

			object->addEventListener("click", [this,object]() {
				
				if (this->isWaitingForRelease || this->isDragging) return; //prevent click event right after dragging
				
				auto it = this->mouseMoveObjects.begin();
				for(;it != this->mouseMoveObjects.end(); it++) {

					bool check = false;

					if (auto stored = std::get_if<t>(&it->object)) {
						check = (*stored == object);
					}

					if (check) {

						if (std::holds_alternative<OglObject3D::Mesh*>(it->object)) {
							it->dragStartPoint = get<OglObject3D::Mesh*>(it->object)->position;
						}
						if (std::holds_alternative<OglAssimp::ModelLoader::ModelData*>(it->object)) {
							it->dragStartPoint = get<OglAssimp::ModelLoader::ModelData*>(it->object)->position;
						}
						it->dragPlaneNormal= this->camera->GetDirection();
						
						MyVectorV3::Vector3f rayOrigin=this->camera->GetPosition();

						MyVectorV3::Vector3f hitPoint;

						if (this->RayPlaneIntersection(rayOrigin, this->GetRayDirection(), it->dragStartPoint, it->dragPlaneNormal, hitPoint)) {
							it->objectStartPosition = it->dragStartPoint;
							it->dragStartWorld = hitPoint;

							this->draggingObject = &(*it);
							this->isDragging = true;

							if (this->orbitControls != nullptr) {
								this->orbitControls->isEnabled = false; //disable orbit controls while dragging
							}
						}
						
						
						break;
					}
				}
			
				
				});

		}
	};
	
}  


#endif