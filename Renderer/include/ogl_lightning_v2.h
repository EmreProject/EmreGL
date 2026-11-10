#ifndef  light_v2_h
#define light_v2_h


#include <GL/glew.h>
#include <iostream>
#include "Shaders.h"
#include <tuple>
#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Projection.h"
#include "ogl_Camera.h"
#include "ogl_World.h"
#include "ogl_texture.h"
#include "oglShadowMap.h"


using namespace std;


namespace MyOglLightV2 {



	class BaseLight {

		
	public:

		MyVectorV3::Vector3f Color{ 1, 1, 1 };


		float AmbientIntensity = 0.1f;
		float DiffuseIntensity = 0.8f;
		float SpecularIntensity = 1.0f;

		

		void setColor(float x, float y, float z);


		struct {

			
			bool castShadow = false;
			bool updateShadow = true;//when light's direction or position changes, 
									 //shadow map must be updated
			
			bool isShadowMapAlreadyInit_ = false;

			float maxDistance;
			MyMatrixV3::Matrix4f lightViewMatrix;
			MyMatrixV3::Matrix4f lightProjMatrix;
			MyTextureV2::Texture shadowMapTexture;
		}shadow;

		virtual void updateShadowMap() = 0;
		virtual void gereksiz() {}
	};

	class DirectionalLight :public BaseLight {

	protected:

		MyVectorV3::Vector3f LocalDirection;

	public:

		OglShadow::DirectionalShadowMap shadowMap;
		void updateShadowMap();

		MyVectorV3::Vector3f WorldDirection;

		DirectionalLight();
		void CalcLocalDirection(MyOglWorldTransform::WorldTrans& worldTransform);
		void setWorldDirection(float x, float y, float z);

		MyVectorV3::Vector3f& getLocalDirection() { return this->LocalDirection; };

	};

	struct LightAttenuation
	{
		float Constant = 1.0f;
		float Linear = 0.0f;
		float Exp = 0.0f;
	};


	class PointLight :public BaseLight {

	protected:

		MyVectorV3::Vector3f LocalPosition;

	public:

		OglShadow::PointLightShadowMap shadowMap;
		void updateShadowMap();

		MyVectorV3::Vector3f WorldPosition;
		LightAttenuation Attenuation;

		void CalcLocalPosition(MyOglWorldTransform::WorldTrans& worldTransform);
		MyVectorV3::Vector3f& GetLocalPosition() { return LocalPosition; }
		void setLightAttenuation(float constant, float linear, float exp);
		void setWorldPosition(float x, float y, float z);
		void setWorldPosition(MyCamera::BasicCamera& cam);

	};


	class SpotLight :public BaseLight {

	protected:

		MyVectorV3::Vector3f LocalDirection;
		MyVectorV3::Vector3f LocalPosition;
	public:

		OglShadow::SpotLightShadowMap shadowMap;
		void updateShadowMap();

		MyVectorV3::Vector3f WorldPosition;
		LightAttenuation Attenuation;
		MyVectorV3::Vector3f WorldDirection;
		float InnerCutoff = 12.5; //as angle in degrees
		float OuterCutoff = 15; //as angle in degrees


		SpotLight();

		void CalcLocalPosition(MyOglWorldTransform::WorldTrans& worldTransform);
		MyVectorV3::Vector3f& GetLocalPosition() { return LocalPosition; }
		void setLightAttenuation(float constant, float linear, float exp);
		void setWorldPosition(float x, float y, float z);
		void setWorldPosition(MyCamera::BasicCamera& cam);
		void CalcLocalDirectionAndPosition(MyOglWorldTransform::WorldTrans& worldTransform);
		MyVectorV3::Vector3f& GetLocalDirection() { return LocalDirection; }
		void setWorldDirection(float x, float y, float z);
		void setWorldDirection(float hAngle, float vAngle);
		void setWorldDirection(MyCamera::BasicCamera& cam);
		

	};

}

#endif // ! light_h
