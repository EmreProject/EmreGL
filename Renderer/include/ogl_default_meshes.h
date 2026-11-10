#ifndef ogl_def_meshes_h
#define ogl_def_meshes_h


#include "ogl_anatutorial_v2.h"
#include "ogl_shader_v2.h"

namespace OglMeshes {


	class Material {

	public:


		virtual void Info();

	};

	class LightningMaterial:public Material {

	private:
	

	public:

		MyTextureV2::Texture* albedoMap;
		MyTextureV2::Texture* normalMap;
		MyTextureV2::Texture* depthMap;//parallax mapping

		LightningMaterial(){}
		~LightningMaterial();

		void setAlbedoMap(string file);
		void setNormalMap(string file);
		void setDepthMap(string file);

		void Info();

	};

	


	class MeshesCommon_ {

	protected:

		struct Vertex {

			MyVectorV3::Vector3f m_pos;
			MyVectorV3::Vector2f m_tex;
			MyVectorV3::Vector3f m_normal;
			MyVectorV3::Vector3f m_tangent;
		};
		
		//bittangnt will be calculated on vs shader
		void CalculateTangentSpace(Vertex* vertices,
			int verticesLength,unsigned int* indices, int indicesLength);

		unsigned int VAO, VBO, EBO;
		MyOglWorldTransform::WorldTrans worldTransform;
		Material* material;

	

	public:
		virtual void setGeometry();
		unique_ptr<MyOglShader::LightningShader> shader;
		

		MeshesCommon_();

		
		LightningMaterial* createLightningMaterial();
		
	
		virtual void setShader(); //IT IS FOR 2 SPOT AND POÝNT LÝGHTS
		//JUST MODIFY IT FOR ANY NUMBERS!!!!
		void resetWorldTransform();
		MyOglWorldTransform::WorldTrans* getWorldTransform();
		void bindTextures();
		void BindShader();
		virtual void Draw()=0;
	};


	class PlaneMesh: public MeshesCommon_ {
	private:

		
		
		
	public:
		void setGeometry();
		void Draw();
		PlaneMesh();

	};



}



#endif