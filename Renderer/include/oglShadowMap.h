#ifndef oglShadowMap_h
#define  oglShadowMap_h



#include <GL/glew.h>
#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Projection.h"
#include "ogl_Camera.h"
#include "ogl_texture.h"




namespace MyOglLightV2 {

	class DirectionalLight;
	class SpotLight;
	class PointLight;

};

namespace OglShadow {



	class ShadowMap {

	protected:

		unsigned int SHADOW_WIDTH = 2048;
		unsigned int SHADOW_HEIGHT = 2048;

		unsigned int depthMapFBO;
		unsigned int depthMap;

		virtual void CreateFbo();
		

	
		bool isLoaded_ = false;


	public:

		ShadowMap();
		virtual void Load();
		void BindFBO();
		void ClearFBO();
		void UnbindFBO();

		static float windowWidth;
		static float windowHeight;


		virtual MyMatrixV3::Matrix4f GetLightProjectionViewMatrix() = 0;
		virtual MyMatrixV3::Matrix4f GetLightViewMatrix() = 0;
		virtual MyMatrixV3::Matrix4f GetLightProjectionMatrix() = 0;

		bool isLoaded();
	
		MyTextureV2::Texture GetShadowMapTexture();
		void BindShader();
	};

	class DirectionalShadowMap :public ShadowMap {

	private:

		float scale = 5.0f;


		MyMatrixV3::Matrix4f viewMatrix;
		MyMatrixV3::Matrix4f projectionMatrix;

	public:

		struct {
			float left = -15.0f;
			float right = 15.0f;
			float bottom = -15.0f;
			float top = 15.0f;
			float near_ = 0.1f;
			float far_ = 30.0f;
		}orthographicCamera;

		float minBias = 0.0005f;
		float biasFactor = 0.05f;


		DirectionalShadowMap():ShadowMap(){}

		
		void SetScale(float scale);

		MyMatrixV3::Matrix4f CreateViewMatrix(MyVectorV3::Vector3f direction);
		MyMatrixV3::Matrix4f CreateOrthographicMatrix();

		MyMatrixV3::Matrix4f GetLightProjectionViewMatrix() ;
		MyMatrixV3::Matrix4f GetLightViewMatrix();
		MyMatrixV3::Matrix4f GetLightProjectionMatrix();


		friend class MyOglLightV2::DirectionalLight;

	};


	class SpotLightShadowMap :public ShadowMap {

	private:

		MyVectorV3::Vector3f up = MyVectorV3::Vector3f(0, 1, 0);

		MyMatrixV3::Matrix4f viewMatrix;
		MyMatrixV3::Matrix4f projectionMatrix;
	public:


		struct {
			float FOV = 40.0f;
			float width = 2048.0f;
			float height = 2048.0f;
			float zNear = 0.1f;
			float zFar = 150.0f;
			
		}perspectiveCamera;

		float minBias = 0.0005f;
		float biasFactor = 0.005f;

		SpotLightShadowMap() :ShadowMap() {}

		MyMatrixV3::Matrix4f CreatePerspectiveMatrix();
		MyMatrixV3::Matrix4f CreateViewMatrix(MyVectorV3::Vector3f& position, MyVectorV3::Vector3f& direction);
		MyMatrixV3::Matrix4f GetLightProjectionViewMatrix();
		MyMatrixV3::Matrix4f GetLightViewMatrix();
		MyMatrixV3::Matrix4f GetLightProjectionMatrix();

		friend class MyOglLightV2::SpotLight;
	};


	class PointLightShadowMap :public ShadowMap {

	private:

		MyMatrixV3::Matrix4f CreateViewMatrix(MyVectorV3::Vector3f position, MyVectorV3::Vector3f direction, MyVectorV3::Vector3f up);


		void CreateFbo();

		GLuint shadowMatricesLocation[6];
		GLuint lightPosLocation;

		vector<MyMatrixV3::Matrix4f> viewMatrices;
		MyMatrixV3::Matrix4f projectionMatrix;

	public:

		struct {
			float FOV = 90.0f;
			float width = 2048.0f;
			float height = 2048.0f;
			float zNear = 0.01f;
			float zFar = 50.0f;

		}perspectiveCamera;

		float bias = 0.11;
		float diskRadius = 0.001;

		PointLightShadowMap():ShadowMap(){}

		MyTextureV2::CubeTexture GetShadowMapTexture();

		MyMatrixV3::Matrix4f CreatePerspectiveMatrix();
		vector<MyMatrixV3::Matrix4f> CreateViewMatrices(MyVectorV3::Vector3f lightPos);
		MyMatrixV3::Matrix4f GetLightProjectionViewMatrix(); //JUST FOR FILLING NO USE....
		MyMatrixV3::Matrix4f GetLightViewMatrix();	//JUST FOR FILLING NO USE....
		MyMatrixV3::Matrix4f GetLightProjectionMatrix();
		vector<MyMatrixV3::Matrix4f> GetLightProjectionViewMatrices(); 
		vector<MyMatrixV3::Matrix4f> GetLightViewMatrices();	

		friend class MyOglLightV2::PointLight;
	};


}


#endif // !oglShadowMap_h
