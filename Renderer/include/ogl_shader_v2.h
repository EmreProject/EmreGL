#ifndef ogl_shader_v2_h
#define ogl_shader_v2_h



#include <GL/glew.h>
#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Projection.h"
#include "ogl_Camera.h"
#include "ogl_lightning_v2.h"
#include "ogl_model_v2.h"
#include "ogl_texture.h"
#include "ogl_World.h"


//S�MD�L�K YEN� SHADER, VE YEN� TUTOR�ALLER TAM DEG�L ONLARI YAP

namespace MyOglShader {


	class BaseShader {


	public:

		GLuint getUniformLocation(string uniName);

		unique_ptr<myShader> shader;


		BaseShader();
		BaseShader(string vs, string fs);
		BaseShader(string vs, string fs,string gs);
		BaseShader(const BaseShader& other);


		BaseShader& operator=(const BaseShader& other);
		void setShader(string vs, string fs);
		void setShader(string vs, string fs,string gs);
		void setShaderTextOnly(string vs, string fs); //�stteki gibi shader
		//olusturmak yerine sadece vs,fs etxt set eder, vs ve fs gene location!!!
		void setShaderTextOnly(string vs, string fs,string gs);

		string* getVertexShaderText();
		string* getFragmentShaderText();
		string* getGeometryShaderText();

		void createShader();
		void bindShader();


	};


	class LightningShader : public BaseShader {

	protected:

		//extra types**
		//*
		struct PointLightLocation {
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint DiffuseIntensity;
			GLuint SpecularIntensity;
			GLuint LocalPosition;

			struct
			{
				GLuint Constant;
				GLuint Linear;
				GLuint Exp;
			} Atten;
		};

		struct SpotLightLocation {
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint DiffuseIntensity;
			GLuint SpecularIntensity;
			GLuint LocalPosition;
			GLuint Direction;
			GLuint InnerCutoff;
			GLuint OuterCutoff;
			struct {
				GLuint Constant;
				GLuint Linear;
				GLuint Exp;
			} Atten;
		};

		struct DirLightLocation {
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint Direction;
			GLuint DiffuseIntensity;
			GLuint SpecularIntensity;
			
		};


		struct MaterialSpecularPropertyLocation {

			GLuint SpecularIntensity;
			GLuint SpecularPower;

		};
		//**
		//extra types*


		GLuint WVPLoc;
		GLuint samplerLoc;
		GLuint samplerSpecularExponentLoc;
		GLuint normalMapLoc;
		GLuint depthMapLoc;
		GLuint CameraLocalPosLoc;
		GLuint NumPointLightsLocation;
		GLuint NumSpotLightsLocation;
		GLuint heightScaleLocation;


		struct {
			GLuint AmbientColor;
			GLuint DiffuseColor;
			GLuint SpecularColor;
		} materialLoc;


		DirLightLocation dirLightLocation;
		list< PointLightLocation> pointLightLocations;
		list< SpotLightLocation> spotLightLocations;
		MaterialSpecularPropertyLocation materialSpecularPropertyLocation;


	public:


		struct DirLightUniformName {
			string Color;
			string AmbientIntensity;
			string Direction;
			string DiffuseIntensity;
			string SpecularIntensity;

		};
		struct PointLightUniformName {
			string Color;
			string AmbientIntensity;
			string LocalPosition;
			string DiffuseIntensity;
			string SpecularIntensity;

			struct
			{
				string Constant;
				string Linear;
				string Exp;
			} Atten;
		};

		struct SpotLightLocationName {
			string Color;
			string AmbientIntensity;
			string DiffuseIntensity;
			string SpecularIntensity;
			string LocalPosition;
			string Direction;
			string InnerCutoff;
			string OuterCutoff;
			struct {
				string Constant;
				string Linear;
				string Exp;
			} Atten;
		};



		LightningShader();
		LightningShader(string vs, string fs);



		//FOR SHADER LOCATIONS
		void setWVPlocation(string uniName);
		void setSamplerlocation(string uniName);
		void setSamplerSpecularExponentLoc(string uniName);
		void setNormalMaplocation(string uniName);
		void setDepthMapLocation(string uniName);
		void setDepthMapScaleLocation(string uniName);
		void setCameraLocalPosLoc(string uniName);
		void setNumPointLightsLocation(string uniName);
		void setNumSpotLightsLocation(string uniName);
		void setMaterialLocation(string ambient, string diffuse, string specular);
		void setMaterialSpecularPropertyLocation(string specularIntensity, string specularPower);

		void setDirectionalLightLocation(DirLightUniformName& newDir);
		void addPointLightLocation(PointLightUniformName& newLight);
		void addSpotLightLocation(SpotLightLocationName& newLight);



		//FOR SETTING SHADER THE VALUE
		void setWVP(MyMatrixV3::Matrix4f& mat1);
		void setSampler(unsigned int);//albedo
		void setSamplerSpecular(unsigned int);//specular map
		void setNormalMap(unsigned int);//normal map
		void setDepthMap(unsigned int);//depth map
		void setDepthMapScale(float);//depth map scale factor
		void setCameraLocalPos(MyVectorV3::Vector3f camPos);
		void setNumPointLights(int numLight);
		void setNumSpotLights(int numLight);
		void setMaterial(MyVectorV3::Vector3f ambient, MyVectorV3::Vector3f diffuse, MyVectorV3::Vector3f specular);
		void setMaterialSpecularProperty(float specularIntensity, float specularPower);
		

		void setDirectionalLight(MyOglLightV2::DirectionalLight& newDir);
		void addPointLight(MyOglLightV2::PointLight& newLight, int index);
		void addSpotLight(MyOglLightV2::SpotLight& newLight, int index);
	};



	class SkelatalShader : public LightningShader {




	public:


		list<GLuint> bonesTranformLocation;

		SkelatalShader() :LightningShader() {}
		SkelatalShader(string vs, string fs) :LightningShader(vs, fs) {}



		//FOR SHADER LOCATIONS
		void setBonesTranformLocation(string uniName, int maxBoneNumber);//vertex shader icindeki max bone, 200 gibi

		//FOR SETTING SHADER THE VALUE
		void setBoneTranform(int index, MyMatrixV3::Matrix4f& mat1);




	};



}



#endif