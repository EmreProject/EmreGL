#ifndef ogl_meshes_modern_h
#define ogl_meshes_modern_h



#include <GL/glew.h>
#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Projection.h"
#include "ogl_Camera.h"
#include "ogl_lightning_v2.h"
#include "ogl_model_v2.h"
#include "ogl_shader_v2.h"
#include "ogl_texture.h"
#include "ogl_World.h"
#include "ogl_default_meshes.h"
#include "oglMaterialShader.h"
#include "oglMaterialConstants.h"
#include "oglComputeShader.h"
#include <map>


using namespace std;


namespace OglScene {
	class Scene;
}

namespace OglMouseEvents {
	class MouseEventHandler;
}

namespace OglRenderer {
	class Renderer;
}


namespace OglObject3D {

	//ATTRIBUTES

	struct BufferAttribute {
	private:


		template<typename t>
		void setAttributeType() {
			cout << "----BufferAttribute type of this not inluced in oglMeshes.h----" << endl;
		}

		template<>
		void setAttributeType<int>() {
			this->value_type = "int";
		}
		template<>
		void setAttributeType<unsigned int>() {
			this->value_type = "uint";
		}
		template<>
		void setAttributeType<float>() {
			this->value_type = "float";
		}

	public:

		vector<float> values;//hold everything like int, uint as float
		string value_type = "float";//default
		string shaderName;
		int count; //her vertex de kac tane position da 3 , texturada 2 gibi
		int layout_ = -1;//set in buffergeometry

		BufferAttribute();
		template<typename t>
		BufferAttribute(vector<t> values_, int count_) {

			auto it = values_.begin();
			for(; it != values_.end(); it++) {
				this->values.push_back(static_cast<float>(*it));
			}
			this->count = count_;

			setAttributeType<t>();
		}

		string getAttributeNameForHook();

	};

	struct InstancedAttribute : public BufferAttribute{

	public:

		InstancedAttribute();

		template<typename t>
		InstancedAttribute(vector<t> values_, int count_) : BufferAttribute(values_,count_){
		}


	};

	struct SSBOAttribute {
	private:

		GLuint ssbo;
		int binding;

	public:
		
		SSBOAttribute(GLuint ssbo, int binding);
		SSBOAttribute(){}

		void Use();

		void setSSBO(GLuint ssbo, int binding);
	
	};


	//GEOMETRIES
	enum class GeometryType {

		BufferGeometry,PlaneGeometry,BoxGeometry,SphereGeometry, LineGeometry
	};

	class Geometry {
	protected:
		void sortAttributes();
		list<BufferAttribute> bufferAttributes; //exta attributes if i set
		list<InstancedAttribute> instancedBufferAttributes;
		list<SSBOAttribute> ssboAttributes; //ssbo attributes if i set

	public:


		Geometry();
		Geometry(const Geometry& other);

		Geometry& operator=(const Geometry& other);

		vector<unsigned int> indices;

		//default attributes!!!!!
		BufferAttribute position;
		BufferAttribute texcoord;
		BufferAttribute normal;
		BufferAttribute tangent;


		list<BufferAttribute> getExtraBufferAttributes();
		list<InstancedAttribute> getInstancedBufferAttributes();
		list<SSBOAttribute> getExtraSSBOAttributes();
		void setAttribute(string shaderName, BufferAttribute attribute);
		void setAttribute(string shaderName, int layout, BufferAttribute attribute);
		void setInstancedAttribute(string shaderName, InstancedAttribute attribute);
		void setInstancedAttribute(string shaderName, int layout, InstancedAttribute attribute);
		void addSSBOAttribute(SSBOAttribute attribute);
		
		

		int getBufferNumber();
		bool isAttributeExist(string shaderName);

		virtual GeometryType GetGeometryType() = 0;
		friend class RawShaderMaterial;
		friend class LineMaterial;

	};
	class BufferGeometry: public Geometry {


	public:

		
		BufferGeometry();
		BufferGeometry(const BufferGeometry& other);

		BufferGeometry& operator=(const BufferGeometry& other);

	
		BufferGeometry* clone();

		GeometryType GetGeometryType();

		friend class RawShaderMaterial;
		friend class LineMaterial;


	};


	class PlaneGeometry :public Geometry{

	private:


		void GeneratePlane(float width, float height, int widthSubdivision, int heightSubdivision);

	public:

		PlaneGeometry(); //default size 1 , subdivisions 1
		PlaneGeometry(float width, float height);
		PlaneGeometry(float width, float height,int widthSubdivision, int heightSubdivision);
		PlaneGeometry(const PlaneGeometry& other);

		PlaneGeometry& operator=(const PlaneGeometry& other);

		PlaneGeometry* clone();
	
		GeometryType GetGeometryType();

	};


	class BoxGeometry : public Geometry {

	private:

		float width, height, depth;
		float widthSubdivision, heightSubdivision, depthSubdivision;

	

		void createFace(MyVectorV3::Vector3f startPos, MyVectorV3::Vector3f xAxis,
			MyVectorV3::Vector3f yAxis);

		void topFace();
		void bottomFace();
		void leftFace();
		void rightFace();
		void frontFace();
		void backFace();


		void CreateBox();
		void ReservePositionAndIndices(int widthSubdivision, int heightSubdivision, int depthSubDivision);

	public:

		BoxGeometry(); //default size 1 , subdivisions 1
		BoxGeometry(float width, float height, float depth);
		BoxGeometry(float width, float height, float depth,
			int widthSubdivision, int heightSubdivision, int depthSubDivision);
		BoxGeometry(const BoxGeometry& other);

		BoxGeometry& operator=(const BoxGeometry& other);

		BoxGeometry* clone();

		GeometryType GetGeometryType();

	};

	class SphereGeometry : public Geometry {

	private:

		float radius;
		int widthSubdivision, heightSubdivision;


		void addIndices();
		void addVertex(MyVectorV3::Vector3f pos,MyVectorV3::Vector2f uv, MyVectorV3::Vector3f normal);
		void CreateSphere();

	public:

		SphereGeometry();
		SphereGeometry(float radius_);
		SphereGeometry(float radius_, int widthSubdivision_, int heightSubdivision_);
		SphereGeometry(const SphereGeometry& other);

		SphereGeometry& operator=(const SphereGeometry& other);

		SphereGeometry* clone();

		GeometryType GetGeometryType();

	};

	class LineGeometry: public Geometry {

	private:


	public:

		LineGeometry();
		LineGeometry(const LineGeometry& other);
		LineGeometry& operator=(const LineGeometry& other);

		void addPoint(MyVectorV3::Vector3f pos);
		void updatePoint(int index, MyVectorV3::Vector3f pos);
		void erasePoint(int index, int count = 1); //start erase index and how many points erased after that index
		MyVectorV3::Vector3f getPoint(int index);
		int getPointCount();

		GeometryType GetGeometryType();

	};

	//MATERIALS

	class Material {

	protected:

	
		string* vertexShaderText;
		string* fragmentShaderText;
		string* geometryShaderText; //optional

		MyOglShader::BaseShader shader;
		bool isCompiled_ = false;

		list<MaterialUniform::UniformVariant> uniforms;
		void eraseIfUniformAlready(string uniName);


		void CreateShader();
		void setUniformLocations();
		
	public:
		
	

		Material(){}
		Material(const Material& other);

		Material& operator=(const Material& other);

		template<typename t>
		void setUniform(string uniName, t value) {}

		bool isDebugShaderText = false;
		bool isDebugModeUniformsAddress = true; //default is true, but if you dont want to see cout
		//error messages when no uniform found in shader make it false

		template<>
		void setUniform<bool>(string uniName, bool value) {
			eraseIfUniformAlready(uniName);
			this->uniforms.insert(this->uniforms.end(), MaterialUniform::UniformVariant(MaterialUniform::BoolUniform(uniName, value)));
		}

		template<>
		void setUniform<int>(string uniName, int value) {
			eraseIfUniformAlready(uniName);
			this->uniforms.insert(this->uniforms.end(), MaterialUniform::UniformVariant(MaterialUniform::IntUniform(uniName, value)));
		}

		template<>
		void setUniform<float>(string uniName, float value) {
			eraseIfUniformAlready(uniName);
			this->uniforms.insert(this->uniforms.end(), MaterialUniform::UniformVariant(MaterialUniform::FloatUniform(uniName, value)));
		}

		template<>
		void setUniform<MyVectorV3::Vector2f>(string uniName, MyVectorV3::Vector2f value) {
			eraseIfUniformAlready(uniName);
			this->uniforms.insert(this->uniforms.end(), MaterialUniform::UniformVariant(MaterialUniform::Vec2fUniform(uniName, value)));
		}

		template<>
		void setUniform<MyVectorV3::Vector3f>(string uniName, MyVectorV3::Vector3f value) {
			eraseIfUniformAlready(uniName);
			this->uniforms.insert(this->uniforms.end(), MaterialUniform::UniformVariant(MaterialUniform::Vec3fUniform(uniName, value)));
		}

		template<>
		void setUniform<MyVectorV3::Vector4f>(string uniName, MyVectorV3::Vector4f value) {
			eraseIfUniformAlready(uniName);
			this->uniforms.insert(this->uniforms.end(), MaterialUniform::UniformVariant(MaterialUniform::Vec4fUniform(uniName, value)));
		}

		template<>
		void setUniform<MyMatrixV3::Matrix3f>(string uniName, MyMatrixV3::Matrix3f value) {
			eraseIfUniformAlready(uniName);
			this->uniforms.insert(this->uniforms.end(), MaterialUniform::UniformVariant(MaterialUniform::Mat3fUniform(uniName, value)));
		}

		template<>
		void setUniform<MyMatrixV3::Matrix4f>(string uniName, MyMatrixV3::Matrix4f value) {
			eraseIfUniformAlready(uniName);
			this->uniforms.insert(this->uniforms.end(), MaterialUniform::UniformVariant(MaterialUniform::Mat4fUniform(uniName, value)));
		}

		template<>
		void setUniform<MyTextureV2::Texture>(string uniName, MyTextureV2::Texture value) {
			eraseIfUniformAlready(uniName);
			//cout << value.getShaderValue() <<endl;
			//cout <<"shader value: "<< value.getShaderValue() << ", " << value.getTextureId() << endl;
			this->uniforms.insert(this->uniforms.end(), MaterialUniform::UniformVariant(MaterialUniform::TextureUniform(uniName, value)));
		}
		
		template<>
		void setUniform<MyTextureV2::CubeTexture>(string uniName, MyTextureV2::CubeTexture value) {
			eraseIfUniformAlready(uniName);
			//cout << value.getShaderValue() <<endl;
			//cout <<"cube value: "<< value.getTextureId() << endl;
			this->uniforms.insert(this->uniforms.end(), MaterialUniform::UniformVariant(MaterialUniform::CubeTextureUniform(uniName, value)));
		}

	


		//uniforms are optional

		void setWireframe(bool isWireframe);


		virtual void Compile(Geometry* geometry); 
		bool isCompiled() { return isCompiled_; }


		bool transparent = false;

		virtual void setModelMatrix(MyMatrixV3::Matrix4f modelMatrix);
		virtual void setViewProjectionMatrix(MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix)=0;

		void BindShader(); //for using

		

		friend class Mesh;
		friend class Points;
		friend class Line;
		friend class OglMouseEvents::MouseEventHandler;
		friend class OglRenderer::Renderer;

	};
	   

	class RawShaderMaterial : public Material {

	protected:

		int getLayoutNumber(string line);
		void setAttributeLayout(string line,int layout, Geometry* geometry);
		void updateBufferGeometryLayout(Geometry* geometry);

		static string LoadShaderFile(string fileLocation);

	public:


		static string LoadVertexShaderFromFile(string vertexShaderFileLocation);
		static string LoadFragmentShaderFromFile(string fragmentShaderFileLocation);

		RawShaderMaterial(){}
		RawShaderMaterial(string vertexShader, string fragmentShader);
		RawShaderMaterial(const RawShaderMaterial& other);

		void setVertexShader(string vertexShader);
		void setFragmentShader(string fragmentShader);
		void setGeometryShader(string geometryShader);


		
		RawShaderMaterial& operator=(const RawShaderMaterial& other);
	
		string* getVertexShader();
		string* getFragmentShader();
		string* getGeometryShader();


		void setViewProjectionMatrix(MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix);

		void Compile(Geometry* geometry); //virtual update from material parent class

		RawShaderMaterial* clone();
		
		friend class Mesh;
		friend class Points;
		friend class OglMouseEvents::MouseEventHandler;
	};
	
	
	class LineMaterial : public Material {

	private:


		int getLayoutNumber(string line);
		void setAttributeLayout(string line, int layout, Geometry* geometry);
		void updateBufferGeometryLayout(Geometry* geometry);


	public:

		LineMaterial();
		LineMaterial(const LineMaterial& cp);

		void Compile(Geometry* geometry); //virtual update from material parent class

		void setViewProjectionMatrix(MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix);
		void setColor(float r, float g, float b);
		void setColor(MyVectorV3::Vector3f color);


		LineMaterial& operator=(const LineMaterial& cp);
		LineMaterial* clone();

		friend class Mesh;
		friend class Line;

	};


	class MeshBasicMaterial: public Material {

	protected:

		

		//must have hooks
		MaterialShaderHooks::AttributeFromGeometryHook attributeHook;
		MaterialShaderHooks::CommonHook commonHook;
		MaterialShaderHooks::BeginVertexHook beginVertexHook;
		MaterialShaderHooks::BeginFragmentHook beginFragmentHook;
		MaterialShaderHooks::BeginFragmentResultHook beginFragmentResultHook;
		MaterialShaderHooks::BeginNormalFragmentHook beginNormalFragmentHook;
		MaterialShaderHooks::BeginNormalFragmentDoubleSideHook beginNormalFragmentDoubleSideHook;
		MaterialShaderHooks::EndFragmentGammaEnabledHook endFragmentGammaEnabledHook;
		MaterialShaderHooks::OutFragmentColorHook outFragmentColorHook;
		MaterialShaderHooks::EndFragmentResultHook endFragmentResultHook;
		
		//may or not hooks that depends on user use it or not like lights
		list<MaterialShaderHooks::Hook*> extraVertexHooks;
		list<MaterialShaderHooks::Hook*> extraFragmentHooks;


		//they are called inside compile method calls those four funcs
		virtual void replaceShaderHooks(Geometry* geometry);//shader icindeki hooklar� kald�r�p yerine
		//text file icindekini koyar
		void eraseRemainingHooks();


		MyTextureV2::Texture map;
		MyVectorV3::Vector4f color;

		bool isMap = false;
		bool isColor = false;


	public:

		MeshBasicMaterial();
		MeshBasicMaterial(const MeshBasicMaterial& other);

		MeshBasicMaterial& operator=(const MeshBasicMaterial& other);
	

		//diffuse Color, choose one of them if you use
		void setColor(float r, float g, float b, float w); //optional
		void setMap(MyTextureV2::Texture texture); //optional
		void setMapAndColor(MyTextureV2::Texture texture, MyVectorV3::Vector4f color, float ratio);
		//0  -> full texture, 1 -> full color, also optional


		MyTextureV2::Texture getMapTexture();
		MyVectorV3::Vector4f getColor();

		bool IsMap();
		bool IsColor();

		//if skelatal animation is used
		void enableSkinning(); //optional

		//assimp specific hooks
		void enableAssimpModelHooks();

		void onBeforeCompile(function<void(string* vs, string* fs)> func); //optional
		void Compile(Geometry* geometry); //must after every modification
	
		void setCameraLocalPos(MyVectorV3::Vector3f camLocalPos);
		void setViewProjectionMatrix(MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix);

		MeshBasicMaterial* clone();
		
		friend class Mesh;
		friend class Points;
		friend class OglMouseEvents::MouseEventHandler;	
	};



	class MeshStandardMaterial : public MeshBasicMaterial {

	private:

		MaterialShaderHooks::LightCommonHook lightCommonHook;
		MaterialShaderHooks::LightMainBeginHook lightMainBeginHook;
		MaterialShaderHooks::LightMainEndHook lightMainEndHook;
		MaterialShaderHooks::LightDirectionalHook lightDirectionalHook;
		MaterialShaderHooks::LightPointHook lightPointHook;	
		MaterialShaderHooks::LightSpotHook lightSpotHook;
		MaterialShaderHooks::LightMainDirectionalHook lightMainDirectionalHook;
		MaterialShaderHooks::LightMainPointHook lightMainPointHook;
		MaterialShaderHooks::LightMainSpotHook lightMainSpotHook;


		
		void setNormalMapSettings(Geometry*);

		//directional light shadow hooks
		bool isDirectionalLightCastShadow = false; //modified from friend class
		MaterialShaderHooks::ShadowMapVertexHook shadowMapVertexHook;
		MaterialShaderHooks::ShadowMapVertexBeginHook shadowMapVertexBeginHook;
		MaterialShaderHooks::ShadowMapFragmentHook shadowMapFragmentHook;
		MaterialShaderHooks::ShadowMapDirectionalLightHook shadowMapDirectionalLightHook;
		MaterialShaderHooks::ShadowMapSpotLightHook shadowMapSpotLightHook;
		MaterialShaderHooks::ShadowMapPointLightHook shadowMapPointLightHook;
		MaterialShaderHooks::DoubleSideDirectionalFragmentHook doubleSideDirectionalFragmentHook;
		MaterialShaderHooks::DoubleSidePointFragmentHook doubleSidePointFragmentHook;
		MaterialShaderHooks::DoubleSideSpotFragmentHook	 doubleSideSpotFragmentHook;
	

		void replaceShaderHooks(Geometry* geometry);


		MyTextureV2::Texture displacementMap;
		float displacementMapIntensity = 0.1f;
		bool isDisplacementMap = false;
		MyTextureV2::Texture normalMap;
		bool isNormalMap = false;

		int dirLightNumber = 0;
		int spotLightNumber = 0;
		int pointLightNumber = 0;

		bool isDirectionalLightEnabled = false;
		bool isPointLightEnabled = false;
		bool isSpotLightEnabled = false;
		bool isEnvironmentMapEnabled = false;

	public:


		//shadow
		struct {
			bool castShadow = false;
			bool receiveShadow = false;
		}shadow;

		struct {
			float reflection = 0;
			float refraction = 0;
			float refractionRatio = 1.0f / 1.52f;

		}envMapProperties;

		//side
		bool doubleSided = false; 

		//those are(also shadows) enabled by renderer if scene has dir,poi or spotlight, dont touch outside
		void enableDirectionalLight(int numberOfDirlight);
		void enablePointLight(int numberOfPointLight);
		void enableSpotLight(int numberOfSpotLight);
		void enableEnvironmentMap();

		bool getEnvironmentMapEnabled();

		void setNormalMap(MyTextureV2::Texture texture);
		MyTextureV2::Texture getNormalMap();
		bool IsNormalMap();

		void setDisplacementMap(MyTextureV2::Texture texture, float intensity);
		void setDisplacementMapIntensity( float intensity);
		MyTextureV2::Texture getDisplacementMap();
		float getDisplacementMapIntensity();
		bool IsDisplacementMap();


		void Compile(Geometry* geometry); //must after every modification

		MeshStandardMaterial();
		MeshStandardMaterial(const MeshStandardMaterial& other);

		MeshStandardMaterial& operator=(const MeshStandardMaterial& other);

		void setViewProjectionMatrix(MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix);

		MeshStandardMaterial* clone();

		friend class MeshStandardMaterialModifier;
		friend class Mesh;
		friend class Points;
		friend class OglMouseEvents::MouseEventHandler;
	};



	class MeshStandardMaterialModifier {

	public:

		static bool isNormalMap(MeshStandardMaterial*);
		static void setNormalMapSettings(MeshStandardMaterial*, Geometry*);

		static void enableDirectionalLightShadowShader(MeshStandardMaterial* standardMaterial);

	};

	//MESHES


	class Mesh {
	protected:

		unsigned int VAO;
		GLuint* m_Buffers;
		GLuint* m_BuffersShadow;
		MyOglWorldTransform::WorldTrans worldTransform;
		bool isLoaded_ = false;

		unsigned int ShadowMapVAO;

		
		MyMatrixV3::Matrix4f matrix;
		bool isAssimpMesh = false; //if true gl_cullface will be used
		
		//new shadow
		RawShaderMaterial shadowMapMaterial;
		RawShaderMaterial shadowMapMaterialPointLight;
		bool isShadowMapMaterialSet = false;
		void setShadowMapMaterial();
	

	public:

		std::map<string, string> personalData;

		bool isAssimpAnimatedMesh = false; //if true skinning will be used
		struct {

			list<function<void()>> mouseClickEventFunc;
			bool isMouseClickEventFuncSet = false;
			list<function<void()>> mouseEnterEventFunc;
			bool isMouseEnterEventFuncSet = false;
			list<function<void()>> mouseMoveEventFunc;
			bool isMouseMoveEventFuncSet = false;
			list<function<void()>> mouseLeaveEventFunc;
			bool isMouseLeaveEventFuncSet = false;


		} mouseEvents; //dont touch from outside use addEventListener func
		//actually i designed as private


		//parent class which is inherited
		Material* material;
		Geometry* geometry;

		MyVectorV3::Vector3f position=MyVectorV3::Vector3f(0,0,0);
		MyMatrixV3::Quaternion rotation;
		MyVectorV3::Vector3f scale = MyVectorV3::Vector3f(1,1,1);
	

		Mesh();		
		Mesh(Geometry& geo, Material& material);
		Mesh(Geometry* geo, Material* material);
		Mesh(const Mesh& other);


		Mesh& operator=(const Mesh& other); 
		
		unsigned int getVAO();

		MyOglWorldTransform::WorldTrans* getWorldTransform();

		void RotateOnAxis(float x, float y, float z, string order = "XYZ");
		void RotateOnAxis(float angle,const MyVectorV3::Vector3f& axis); //angle in degree
		void RotateOnAxis(const MyMatrixV3::Matrix4f& rotation); 
		void RotateOnAxis(const MyMatrixV3::Quaternion& q);
		void RotateOnWorldAxis(float x, float y, float z, string order = "XYZ");
		void RotateOnWorldAxis(float angle, const MyVectorV3::Vector3f& axis); //angle in degree
		void RotateOnWorldAxis(const MyMatrixV3::Matrix4f& rotation);
		void RotateOnWorldAxis(const MyMatrixV3::Quaternion& q);
		void ApplyMatrix(MyMatrixV3::Matrix4f matrix); //applies on model matrix
		MyMatrixV3::Matrix4f GetMatrix();
			
		virtual void LoadMesh();
		bool isLoaded();

		template<typename t>
		t* GetMaterial() {

			t* mat = dynamic_cast<t*>(this->material);
			return mat;
		}
		
		template<typename t>
		t* GetGeometry() {

			t* geometry_ = dynamic_cast<t*>(this->geometry);
			return geometry_;
		}

		void addEventListener(string eventName, function<void()> func);
		

		
		virtual void Render();


		 Mesh* clone();

		 friend class OglRenderer::Renderer;
		 friend class OglScene::Scene;
		 friend class OglMouseEvents::MouseEventHandler;
		

	};

	class Points : public Mesh {

	private:


	public:
		Points() :Mesh() {}
		Points(Geometry& geo, RawShaderMaterial& material) :Mesh(geo, material) {}
		Points(const Points& other) :Mesh(other) {}

		Points& operator=(const Points& other);

	
		void Render();


		Points* clone();
	};


	class Line : public Mesh {

	private:

		GLuint linePositionBuffer;
		float lineSize = 1.0f;

	public:

		Line(Geometry* geo, Material* mat);
		Line(Geometry& geo, Material& mat);
		Line(const Line& other);

		Line& operator=(const Line& other);

		void LoadMesh();
		void setSize(float size);

		void Render();


	};
}

#endif