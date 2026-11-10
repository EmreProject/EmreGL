#ifndef ogl_scene_h
#define ogl_scene_h


#include <GL/glew.h>
#include <map>
#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Projection.h"
#include "ogl_Camera.h"
#include "ogl_lightning_v2.h"
#include "ogl_model_v2.h"
#include "ogl_shader_v2.h"
#include "ogl_texture.h"
#include "ogl_World.h"
#include "oglWindow.h"
#include "ogl_default_meshes.h"
#include "oglMeshes.h"
#include "oglAssimp.h"
#include "ogl_axis_debug.h"





using namespace std;


namespace OglScene{


	class Scene {

	private:

		bool meshIsAlreadyAdded(OglObject3D::Mesh* mesh);
		bool modelIsAlreadyAdded(OglAssimp::ModelData* model);

		OglObject3D::Mesh* envMap = nullptr;
		MyTextureV2::CubeTexture envMapTexture;
		bool isEnvMapSet = false;

	public:


		//lights
		list<MyOglLightV2::DirectionalLight*> directionalLights;
		list<MyOglLightV2::PointLight*> pointLights;
		list<MyOglLightV2::SpotLight*> spotLights;

		//meshes
		list<OglObject3D::Mesh*> meshes;

		//assimp models
		list<OglAssimp::ModelData*> models;

		
		Scene(){}


		void SetEnvironmentMap(MyTextureV2::CubeTexture);
		OglObject3D::Mesh* GetEnvironmentMap();
		MyTextureV2::CubeTexture GetEnvironmentMapTexture();
		bool isEnvironmentMapSet();

		template<typename t>
		void add(t& obj){
		
			cout << "scene empty add maybe error look at oglscene.h" << endl;
		}

		template<typename t>
		void add(t* obj) {

			cout << "scene empty add maybe error look at oglscene.h" << endl;
		}
		
		template<typename t>
		void remove(t& obj) {

			cout << "scene empty remove maybe error look at oglscene.h" << endl;
		}

		template<typename t>
		void remove(t* obj) {

			cout << "scene empty remove maybe error look at oglscene.h" << endl;
		}
		
		template<>
		void add<OglObject3D::Mesh>(OglObject3D::Mesh* mesh) {


			if (!meshIsAlreadyAdded(mesh)) {
				if (!mesh->isLoaded()) {
				mesh->LoadMesh();
				}
				this->meshes.insert(this->meshes.end(), mesh);
			}
			else {
				cout << "Mesh is already added to scene, not adding again" << endl;
			}

		}

		template<>
		void add<OglObject3D::Mesh>(OglObject3D::Mesh& mesh) {


			if (!meshIsAlreadyAdded(&mesh)) {

				if (!mesh.isLoaded()) {
					mesh.LoadMesh();
				}

				this->meshes.insert(this->meshes.end(), &mesh);

			}
			else {
				cout << "Mesh is already added to scene, not adding again" << endl;
			}

		}


		template<>
		void remove<OglObject3D::Mesh>(OglObject3D::Mesh& mesh) {


			auto it = remove_if(this->meshes.begin(), this->meshes.end(),
				[&mesh](OglObject3D::Mesh* m) {
					return m == &mesh;
				});
			meshes.erase(it, this->meshes.end());

		}

		template<>
		void remove<OglObject3D::Mesh>(OglObject3D::Mesh* mesh) {


			remove(*mesh);

		}



		template<>
		void add<OglObject3D::Points>(OglObject3D::Points* mesh) {


			if (!meshIsAlreadyAdded(mesh)) {
				if (!mesh->isLoaded()) {
				mesh->LoadMesh();
				}

				this->meshes.insert(this->meshes.end(), mesh);
			}
			else {
				cout << "Points is already added to scene, not adding again" << endl;
			}
		
		}

		template<>
		void add<OglObject3D::Points>(OglObject3D::Points& mesh) {


			if (!meshIsAlreadyAdded(&mesh)) {
				if (!mesh.isLoaded()) {
					mesh.LoadMesh();
				}

				this->meshes.insert(this->meshes.end(), &mesh);
			}
			else {
				cout << "Points is already added to scene, not adding again" << endl;
			}

		}



		template<>
		void remove<OglObject3D::Points>(OglObject3D::Points& mesh) {

			auto it = remove_if(this->meshes.begin(), this->meshes.end(),
				[&mesh](OglObject3D::Mesh* m) {
					OglObject3D::Points* pointsMesh = dynamic_cast<OglObject3D::Points*>(m);
					if(pointsMesh!=nullptr) {
						//if m is a points mesh and m is the same as mesh to remove
						return pointsMesh == &mesh;
					}
					else {
						return false;
					}
					
				});
			meshes.erase(it, this->meshes.end());
		}

		template<>
		void remove<OglObject3D::Points>(OglObject3D::Points* mesh) {

			remove(*mesh);
		}



		template<>
		void add<OglObject3D::Line>(OglObject3D::Line* line) {


			if (!meshIsAlreadyAdded(line)) {
				if (!line->isLoaded()) {
					line->LoadMesh();
				}

				this->meshes.insert(this->meshes.end(), line);
			}
			else {
				cout << "Line is already added to scene, not adding again" << endl;
			}

		}

		template<>
		void add<OglObject3D::Line>(OglObject3D::Line& line) {


			if (!meshIsAlreadyAdded(&line)) {
				if (!line.isLoaded()) {
					line.LoadMesh();
				}

				this->meshes.insert(this->meshes.end(), &line);
			}
			else {
				cout << "Line is already added to scene, not adding again" << endl;
			}

		}



		template<>
		void remove<OglObject3D::Line>(OglObject3D::Line& mesh) {

			auto it = remove_if(this->meshes.begin(), this->meshes.end(),
				[&mesh](OglObject3D::Mesh* m) {
					OglObject3D::Line* lineMesh = dynamic_cast<OglObject3D::Line*>(m);
					if (lineMesh != nullptr) {
						//if m is a points mesh and m is the same as mesh to remove
						return lineMesh == &mesh;
					}
					else {
						return false;
					}

				});
			meshes.erase(it, this->meshes.end());
		}

		template<>
		void remove<OglObject3D::Line>(OglObject3D::Line* line) {

			remove(*line);
		}


		template<>
		void add<OglAssimp::ModelData>(OglAssimp::ModelData& model) {

			if (!modelIsAlreadyAdded(&model)) {

				for_each(model.meshes.begin(), model.meshes.end(), [this](OglObject3D::Mesh& mesh) {
					
	
					mesh.isAssimpMesh = true;

					
					
					if (!mesh.isLoaded()) {
					mesh.LoadMesh();
					}
					
					this->meshes.insert(this->meshes.end(), &mesh);
					});
				
				this->models.push_back(&model);

				cout << "Model with " << model.meshes.size() << " meshes added to the scene" << endl;
			}
			else {


				cout << "Model with " << model.meshes.size() << " meshes is already added to scene, not adding again" << endl;
			}

		}


		template<>
		void add<OglAssimp::ModelData>(OglAssimp::ModelData* model) {

			add(*model);
		}

		template<>
		void remove<OglAssimp::ModelData>(OglAssimp::ModelData& model) {

			if (modelIsAlreadyAdded(&model)) {

				auto it=remove_if(this->models.begin(), this->models.end(),
					[&model](OglAssimp::ModelData* m) {
						return m == &model;
					});
				models.erase(it, this->models.end());


				for_each(model.meshes.begin(), model.meshes.end(), [this, &model](OglObject3D::Mesh& mesh) {
					auto itMesh = remove_if(this->meshes.begin(), this->meshes.end(),
						[&mesh](OglObject3D::Mesh* m) {
							return m == &mesh;
						});
					this->meshes.erase(itMesh, this->meshes.end());
					});

			
			}
		

		}

		template<>
		void remove<OglAssimp::ModelData>(OglAssimp::ModelData* model) {

			remove(*model);

		}

		template<>
		void add<MyOglLightV2::DirectionalLight>(MyOglLightV2::DirectionalLight* dirLight) {

			this->directionalLights.push_back(dirLight);
	
		}

		template<>
		void remove<MyOglLightV2::DirectionalLight>(MyOglLightV2::DirectionalLight* dirLight) {
			
			for_each(this->directionalLights.begin(), this->directionalLights.end(), [&dirLight, this](MyOglLightV2::DirectionalLight* dl) {
				if (dl == dirLight) {
					this->directionalLights.remove(dl);
				}
				});
			
		}

		template<>
		void add<MyOglLightV2::PointLight>(MyOglLightV2::PointLight* pointLight) {

			this->pointLights.insert(pointLights.end(), pointLight);

		}

		template<>
		void remove<MyOglLightV2::PointLight>(MyOglLightV2::PointLight* pointLight) {
			auto itErase = remove_if(this->pointLights.begin(), this->pointLights.end(),
				[&pointLight](MyOglLightV2::PointLight* pl) {
					return pl == pointLight;
				});

			this->pointLights.erase(itErase, this->pointLights.end());
			
		}

		template<>
		void add<MyOglLightV2::SpotLight>(MyOglLightV2::SpotLight* spotLight) {

			this->spotLights.insert(spotLights.end(), spotLight);


		}

		template<>
		void remove<MyOglLightV2::SpotLight>(MyOglLightV2::SpotLight* spotLight) {
			auto itErase = remove_if(this->spotLights.begin(), this->spotLights.end(),
				[&spotLight](MyOglLightV2::SpotLight* sl) {
					return sl == spotLight;
				});
			this->spotLights.erase(itErase, this->spotLights.end());
		}


		template<>
		void add<MyOglLightV2::DirectionalLight>(MyOglLightV2::DirectionalLight& dirLight) {

			this->directionalLights.push_back(&dirLight);
			
		}

		template<>
		void remove<MyOglLightV2::DirectionalLight>(MyOglLightV2::DirectionalLight& dirLight) {
			
			auto it=remove_if(this->directionalLights.begin(), this->directionalLights.end(),[&dirLight](MyOglLightV2::DirectionalLight* dl) {
				return dl == &dirLight;
				});

			this->directionalLights.erase(it, this->directionalLights.end());
			
		}

		template<>
		void add<MyOglLightV2::PointLight>(MyOglLightV2::PointLight& pointLight) {

			this->pointLights.insert(pointLights.end(), &pointLight);

		}

		template<>
		void remove<MyOglLightV2::PointLight>(MyOglLightV2::PointLight& pointLight) {
			auto itErase=remove_if(this->pointLights.begin(), this->pointLights.end(),
				[&pointLight](MyOglLightV2::PointLight* pl) {
					return pl == &pointLight;
				});

			this->pointLights.erase(itErase, this->pointLights.end());
		}

		template<>
		void add<MyOglLightV2::SpotLight>(MyOglLightV2::SpotLight& spotLight) {

			this->spotLights.insert(spotLights.end(), &spotLight);

		}
		
		template<>
		void remove<MyOglLightV2::SpotLight>(MyOglLightV2::SpotLight& spotLight) {
			auto itErase = remove_if(this->spotLights.begin(), this->spotLights.end(),
				[&spotLight](MyOglLightV2::SpotLight* sl) {
					return sl == &spotLight;
				});
			this->spotLights.erase(itErase, this->spotLights.end());
		}

		template<>
		void add<OglAxisDebug::AxesHelper>(OglAxisDebug::AxesHelper& axesHelper) {
			this->add(axesHelper.x_axis);
			this->add(axesHelper.y_axis);
			this->add(axesHelper.z_axis);
		}

		//adding camera for only setting glfw events!!!!!!!!!!
		template<>
		void add<MyCamera::OrbitalCamera>(MyCamera::OrbitalCamera* cam) {

			

			OglWindow::Window::addEventListener(OglWindow::MouseMove, [cam](double x, double y)->void {

				cam->updateDrag(x, y);
				
				});


			OglWindow::Window::addEventListener(OglWindow::MouseClick, [cam](int button,int action,int x,int y)->void {


				if (button == GLFW_MOUSE_BUTTON_LEFT) {
					if (action == GLFW_PRESS && OglWindow::Window::isOnGuiElement==false) {
						cam->isMouseLeftButton = true;
						std::cout << "Left mouse button pressed" << std::endl;
					}
					else if (action == GLFW_RELEASE) {
						cam->isMouseLeftButton = false;
						std::cout << "Left mouse button released" << std::endl;
					}
				}

				});

			OglWindow::Window::addEventListener(OglWindow::Scroll, [cam](double xoffset, double yoffset) {

				//yak�nlasma
				if (yoffset == 1) {

					cam->radius *= (1 - cam->ratioZoom);
				}
				else if (yoffset == -1) { //uzaklasma

					cam->radius *= (1 + cam->ratioZoom);
				}

				});

			
		}
		template<>
		void add<MyCamera::BasicCamera>(MyCamera::BasicCamera* cam) {

		

			OglWindow::Window::addEventListener(OglWindow::KeyDown, [cam](int key,int action)->void {

				cam->OnKeyboard(key);

				});

			OglWindow::Window::addEventListener(OglWindow::MouseMove, [cam](double x, double y)->void {

				cam->OnMouse(x, y);

				});



		}



		list<OglObject3D::Mesh*>& GetMeshes();
		list<OglObject3D::Mesh*> GetOpaqueMeshes();
		list<OglObject3D::Mesh*> GetTransparentMeshes(); 
		


	};




}



#endif