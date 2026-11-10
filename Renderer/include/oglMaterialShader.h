#ifndef ogl_material_uniform_h
#define ogl_material_uniform_h


#include <GL/glew.h>
#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include <variant>
#include "ogl_Projection.h"
#include "ogl_Camera.h"
#include "ogl_lightning_v2.h"
#include "ogl_model_v2.h"
#include "ogl_shader_v2.h"
#include "ogl_texture.h"
#include "ogl_World.h"
#include "ogl_default_meshes.h"

using namespace std;

namespace MaterialUniform {



	template<typename t>
	class BaseUniform {

	protected:


	public:
		t value;
		string uniformName;
		GLint uniformLocation; //set in material class part not in here

		BaseUniform(string uniName) {
			uniformName = uniName;
		
		}

	
		virtual void setUniformToShader() = 0;

	};


	class TextureUniform :public BaseUniform<int> {

	public:
		

		MyTextureV2::Texture texture;

		

		//TextureUniform(string uniformName, int valueShader, GLenum valueTexture); // 0, GL_TEXTURE0
		//void setTexture(string textureFileLocation);
		//void setTexture(GLuint texture);
		TextureUniform(string uniformName);
		TextureUniform(string uniformName,MyTextureV2::Texture& texture_);

		void setTexture(MyTextureV2::Texture& texture_);
		void setUniformToShader();

	};

	class CubeTextureUniform :public BaseUniform<int> {

	public:


		MyTextureV2::CubeTexture texture;



		//TextureUniform(string uniformName, int valueShader, GLenum valueTexture); // 0, GL_TEXTURE0
		//void setTexture(string textureFileLocation);
		//void setTexture(GLuint texture);
		CubeTextureUniform(string uniformName);
		CubeTextureUniform(string uniformName, MyTextureV2::CubeTexture& texture_);

		void setTexture(MyTextureV2::CubeTexture& texture_);
		void setUniformToShader();

	};

	class BoolUniform :public BaseUniform<bool> {

	public:


		BoolUniform(string uniformName, bool val);
		void setUniformToShader();

	};

	class IntUniform :public BaseUniform<int> {

	public:

	
		IntUniform(string uniformName, int val);
		void setUniformToShader();

	};


	class FloatUniform:public BaseUniform<float> {

	public:
		
		FloatUniform(string uniformName, float val);
		void setUniformToShader();

	};


	class Vec2fUniform : public BaseUniform< MyVectorV3::Vector2f> {

	public:

	
		Vec2fUniform(string uniName, MyVectorV3::Vector2f vec2_);
		Vec2fUniform(string uniName, float x, float y);
		void setUniformToShader();


	};

	class Vec3fUniform : public BaseUniform< MyVectorV3::Vector3f> {

	public:


	
		Vec3fUniform(string uniName, MyVectorV3::Vector3f vec3_);
		Vec3fUniform(string uniName, float x, float y,float z);
		void setUniformToShader();


	};

	class Vec4fUniform : public BaseUniform< MyVectorV3::Vector4f> {

	public:


		
		Vec4fUniform(string uniName, MyVectorV3::Vector4f vec4_);
		Vec4fUniform(string uniName, float x, float y, float z,float w);
		void setUniformToShader();


	};

	class Mat3fUniform : public BaseUniform< MyMatrixV3::Matrix3f> {

	public:

	
		Mat3fUniform(string uniName, MyMatrixV3::Matrix3f mat3_);
		void setUniformToShader();


	};

	class Mat4fUniform : public BaseUniform< MyMatrixV3::Matrix4f> {

	public:

		Mat4fUniform(string uniName, MyMatrixV3::Matrix4f mat4_);
		void setUniformToShader();


	};



	using UniformVariant = std::variant<
		TextureUniform,CubeTextureUniform, IntUniform, FloatUniform, 
		Vec2fUniform, Vec3fUniform, Vec4fUniform,
		Mat3fUniform, Mat4fUniform, BoolUniform >;


	//....

}

namespace MaterialShaderHooks {


	class Hook {

	protected:

		string shaderTagName;
		string hookFileLoc;



		string readHookFile();

	public:


		Hook(string shaderTagNam) :shaderTagName(shaderTagNam) {}
		Hook(string shaderTagNam, string hookFileLo) :shaderTagName(shaderTagNam), hookFileLoc(hookFileLo) {}


		virtual void Replace(string* shader); //vs or frag

	};

	class CommonHook : public Hook { //hem fragment hem vertex shader�nki icin ayn�s�
	public:
		CommonHook();
	};


	//VERTEX SHADER HOOKS
	class AttributeFromGeometryHook : public Hook {
	private:

		list<string> attributesFromGeometry;

	public:
		AttributeFromGeometryHook(); 
		void addAttributeShaderCode(string attrib);
		void Replace(string* shader);
		void Reset();
	};

	class AttributesBone : public Hook {
	public:
		AttributesBone();
	};

	class SkinningVertexMain : public Hook {
	public:
		SkinningVertexMain();
	};


	class ColorVertexHook : public Hook {
	public:
		ColorVertexHook();
	};

	class BeginVertexHook : public Hook {
	public:
		BeginVertexHook();
	};

	class BeginColorVertexHook : public Hook {
	public:
		BeginColorVertexHook();
	};

	class EndColorVertexHook : public Hook {
	public:
		EndColorVertexHook();
	};

	class EnvironmentMapVertex : public Hook {
	public:
		EnvironmentMapVertex();
	};


	class EnvironmentMapVertexBegin : public Hook {
	public:
		EnvironmentMapVertexBegin();
	};




	//FRAGMENT SHADER HOOKS

	class AssimpTransparency : public Hook {
	public:
		AssimpTransparency();
	};

	class ColorFragmentHook : public Hook {
	public:
		ColorFragmentHook();
	};

	class TextureFragmentHook : public Hook {
	public:
		TextureFragmentHook();
	};

	class ColorAndTextureFragmentHook : public Hook {
	public:
		ColorAndTextureFragmentHook();
	};

	class LightCommonHook : public Hook {
	public:
		bool isNormalMap = false;
		
		LightCommonHook();
		void Replace(string* shader);
	};

	class LightDirectionalHook : public Hook {
	public:
		LightDirectionalHook();
	};


	class LightPointHook : public Hook {
	public:
		LightPointHook();
	};

	class LightSpotHook : public Hook {
	public:
		LightSpotHook();
	};


	class BeginFragmentHook : public Hook {
	public:
		BeginFragmentHook();
	};

	class BeginFragmentResultHook : public Hook {
	public:
		BeginFragmentResultHook();
	};

	class FragmentColorResultHook : public Hook {
	public:
		FragmentColorResultHook();
	};

	class FragmentTextureResultHook : public Hook {
	public:
		FragmentTextureResultHook();
	};

	class FragmentColorAndTextureResultHook : public Hook {
	public:
		FragmentColorAndTextureResultHook();
	};

	class BeginFragmentManualGammaHook : public Hook {
	public:
		BeginFragmentManualGammaHook();
	};

	class NormalMapHook : public Hook {
	public:
		NormalMapHook();
	};

	class DisplacementMapVertexHook : public Hook {
	public:
		DisplacementMapVertexHook();
	};


	class DisplacementMapVertexBeginHook : public Hook {
	public:
		DisplacementMapVertexBeginHook();
	};

	class BeginNormalFragmentHook : public Hook {
	public:
		BeginNormalFragmentHook();
	};


	class BeginNormalFragmentDoubleSideHook : public Hook {
	public:
		BeginNormalFragmentDoubleSideHook();
	};

	class InNormalMapHook : public Hook {
	public:
		InNormalMapHook();
	};

	class OutNormalMapHook : public Hook {
	public:
		OutNormalMapHook();
	};


	class OutFragmentColorHook : public Hook {
	public:
		OutFragmentColorHook();
	};

	class BeginNormalMapVertexHook : public Hook {

	public:

		BeginNormalMapVertexHook();
	};

	class BeginNormalMapFragmentHook : public Hook {

	public:

		BeginNormalMapFragmentHook();
	};

	class LightMainBeginHook : public Hook {
	public:
		LightMainBeginHook();
	};

	class LightMainDirectionalHook : public Hook {
	public:
		LightMainDirectionalHook();
	};

	class LightMainPointHook : public Hook {
	public:
		LightMainPointHook();
	};

	class LightMainSpotHook : public Hook {
	public:
		LightMainSpotHook();
	};

	class LightMainEndHook : public Hook {
	public:
		LightMainEndHook();
	};

	class DoubleSideDirectionalFragmentHook : public Hook {
	public:
		DoubleSideDirectionalFragmentHook();
	};

	class DoubleSidePointFragmentHook : public Hook {
		public:
			DoubleSidePointFragmentHook();
	};

	class DoubleSideSpotFragmentHook : public Hook {
	public:
		DoubleSideSpotFragmentHook();
	};


	class ShadowMapVertexHook : public Hook {
	public:
		ShadowMapVertexHook();
	};

	class ShadowMapVertexBeginHook : public Hook {
	public:
		ShadowMapVertexBeginHook();
	};

	class ShadowMapFragmentHook : public Hook {
	public:
		ShadowMapFragmentHook();
	};

	class ShadowMapDirectionalLightHook : public Hook {
	public:
		ShadowMapDirectionalLightHook();
	};

	class ShadowMapSpotLightHook : public Hook {
	public:
		ShadowMapSpotLightHook();
	};

	class ShadowMapPointLightHook : public Hook {
	public:
		ShadowMapPointLightHook();
	};


	class EndFragmentGammaEnabledHook : public Hook {
	public:
		EndFragmentGammaEnabledHook();
	};

	class EndFragmentResultHook : public Hook {
	public:
		EndFragmentResultHook();
	};

	class EndFragmentResultAssimp : public Hook {
	public:
		EndFragmentResultAssimp();
	};

	class EnvironmentMapFragment : public Hook {
	public:
		EnvironmentMapFragment();
	};

	class EnvironmentMapNormalMapFragment : public Hook {
	public:
		EnvironmentMapNormalMapFragment();
	};

	class EndFragmentResultEnvironmentMap : public Hook {
	public:
		EndFragmentResultEnvironmentMap();
	};


}

#endif