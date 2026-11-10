#include "ogl_shader_v2.h"


namespace MyOglShader {



	GLuint BaseShader::getUniformLocation(string uniName) {

		GLuint uniLoc = glGetUniformLocation(shader->getShaderAddress(), uniName.c_str());
		if (uniLoc == -1) {
			cout << "Warning! Unable to get the location of uniform " << uniName << endl;
			return -1;
		}

		return uniLoc;
	}


	BaseShader::BaseShader() {

		shader.reset(new myShader());
	}


	BaseShader::BaseShader(string vs, string fs) {

		setShader(vs, fs);

	}

	BaseShader::BaseShader(string vs, string fs, string gs) {

		setShader(vs, fs, gs);

	}

	BaseShader::BaseShader(const BaseShader& other) {

		this->shader = make_unique<myShader>(*other.shader); //copy the shader


	}


	BaseShader& BaseShader::operator=(const BaseShader& other){

		
		if (this != &other) {
			this->shader = make_unique<myShader>(*other.shader); //copy the shader
		}
		return *this;

		}

	void BaseShader::setShader(string vs, string fs) {

		shader.reset(new myShader(myVertexSha(vs), myFragmentSha(fs)));
		shader->createProgram();
	}

	void BaseShader::setShader(string vs, string fs, string gs) {

		shader.reset(new myShader(myVertexSha(vs), myFragmentSha(fs),myGeometrySha(gs)));
		shader->createProgram();

	}

	void BaseShader::createShader() {

		shader->createProgram();
	}

	void BaseShader::setShaderTextOnly(string vs, string fs) {

		shader.reset(new myShader(myVertexSha(vs), myFragmentSha(fs)));

	}
	void BaseShader::setShaderTextOnly(string vs, string fs, string gs) {

		shader.reset(new myShader(myVertexSha(vs), myFragmentSha(fs), myGeometrySha(gs)));
	}

	string* BaseShader::getVertexShaderText(){
		return shader->getVertexShaderText();
	}
	string* BaseShader::getFragmentShaderText() {
		return shader->getFragmentShaderText();
	}

	string* BaseShader::getGeometryShaderText() {

		return shader->getGeometryShaderText();
	}


	void BaseShader::bindShader() {
		glUseProgram(shader->getShaderAddress());
	}


	//Lightning Shader


	LightningShader::LightningShader() :BaseShader() {
	}
	LightningShader::LightningShader(string vs, string fs) :BaseShader(vs, fs) {
	}

	//SETTING LOCATION OF UNIFORMS

	void LightningShader::setWVPlocation(string uniName) {
		WVPLoc = getUniformLocation(uniName);
	}
	void LightningShader::setSamplerlocation(string uniName) {
		samplerLoc = getUniformLocation(uniName);
	}
	void LightningShader::setSamplerSpecularExponentLoc(string uniName) {
		samplerSpecularExponentLoc = getUniformLocation(uniName);
	}
	void LightningShader::setNormalMaplocation(string uniName) {
		normalMapLoc = getUniformLocation(uniName);
	}
	void LightningShader::setDepthMapLocation(string uniName) {
		depthMapLoc = getUniformLocation(uniName);
	}
	void LightningShader::setDepthMapScaleLocation(string uniName) {

		this->heightScaleLocation = getUniformLocation(uniName);
	}
	void LightningShader::setCameraLocalPosLoc(string uniName) {
		CameraLocalPosLoc = getUniformLocation(uniName);
	}
	void LightningShader::setNumPointLightsLocation(string uniName) {
		this->NumPointLightsLocation = getUniformLocation(uniName);

	}
	void LightningShader::setNumSpotLightsLocation(string uniName) {

		NumSpotLightsLocation = getUniformLocation(uniName);
	}
	void LightningShader::setMaterialLocation(string ambient, string diffuse, string specular) {

		materialLoc.AmbientColor = getUniformLocation(ambient);
		materialLoc.DiffuseColor = getUniformLocation(diffuse);
		materialLoc.SpecularColor = getUniformLocation(specular);
	}

	void LightningShader::setMaterialSpecularPropertyLocation(string specularIntensity, string specularPower) {


		materialSpecularPropertyLocation.SpecularIntensity= getUniformLocation(specularIntensity);
		materialSpecularPropertyLocation.SpecularPower = getUniformLocation(specularPower);
	}


	void LightningShader::setDirectionalLightLocation(DirLightUniformName& newDir) {

		this->dirLightLocation.Color = getUniformLocation(newDir.Color);
		this->dirLightLocation.AmbientIntensity = getUniformLocation(newDir.AmbientIntensity);
		this->dirLightLocation.DiffuseIntensity = getUniformLocation(newDir.DiffuseIntensity);
		this->dirLightLocation.SpecularIntensity = getUniformLocation(newDir.SpecularIntensity);
		this->dirLightLocation.Direction = getUniformLocation(newDir.Direction);
	}
	void LightningShader::addPointLightLocation(PointLightUniformName& newLight) {
		pointLightLocations.emplace_back();
		auto pointNew = pointLightLocations.end();
		pointNew--;

		pointNew->Color = getUniformLocation(newLight.Color);
		pointNew->AmbientIntensity = getUniformLocation(newLight.AmbientIntensity);
		pointNew->DiffuseIntensity = getUniformLocation(newLight.DiffuseIntensity);
		pointNew->SpecularIntensity = getUniformLocation(newLight.SpecularIntensity);
		pointNew->LocalPosition = getUniformLocation(newLight.LocalPosition);

		pointNew->Atten.Constant = getUniformLocation(newLight.Atten.Constant);
		pointNew->Atten.Exp = getUniformLocation(newLight.Atten.Exp);
		pointNew->Atten.Linear = getUniformLocation(newLight.Atten.Linear);

	}
	void LightningShader::addSpotLightLocation(SpotLightLocationName& newLight) {

		spotLightLocations.emplace_back();
		auto spotNew = spotLightLocations.end();
		spotNew--;

		spotNew->Color = getUniformLocation(newLight.Color);
		spotNew->AmbientIntensity = getUniformLocation(newLight.AmbientIntensity);
		spotNew->DiffuseIntensity = getUniformLocation(newLight.DiffuseIntensity);
		spotNew->SpecularIntensity = getUniformLocation(newLight.SpecularIntensity);
		spotNew->LocalPosition = getUniformLocation(newLight.LocalPosition);
		spotNew->Direction = getUniformLocation(newLight.Direction);
		spotNew->InnerCutoff = getUniformLocation(newLight.InnerCutoff);
		spotNew->OuterCutoff = getUniformLocation(newLight.OuterCutoff);

		spotNew->Atten.Constant = getUniformLocation(newLight.Atten.Constant);
		spotNew->Atten.Exp = getUniformLocation(newLight.Atten.Exp);
		spotNew->Atten.Linear = getUniformLocation(newLight.Atten.Linear);

	}




	//SETTING VALUES TO SHADER UNIFORM
	void LightningShader::setSampler(unsigned int tex) {

		glUniform1i(this->samplerLoc, tex);
	}

	void LightningShader::setSamplerSpecular(unsigned int tex) {

		glUniform1i(this->samplerSpecularExponentLoc, tex);
	}
	void LightningShader::setNormalMap(unsigned int tex) {

		glUniform1i(this->normalMapLoc, tex);
	}

	void LightningShader::setDepthMap(unsigned int tex) {

		glUniform1i(this->depthMapLoc, tex);
	}

	void LightningShader::setDepthMapScale(float scaleFactor) {

		glUniform1f(this->heightScaleLocation, scaleFactor);

	}
	void LightningShader::setWVP(MyMatrixV3::Matrix4f& mat1) {
		
		glUniformMatrix4fv(this->WVPLoc, 1, GL_TRUE, (const GLfloat*)(&mat1.m[0][0]));

	}
	void LightningShader::setCameraLocalPos(MyVectorV3::Vector3f camPos) {

		glUniform3fv(this->CameraLocalPosLoc, 1, (float*)(&camPos));
	}
	void LightningShader::setNumPointLights(int numLight) {
		glUniform1i(this->NumPointLightsLocation, numLight);

	}
	void LightningShader::setNumSpotLights(int numLight) {
		glUniform1i(this->NumSpotLightsLocation, numLight);

	}
	void LightningShader::setMaterial(MyVectorV3::Vector3f ambient, MyVectorV3::Vector3f diffuse, MyVectorV3::Vector3f specular) {

		glUniform3fv(this->materialLoc.AmbientColor, 1, (float*)(&ambient[0]));
		glUniform3fv(this->materialLoc.DiffuseColor, 1, (float*)(&diffuse[0]));
		glUniform3fv(this->materialLoc.SpecularColor, 1, (float*)(&specular[0]));
	}

	void LightningShader::setMaterialSpecularProperty(float specularIntensity, float specularPower) {

		glUniform1f(materialSpecularPropertyLocation.SpecularIntensity, specularIntensity);
		glUniform1f(materialSpecularPropertyLocation.SpecularPower, specularPower);
	
	}


	void LightningShader::setDirectionalLight(MyOglLightV2::DirectionalLight& newDir) {
		glUniform3fv(this->dirLightLocation.Color, 1, (float*)(&newDir.Color.r));
		glUniform1f(this->dirLightLocation.AmbientIntensity, newDir.AmbientIntensity);
		glUniform1f(this->dirLightLocation.DiffuseIntensity, newDir.DiffuseIntensity);
		glUniform1f(this->dirLightLocation.SpecularIntensity, newDir.SpecularIntensity);
		MyVectorV3::Vector3f& LocalDir = newDir.getLocalDirection();
		glUniform3fv(this->dirLightLocation.Direction, 1, (float*)(&LocalDir[0]));
	}

	void LightningShader::addPointLight(MyOglLightV2::PointLight& pointLight, int index) {

		//uniform location of index of pointlight
		auto it = this->pointLightLocations.begin();
		for (int i = 0; i < index; i++) { it++; }
		glUniform3f(it->Color, pointLight.Color.x, pointLight.Color.y, pointLight.Color.z);
		//glUniform3fv(it->Color, 1, (float*)(&pointLight.Color.r));
		glUniform1f(it->AmbientIntensity, pointLight.AmbientIntensity);
		glUniform1f(it->DiffuseIntensity, pointLight.DiffuseIntensity);
		glUniform1f(it->SpecularIntensity, pointLight.SpecularIntensity);
		MyVectorV3::Vector3f& LocalPos = pointLight.GetLocalPosition();
		glUniform3fv(it->LocalPosition, 1, (float*)(&LocalPos[0]));
		glUniform1f(it->Atten.Constant, pointLight.Attenuation.Constant);
		glUniform1f(it->Atten.Linear, pointLight.Attenuation.Linear);
		glUniform1f(it->Atten.Exp, pointLight.Attenuation.Exp);

	}
	void LightningShader::addSpotLight(MyOglLightV2::SpotLight& newLight, int index) {

		auto it = this->spotLightLocations.begin();
		for (int i = 0; i < index; i++) { it++; }
		glUniform3f(it->Color, newLight.Color.x, newLight.Color.y, newLight.Color.z);
		//glUniform3fv(it->Color, 1, (float*)(&newLight.Color.r));
		glUniform1f(it->AmbientIntensity, newLight.AmbientIntensity);
		glUniform1f(it->DiffuseIntensity, newLight.DiffuseIntensity);
		glUniform1f(it->SpecularIntensity, newLight.SpecularIntensity);
		MyVectorV3::Vector3f LocalPos = newLight.GetLocalPosition();
		MyVectorV3::Vector3f LocalDir = newLight.GetLocalDirection();
		MyVectorV3::Vector3f LocalDirNormalized = LocalDir.Normalize();
		//cout << LocalPos.x << ", " << LocalPos.y << ", " << LocalPos.z << endl;
		glUniform3f(it->LocalPosition, LocalPos.x, LocalPos.y, LocalPos.z);
		glUniform3f(it->Direction, LocalDirNormalized.x, LocalDirNormalized.y, LocalDirNormalized.z);
		glUniform1f(it->OuterCutoff, cosf(ToRadian(newLight.OuterCutoff)));
		glUniform1f(it->InnerCutoff, cosf(ToRadian(newLight.InnerCutoff)));
		glUniform1f(it->Atten.Constant, newLight.Attenuation.Constant);
		glUniform1f(it->Atten.Linear, newLight.Attenuation.Linear);
		glUniform1f(it->Atten.Exp, newLight.Attenuation.Exp);


	}



	// SKELETAL SHADER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	void SkelatalShader::setBonesTranformLocation(string uniName, int maxBoneNumber) {


		for (int i = 0; i < maxBoneNumber; i++) {
			ostringstream oss;
			oss << uniName << "[" << i << "]";
			bonesTranformLocation.insert(bonesTranformLocation.end(), getUniformLocation(oss.str()));

		}


	}


	void SkelatalShader::setBoneTranform(int index, MyMatrixV3::Matrix4f& mat1) {

		if (index < this->bonesTranformLocation.size()) {

			auto it = bonesTranformLocation.begin();
			for (int i = 0; i < index; i++) { it++; }

			glUniformMatrix4fv(*it, 1, GL_TRUE, (float*)(&mat1));
		}
		else {

			cout << " SkelatalShader::setBoneTranform, index is bigger than number of bones" << endl;
		}

	}



}