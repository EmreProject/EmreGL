#include "ogl_lightning_v2.h"



namespace MyOglLightV2 {



	//BASELÝGHT

	void BaseLight::setColor(float x, float y, float z) {

		this->Color = MyVectorV3::Vector3f(x, y, z);

	}


	//Directional Light

	DirectionalLight::DirectionalLight() :BaseLight() {

		this->WorldDirection = MyVectorV3::Vector3f(0, 0, 0);
	}

	void DirectionalLight::CalcLocalDirection(MyOglWorldTransform::WorldTrans& worldTransform) {



		this->LocalDirection = worldTransform.WorldDirToLocalDir(this->WorldDirection);


	}

	void DirectionalLight::setWorldDirection(float x, float y, float z) {
	
		this->WorldDirection.x = x;
		this->WorldDirection.y = y;
		this->WorldDirection.z= z;

		this->shadow.updateShadow = true;

	}

	void DirectionalLight::updateShadowMap() {

		shadowMap.CreateViewMatrix(this->WorldDirection);
		shadowMap.CreateOrthographicMatrix();
	}

	//Point Light

	void PointLight::setLightAttenuation(float constant, float linear, float exp) {

		this->Attenuation.Constant = constant;
		Attenuation.Linear = linear;
		Attenuation.Exp = exp;
	}

	void PointLight::CalcLocalPosition(MyOglWorldTransform::WorldTrans& worldTransform) {


		this->LocalPosition = worldTransform.WorldPosToLocalPos(this->WorldPosition);
	}

	void PointLight::setWorldPosition(float x, float y, float z) {

		this->WorldPosition.x = x;
		this->WorldPosition.y = y;
		this->WorldPosition.z = z;

		this->shadow.updateShadow = true;
	}
	void PointLight::setWorldPosition(MyCamera::BasicCamera& cam) {


		this->WorldPosition = cam.GetPos();
		this->shadow.updateShadow = true;

	}

	void PointLight::updateShadowMap() {
		
		shadowMap.CreatePerspectiveMatrix();

		shadowMap.CreateViewMatrices(this->WorldPosition);
	}

	//Spot Light

	SpotLight::SpotLight() :BaseLight() {
		this->AmbientIntensity = 0;
	}

	void SpotLight::CalcLocalDirectionAndPosition(MyOglWorldTransform::WorldTrans& worldTransform) {

		CalcLocalPosition(worldTransform);
		this->LocalDirection = worldTransform.WorldDirToLocalDir(this->WorldDirection);

	}

	void SpotLight::setWorldDirection(float x, float y, float z) {
		WorldDirection = MyVectorV3::Vector3f(x, y, z);
		this->shadow.updateShadow = true;
	}

	void SpotLight::setWorldDirection(MyCamera::BasicCamera& cam) {

		WorldDirection = cam.GetTarget();
		this->shadow.updateShadow = true;
	}

	void SpotLight::setWorldDirection(float hAngle,float vAngle) {

	}

	void SpotLight::setLightAttenuation(float constant, float linear, float exp) {

		this->Attenuation.Constant = constant;
		Attenuation.Linear = linear;
		Attenuation.Exp = exp;
	}

	void SpotLight::CalcLocalPosition(MyOglWorldTransform::WorldTrans& worldTransform) {


		this->LocalPosition = worldTransform.WorldPosToLocalPos(this->WorldPosition);
	}

	void SpotLight::setWorldPosition(float x, float y, float z) {

		this->WorldPosition.x = x;
		this->WorldPosition.y = y;
		this->WorldPosition.z = z;

		this->shadow.updateShadow = true;
	}
	void SpotLight::setWorldPosition(MyCamera::BasicCamera& cam) {


		this->WorldPosition = cam.GetPos();
		this->shadow.updateShadow = true;

	}

	void SpotLight::updateShadowMap() {

		shadowMap.CreateViewMatrix(
			this->WorldPosition,
			this->WorldDirection);

		shadowMap.perspectiveCamera.FOV = this->OuterCutoff * 2;

		shadowMap.CreatePerspectiveMatrix();

	}

}