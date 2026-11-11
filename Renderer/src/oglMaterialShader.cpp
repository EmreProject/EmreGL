#include "oglMaterialShader.h"



namespace MaterialUniform {


	TextureUniform::TextureUniform(string uniformName) :BaseUniform<int>(uniformName) {
		this->value = -1;//no texture yet

	}
	TextureUniform::TextureUniform(string uniformName, MyTextureV2::Texture& texture_) :BaseUniform<int>(uniformName) {

	
		this->texture = texture_;

	}
	

	void TextureUniform::setTexture(MyTextureV2::Texture& texture_) {

	
		this->texture = texture_;

	}

	void TextureUniform::setUniformToShader() {

		int avaliableTextureUnit = MyTextureV2::Texture::GetCurrentTextureUnitIndex();
		texture.BindTexture(GL_TEXTURE0 + avaliableTextureUnit); 
		//cout <<this->uniformName<<", "<< avaliableTextureUnit <<", "<<uniformLocation<< endl;
		glUniform1i(this->uniformLocation, avaliableTextureUnit);
		
	}

	//void setTexture(GLuint texture);
	CubeTextureUniform::CubeTextureUniform(string uniformName) :BaseUniform<int>(uniformName) {

		this->value = -1;//no texture yet

	}
	CubeTextureUniform::CubeTextureUniform(string uniformName, MyTextureV2::CubeTexture& texture_) :BaseUniform<int>(uniformName) {

		this->texture = texture_;


	}

	void CubeTextureUniform::setTexture(MyTextureV2::CubeTexture& texture_) {

		this->texture = texture_;


	}
	void CubeTextureUniform::setUniformToShader() {


		int avaliableTextureUnit = MyTextureV2::Texture::GetCurrentTextureUnitIndex();
		texture.BindTexture(GL_TEXTURE0 + avaliableTextureUnit);
		//cout <<this->uniformName<<", "<< avaliableTextureUnit <<", "<<uniformLocation<< endl;
		glUniform1i(this->uniformLocation, avaliableTextureUnit);
	

	}

	BoolUniform::BoolUniform(string uniformName, bool val) : BaseUniform<bool>(uniformName) {

		this->value = val;
	}

	void BoolUniform::setUniformToShader() {

		if (value == true) {//if value is true, then set to 1, else set to 0
			glUniform1i(this->uniformLocation, 1);
		}
		else {
			glUniform1i(this->uniformLocation, 0);
		}
		
	}

	
	IntUniform::IntUniform(string uniformName, int val): BaseUniform<int>(uniformName) {

		this->value = val;
	}
	
	void IntUniform::setUniformToShader() {

		glUniform1i(this->uniformLocation, value);
	}

	FloatUniform::FloatUniform(string uniformName, float val) : BaseUniform<float>(uniformName) {

		this->value = val;
	}
	
	void FloatUniform::setUniformToShader() {

		glUniform1f(this->uniformLocation, value);
	}

	Vec2fUniform::Vec2fUniform(string uniName, float x, float y) : BaseUniform< MyVectorV3::Vector2f>(uniformName){


		this->value = MyVectorV3::Vector2f(x,y);

	}

	Vec2fUniform::Vec2fUniform(string uniName, MyVectorV3::Vector2f vec2_) : BaseUniform< MyVectorV3::Vector2f>(uniName) {

		this->value = vec2_;

	}

	

	void Vec2fUniform::setUniformToShader() {

		glUniform2f(this->uniformLocation, value.x,value.y);
	}


	Vec3fUniform::Vec3fUniform(string uniName, float x, float y,float z) : BaseUniform< MyVectorV3::Vector3f>(uniName) {


		this->value = MyVectorV3::Vector3f(x, y, z);

	}

	Vec3fUniform::Vec3fUniform(string uniName, MyVectorV3::Vector3f vec3_) : BaseUniform< MyVectorV3::Vector3f>(uniName) {

		this->value = vec3_;
		

	}
	
	void Vec3fUniform::setUniformToShader() {

		glUniform3f(this->uniformLocation, value.x, value.y, value.z);
		//cout << this->uniformName << ": " << value.x << ", " << value.y 
			 //<< ", " << value.z << endl;
	}


	Vec4fUniform::Vec4fUniform(string uniName, float x, float y, float z,float w) :BaseUniform< MyVectorV3::Vector4f>(uniName) {


		this->value = MyVectorV3::Vector4f(x,y,z,w);
		

	}

	Vec4fUniform::Vec4fUniform(string uniName, MyVectorV3::Vector4f vec4_) :BaseUniform<MyVectorV3::Vector4f>(uniName) {

		this->value = vec4_;

	}
	

	void Vec4fUniform::setUniformToShader() {

		glUniform4f(this->uniformLocation, value.x, value.y, value.z,value.w);
	}

	Mat3fUniform::Mat3fUniform(string uniName, MyMatrixV3::Matrix3f mat3_) :BaseUniform<MyMatrixV3::Matrix3f>(uniName) {

		this->value = mat3_;
	}

	

	void Mat3fUniform::setUniformToShader() {

		glUniformMatrix3fv(this->uniformLocation, 1, GL_TRUE, (const GLfloat*)(&value.m[0][0]));
	
	}

	Mat4fUniform::Mat4fUniform(string uniName, MyMatrixV3::Matrix4f mat4_) :BaseUniform< MyMatrixV3::Matrix4f>(uniName) {

		this->value = mat4_;
	}


	

	void Mat4fUniform::setUniformToShader() {

		glUniformMatrix4fv(this->uniformLocation, 1, GL_TRUE, (const GLfloat*)(&value.m[0][0]));

	}

}



namespace MaterialShaderHooks {


	string Hook::readHookFile() {

		ifstream iff;
		iff.open(this->hookFileLoc);
		ostringstream oss;

		if (iff.is_open()) {

			while (!iff.eof()) {

				string line;
				getline(iff, line, '\n');
				oss << line<<endl;

			}

			iff.close();

			
		}
		else {

			cout << "Hook file of " << shaderTagName << " is not found..." << endl;
		}
		

		return oss.str();
		
	}

	void Hook::Replace(string* shader) {

		string hookCode = readHookFile();

		string::size_type find = shader->find(shaderTagName);
		if (find != string::npos) {

			shader->replace(find, shaderTagName.size(), hookCode);
		}

	}


	CommonHook::CommonHook() :Hook("<common>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/common.txt") {
		
	}


	//VERTEX SHADER HOOKS

	AttributeFromGeometryHook::AttributeFromGeometryHook() :Hook("<attributes_from_geometry>") {

	}

	void AttributeFromGeometryHook::addAttributeShaderCode(string attrib) {
		bool isAlreadyAdded = false;
		/*
		attrib.erase(0, attrib.find_first_of(")")+1);
		
		for_each(attributesFromGeometry.begin(), attributesFromGeometry.end(), [&attrib,&isAlreadyAdded](string existingAttrib) {
			
			existingAttrib.erase(0, attrib.find_first_of(")") + 1);
			if (existingAttrib == attrib) {
				isAlreadyAdded = true;;
			}
			});

		*/
		if (!isAlreadyAdded) {
		this->attributesFromGeometry.push_back(attrib);
		}

	}

	void AttributeFromGeometryHook::Replace(string* shader) {

		ostringstream oss;
		list<string>::iterator it = this->attributesFromGeometry.begin();
		for (; it != attributesFromGeometry.end(); it++) {

			oss << *it << endl;
		}


		string::size_type find = shader->find(shaderTagName);
		if (find != string::npos) {

		shader->replace(find, shaderTagName.size(), oss.str());
		}

	}

	void AttributeFromGeometryHook::Reset() {

		this->attributesFromGeometry.erase(this->attributesFromGeometry.begin(),
			this->attributesFromGeometry.end());

	}

	AttributesBone::AttributesBone() : Hook("<attributes_bone>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/attributes_bone.txt") {
	}

	SkinningVertexMain::SkinningVertexMain() : Hook("<skinning_vertex_main>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/skinning_vertex_main.txt") {
	}

	ColorVertexHook::ColorVertexHook() : Hook("<color_vertex>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/color_vertex.txt") {

	}

	BeginVertexHook::BeginVertexHook(): Hook("<begin_vertex>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/begin_vertex.txt") {

	}

	BeginColorVertexHook::BeginColorVertexHook() : Hook("<begin_color_vertex>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/begin_color_vertex.txt") {

	}

	EndColorVertexHook::EndColorVertexHook() : Hook("<end_color_vertex>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/end_color_vertex.txt") {

	}

	EnvironmentMapVertex::EnvironmentMapVertex() :Hook("<environment_map_vertex>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/environment_map_vertex.txt") {

	}

	EnvironmentMapVertexBegin::EnvironmentMapVertexBegin() :Hook("<environment_map_vertex_begin>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/environment_map_vertex_begin.txt") {

	}

	//FRAGMENT SHADER HOOKS

	AssimpTransparency::AssimpTransparency() : Hook("<assimp_transparency>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/assimp_transparency.txt") {

	}


	ColorFragmentHook::ColorFragmentHook() : Hook("<color_fragment>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/color_fragment.txt") {

	}

	TextureFragmentHook::TextureFragmentHook() : Hook("<texture_fragment>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/texture_fragment.txt") {

	}

	ColorAndTextureFragmentHook::ColorAndTextureFragmentHook() : Hook("<color_and_texture_fragment>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/color_and_texture_fragment.txt") {

	}


	LightCommonHook::LightCommonHook() : Hook("<light_common>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/light_common.txt") {

	}

	void LightCommonHook::Replace(string* shader) {

		Hook::Replace(shader);

		if (isNormalMap) {

		function<void(string tag, string fileLocation)> replaceNormalMapTag = [&shader](string tag, string fileLocation) {

			string::size_type find = shader->find(tag);

			if (find != string::npos) {

				ifstream fileReader;
				ostringstream oss;
				fileReader.open(fileLocation);

				if (fileReader.is_open()) {
					string line;
					while(getline(fileReader,line,'\n')){
						oss << line << endl;
					}
					
					shader->replace(find, tag.size(), oss.str());

				}
				else {
					cout << "light common hook replace method file is not opened" << endl;
				}

			}

		};

		replaceNormalMapTag("<normal_map_light_internal>",
			ENGINE_MATERIAL_SHADERS_DIR "/hooks/normal_map_light_internal.txt");

		replaceNormalMapTag("<normal_map_directional_light>",
			ENGINE_MATERIAL_SHADERS_DIR "/hooks/normal_map_directional_light.txt");

		replaceNormalMapTag("<normal_map_point_light>",
			ENGINE_MATERIAL_SHADERS_DIR "/hooks/normal_map_point_light.txt");

		replaceNormalMapTag("<normal_map_spot_light>",
			ENGINE_MATERIAL_SHADERS_DIR "/hooks/normal_map_spot_light.txt");

		
		}

	}


	DisplacementMapVertexHook::DisplacementMapVertexHook() : Hook("<displacement_map_vertex>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/displacement_map_vertex.txt") {



	}

	DisplacementMapVertexBeginHook::DisplacementMapVertexBeginHook() : Hook("<displacement_map_vertex_begin>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/displacement_map_vertex_begin.txt") {



	}

	BeginNormalMapVertexHook::BeginNormalMapVertexHook() :Hook("<begin_normal_map_vertex>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/begin_normal_map_vertex.txt") {



	}

	BeginNormalMapFragmentHook::BeginNormalMapFragmentHook() :Hook("<begin_normal_map_fragment>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/begin_normal_map_fragment.txt") {



	}



	InNormalMapHook::InNormalMapHook() : Hook("<in_normal_map>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/in_normal_map.txt" ) {



	}


	OutNormalMapHook::OutNormalMapHook() : Hook("<out_normal_map>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/out_normal_map.txt") {



	}

	OutFragmentColorHook::OutFragmentColorHook() : Hook("<out_fragment_color>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/out_fragment_color.txt") {



	}


	LightDirectionalHook::LightDirectionalHook() : Hook("<light_directional>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/light_directional.txt") {

	}


	LightPointHook::LightPointHook() : Hook("<light_point>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/light_point.txt") {

	}
	
	LightSpotHook::LightSpotHook() : Hook("<light_spot>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/light_spot.txt") {

	}

	BeginFragmentHook::BeginFragmentHook() : Hook("<begin_fragment>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/begin_fragment.txt") {

	}

	BeginFragmentResultHook::BeginFragmentResultHook() : Hook("<begin_fragment_result>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/begin_fragment_result.txt") {

	}

	FragmentColorResultHook::FragmentColorResultHook() : Hook("<fragment_color_result>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/fragment_color_result.txt") {

	}

	FragmentTextureResultHook::FragmentTextureResultHook() : Hook("<fragment_texture_result>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/fragment_texture_result.txt") {

	}

	FragmentColorAndTextureResultHook::FragmentColorAndTextureResultHook() : Hook("<fragment_color_and_texture_result>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/fragment_color_and_texture_result.txt") {

	}

	BeginFragmentManualGammaHook::BeginFragmentManualGammaHook() : Hook("<begin_fragment_manual_gamma>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/begin_fragment_manual_gamma.txt") {
	}

	NormalMapHook::NormalMapHook() : Hook("<normal_map>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/normal_map.txt") {



	}

	BeginNormalFragmentHook::BeginNormalFragmentHook() : Hook("<begin_normal_fragment>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/begin_normal_fragment.txt") {

	}

	BeginNormalFragmentDoubleSideHook::BeginNormalFragmentDoubleSideHook() : Hook("<begin_normal_fragment_double_side>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/begin_normal_fragment_double_side.txt") {
	}

	LightMainBeginHook::LightMainBeginHook() : Hook("<light_main_begin>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/light_main_begin.txt") {

	}

	LightMainDirectionalHook::LightMainDirectionalHook() : Hook("<light_main_directional>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/light_main_directional.txt") {

	}

	LightMainPointHook::LightMainPointHook() : Hook("<light_main_point>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/light_main_point.txt") {

	}

	LightMainSpotHook::LightMainSpotHook() : Hook("<light_main_spot>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/light_main_spot.txt") {

	}

	LightMainEndHook::LightMainEndHook() : Hook("<light_main_end>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/light_main_end.txt") {

	}

	DoubleSideDirectionalFragmentHook::DoubleSideDirectionalFragmentHook() : Hook("<double_side_directional_fragment>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/double_side_directional_fragment.txt") {
	}

	DoubleSidePointFragmentHook::DoubleSidePointFragmentHook() : Hook("<double_side_point_fragment>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/double_side_point_fragment.txt") {
	}

	DoubleSideSpotFragmentHook::DoubleSideSpotFragmentHook() : Hook("<double_side_spot_fragment>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/double_side_spot_fragment.txt") {
	}

	ShadowMapVertexHook::ShadowMapVertexHook() : Hook("<shadow_map_vertex>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/shadow_map_vertex.txt") {
	}	

	ShadowMapVertexBeginHook::ShadowMapVertexBeginHook() : Hook("<shadow_map_vertex_begin>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/shadow_map_vertex_begin.txt") {
	}

	ShadowMapFragmentHook::ShadowMapFragmentHook() : Hook("<shadow_map_fragment>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/shadow_map_fragment.txt") {
	}

	ShadowMapDirectionalLightHook::ShadowMapDirectionalLightHook() : Hook("<shadow_map_directional_light>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/shadow_map_directional_light.txt") {
	}

	ShadowMapSpotLightHook::ShadowMapSpotLightHook() : Hook("<shadow_map_spot_light>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/shadow_map_spot_light.txt") {
	}

	ShadowMapPointLightHook::ShadowMapPointLightHook() : Hook("<shadow_map_point_light>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/shadow_map_point_light.txt") {
	}


	EndFragmentGammaEnabledHook::EndFragmentGammaEnabledHook() : Hook("<end_fragment_gamma_enabled>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/end_fragment_gamma_enabled.txt") {
	}

	EndFragmentResultHook::EndFragmentResultHook() : Hook("<end_fragment_result>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/end_fragment_result.txt") {

	}

	EndFragmentResultAssimp::EndFragmentResultAssimp() : Hook("<end_fragment_result_assimp>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/end_fragment_result_assimp.txt") {

	}

	EnvironmentMapFragment::EnvironmentMapFragment() :Hook("<environment_map_fragment>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/environment_map_fragment.txt") {

	}

	EnvironmentMapNormalMapFragment::EnvironmentMapNormalMapFragment() :Hook("<environment_map_normal_map_fragment>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/environment_map_normal_map_fragment.txt") {



	}

	EndFragmentResultEnvironmentMap::EndFragmentResultEnvironmentMap() :Hook("<end_fragment_result_environment_map>",
		ENGINE_MATERIAL_SHADERS_DIR "/hooks/end_fragment_result_environment_map.txt") {

	}

	
}




