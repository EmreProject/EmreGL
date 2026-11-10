#ifndef ogl_assimp_h
#define ogl_assimp_h



#include <GL/glew.h>
#include "oglMeshes.h"
#include "oglComputeShader.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <thread>
#include <chrono>



using namespace std::chrono;

namespace OglAssimp {

#define ASSIMP_LOAD_FLAGS ( aiProcess_Triangulate | aiProcess_GenSmoothNormals | \
                            aiProcess_MakeLeftHanded | aiProcess_JoinIdenticalVertices | \
							aiProcess_FindDegenerates | aiProcess_FindInvalidData | \
							aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes | \
							aiProcess_FixInfacingNormals | aiProcess_SortByPType /* if needed */ )




	class ModelLoader {


	private:

		struct MaterialData {
			GLuint diffuseTexture = 0;
			GLuint heightTexture = 0;
			GLuint displacementTexture = 0;
			GLuint normalTexture = 0;
			GLuint opacityTexture = 0;
			
			float diffuseTextureOpacity=1;

			//for alphamode: blend or opaque
			bool hasAlphaChannel=false;
			bool anyAlphaLT255 = false;   // at least one pixel has a < 255
		};

		struct LoadedTexture {
			string path;
			GLuint id;

			LoadedTexture(string path_, GLuint id_) :path(path_), id(id_) {}
		};

		static GLuint gFlatNormalTex;
		static void CreateFlatNormalTexture();

		struct ModelData;
		

		bool InitFromScene(const aiScene* pScene, ModelData& modelData,string fileLocation);
		void InitAllMaterials(const aiScene* pScene, ModelData& modelData, string fileLocation);
		void InitSingleMaterial(const aiScene* pScene, MaterialData& material,
			int index, string Dir);
		GLuint LoadMaterialTexture(const aiScene* pScene,const aiMaterial* pMaterial, aiTextureType type,
			const string& dir, bool gamma, bool* hasAlphaChannel = nullptr, bool* anyAlphaLT255 = nullptr);
		void InitAllMeshes(const aiScene* pScene, ModelData& modelData);
		void ProcessNode(aiNode* node, MyMatrixV3::Matrix4f& parent, ModelData& modelData, const aiScene* pScene);
		void InitSingleMesh(const aiScene* pScene,const aiMesh* paiMesh, OglObject3D::Mesh& mesh,
			ModelData& modelData);
		


		static string GetDirFromFileName(const string& fileName);
		static GLuint LoadEmbeddedTexture(const aiTexture* tex,bool gamma, bool* hasAlphaChannel=nullptr,bool* anyAlphaLT255 = nullptr);
		static GLuint LoadTextureFromFile(string texturePath,bool gamma, bool* hasAlphaChannel = nullptr, bool* anyAlphaLT255 = nullptr);
		



		Assimp::Importer Importer; //Assimp importer object
		list<ModelData> loadedModels; 
		list<LoadedTexture> loadedTextures; //list of loaded textures to avoid loading the same texture multiple times





	public:

		struct ModelData {

			list<OglObject3D::Mesh> meshes;
			list<MaterialData> materials;
			vector<MyMatrixV3::Matrix4f> meshTransformations; //list of transformation matrices for each mesh
			const aiScene* pScene = nullptr;


			MyVectorV3::Vector3f position= MyVectorV3::Vector3f(0,0,0);
			MyMatrixV3::Quaternion rotation;
			MyVectorV3::Vector3f scale= MyVectorV3::Vector3f(1,1,1);
			

			struct {


				list<function<void()>> mouseEnterEventFunc;	
				list<function<void()>> mouseLeaveEventFunc;
			

			} mouseEvents;
			
			void addEventListener(string event, function<void()> func);
			void onBeforeCompile(function<void(string* vs,string* fs)> func);


			template<typename t>
			void setUniform(string name, t value) {
				//set uniform for all meshes in the model
				for (OglObject3D::Mesh& mesh : meshes) {
					mesh.material->setUniform(name, value);
				}
			}
		};





		ModelLoader();
		~ModelLoader();


		//Load model from file
		bool Load(string fileLocation, function<void(ModelData&)> meshCallback);
		
	};



	class AnimationMixer {
	public:

		struct AnimationClip {
		public:

			struct NodeData {
				const aiNodeAnim* pNodeAnim = nullptr;
				const aiNode* pNode = nullptr;
				int boneIndex = -1;
				vector<MyMatrixV3::Matrix4f> GlobalTransform; //array of whole global
				//transforms in animation, get index using curent time / anim duration
				vector<MyMatrixV3::Matrix4f> BoneFinalTransformation; //array of whole global
				//bone final transforms in animation, get index using curent time / anim duration
				const NodeData* parentNode;

				list<OglObject3D::Mesh*> meshes;

			};

		private:

			enum class AnimationState {
				Playing,
				Paused,
				Stopped,
				NotStarted
			};



			OglComputeShader::SSBOBuffer ssboBoneTransforms;
			bool isSsboFirstTimeSet = true;
			AnimationMixer* mixer = nullptr; //pointer to the parent AnimationMixer
			const aiAnimation* aiAnim = nullptr; //pointer to the aiAnimation object
			int index = -1; //index of the animation in the model
			list<NodeData> nodes;
			double ticksPerSecond = 25.0; //ticks per second of the animation
			double durationMs = -1; //duration of the animation in ms
			double currentTimeMs = 0.0; //current time of the animation in ms, 0 to durationMs
			AnimationState state = AnimationState::NotStarted; //state of the animation
			const aiScene* pScene = nullptr; //pointer to the aiScene object for the model, used to get the node hierarchy and bone data

			double ConvertTicksToSeconds(double ticks);
			double ConvertSecondsToTicks(double seconds);
			double ConvertTicksToMilliseconds(double ticks);
			double ConvertMillisecondsToTicks(double milliseconds);

			void SetSsboAttributesOfEachMesh(); //set the ssbo attributes for the shader
			void Update(double deltaTime); //update the animation state, call this every frame

		public:

			bool isInfinite = false;
			float speed = 1.0f; //speed multiplier for the animation

			void Play();
			void Pause();
			void Stop(); //goes to time 0

			AnimationClip() :ssboBoneTransforms(1) {} //initialize ssbo with size 1


			friend class AnimationMixer;
		};


	private:
	

		struct Influence {

			int32_t boneIndex;
			float weight;

			Influence(int32_t boneIndex_, float weight_) :boneIndex(boneIndex_), weight(weight_) {}
		};

		struct BoneInfluenceData {
			vector<Influence> influences; 
		};

		struct MeshBoneData {
			vector<BoneInfluenceData> vertices; //ssbo array of bone data for each mesh vertices for the model,
		};

		struct Bone {
			MyMatrixV3::Matrix4f offsetMatrix; //offset matrix for the bone, used to transform vertices
			//from model space to bone space
			MyMatrixV3::Matrix4f finalTransform; //final transform for the bone, used to transform vertices
			//from model space to world space
		};

		vector<MyMatrixV3::Matrix4f> palette; //final bone transforms for the current animation frame

		ModelLoader::ModelData& modelData;
		MyMatrixV3::Matrix4f globalInverseTransform;
		map<string, int> boneNameToIndex; //map of bone names to indices
		vector<Bone> bones; //list of bones for the model
		vector<MeshBoneData> meshBonesInfluences; //list of mesh bone influences for the model

		
		int GetBoneId(const aiBone* pBone);

		void Load();
		void LoadAllMeshBones();
		void LoadSingleMeshBones(const aiMesh* paiMesh,OglObject3D::Mesh&, int meshIndex);
		void LoadAttributesAllMeshes();
		void LoadAttributesSingleMesh(OglObject3D::Mesh& mesh,MeshBoneData meshBoneData);
		void LoadBindingPose();
		void LoadSsboBoneMeshData();
		void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
		int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
		int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
		const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string& NodeName);
		void ReadNodeHierarchy(const aiAnimation* pAnimation,float AnimationTimeTicks, const aiNode* pNode, MyMatrixV3::Matrix4f& ParentTransform);
		vector<MyMatrixV3::Matrix4f> GetBoneTransforms(int animationIndex, float TimeInTicks);


		//my way of animation
		void CalcInterpolatedScaling(vector<MyMatrixV3::Matrix4f>& outMatrix,const aiNodeAnim* pNodeAnim,float deltaTimeTicks);
		void CalcInterpolatedRotation(vector<MyMatrixV3::Matrix4f>& outMatrix, const aiNodeAnim* pNodeAnim, float deltaTimeTicks);
		void CalcInterpolatedPosition(vector<MyMatrixV3::Matrix4f>& outMatrix, const aiNodeAnim* pNodeAnim, float deltaTimeTicks);
		void LoadNodeHierarchy(AnimationClip& clip, const AnimationClip::NodeData* a,const aiNode* pNode, float deltaTimeMs);
		void UpdateBoneFrameTransforms(int animationIndex, float AnimTimeMs);


		
		
		list<AnimationClip> animationClips; //list of animation clips for the model
		


	public:

		long long lastTime = -1;
		long long currentTime = -1;


	
		AnimationMixer(ModelLoader::ModelData& modelData_);


		
		AnimationClip* clipAction(int index); //get the animation clip by index
		

		void Update(double deltaTimeMs); //update the animation mixer, call this every frame

	};



	long long GetCurrentTimeAsMilliseconds();
	using AnimationClip = AnimationMixer::AnimationClip;
	using ModelData = ModelLoader::ModelData;


}

#endif // !ogl_assimp_h
