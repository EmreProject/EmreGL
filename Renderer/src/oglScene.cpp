#include "oglScene.h"
#include <random>

namespace OglScene {


	void Scene::SetEnvironmentMap(MyTextureV2::CubeTexture envMapTexture) {

		if(this->envMap!=nullptr){
			
			delete envMap;
		}

		this->isEnvMapSet = true;


		string envMapVertexShader = R"(
			#version 430 core
			layout(location = 0) in vec3 position;
			out vec3 TexCoords;
			uniform mat4 projectionMatrix;
			uniform mat4 viewMatrix;
			void main()
			{
				TexCoords = position;
				vec4 pos = projectionMatrix * viewMatrix * vec4(position, 1.0);
				gl_Position = pos.xyww; //w component is used for depth, so we set it to the same as z component
			}
		)";

		string envMapFragmentShader = R"(
			#version 430 core
			in vec3 TexCoords;
			out vec4 FragColor;
			uniform samplerCube skybox;
			void main()
			{    
				FragColor = texture(skybox, TexCoords);
			}
		)";

		this->envMapTexture = envMapTexture;
		OglObject3D::RawShaderMaterial* envMapMaterial = new OglObject3D::RawShaderMaterial(envMapVertexShader, envMapFragmentShader);
		envMapMaterial->setUniform("skybox", envMapTexture);
		envMapMaterial->isDebugModeUniformsAddress = false;
		OglObject3D::BoxGeometry* boxGeo = new OglObject3D::BoxGeometry(1,1,1);

		this->envMap = new OglObject3D::Mesh(boxGeo, envMapMaterial);
		
		this->envMap->LoadMesh();

	}

	OglObject3D::Mesh* Scene::GetEnvironmentMap() {

		return this->envMap;
	}

	MyTextureV2::CubeTexture Scene::GetEnvironmentMapTexture() {


		return this->envMapTexture;
	}

	bool Scene::isEnvironmentMapSet() { 
		
		return this->isEnvMapSet; 
	}

	list<OglObject3D::Mesh*>& Scene::GetMeshes() {

		return this->meshes;
	}

	list<OglObject3D::Mesh*> Scene::GetOpaqueMeshes() {

		list<OglObject3D::Mesh*> opaqueMeshes;

		auto it = meshes.begin();
		for (; it != meshes.end(); it++) {
			OglObject3D::Mesh* mesh = *it;
			if (!mesh->material->transparent) {
				opaqueMeshes.insert(opaqueMeshes.end(), mesh); //insert mesh to opaque meshes
			}
		}

		return opaqueMeshes;
	}
	list<OglObject3D::Mesh*> Scene::GetTransparentMeshes() {

		list<OglObject3D::Mesh*> transparentMeshes;

		auto it = meshes.begin();
		for (; it != meshes.end(); it++) {
			OglObject3D::Mesh* mesh = *it;
			if (mesh->material->transparent) {
				transparentMeshes.insert(transparentMeshes.end(), mesh); //insert mesh to opaque meshes
			}
		}

		return transparentMeshes;
	}

	bool Scene::meshIsAlreadyAdded(OglObject3D::Mesh* mesh) {

		bool isAlreadyAdded = false;

		for_each(this->meshes.begin(), this->meshes.end(), 
			[&isAlreadyAdded, &mesh](OglObject3D::Mesh* m) {
			if (m == mesh) {
				isAlreadyAdded = true;
			}
			});

		return isAlreadyAdded;

	}
	bool Scene::modelIsAlreadyAdded(OglAssimp::ModelData* model) {

		bool isAlreadyAdded = false;

		for_each(this->models.begin(), models.end(), [&model,&isAlreadyAdded](OglAssimp::ModelData* model_) {

			if (model == model_) {
				isAlreadyAdded = true;
			}

			});

		return isAlreadyAdded;

	}



}