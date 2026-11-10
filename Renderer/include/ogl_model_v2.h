#ifndef ogl_model_v2_h
#define ogl_model_v2_h


#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ogl_texture.h"
#include "ogl_World.h"
#include <map>
#include "assert.h"

using namespace std;



namespace OglModelV2 {

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

#define COLOR_TEXTURE_UNIT              GL_TEXTURE0
#define COLOR_TEXTURE_UNIT_INDEX        0
#define SPECULAR_EXPONENT_UNIT             GL_TEXTURE8
#define SPECULAR_EXPONENT_UNIT_INDEX       8
#define NORMAL_TEXTURE_UNIT              GL_TEXTURE1
#define NORMAL_TEXTURE_UNIT_INDEX        1
#define	DEPTH_TEXTURE_UNIT				GL_TEXTURE2
#define	DEPTH_TEXTURE_UNIT_INDEX		2

#define ARRAY_SIZE_AS_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#define OGLDEV_ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals |  aiProcess_JoinIdenticalVertices )


#define MAX_NUM_BONES_PER_VERTEX 4


	struct Material {

		MyVectorV3::Vector3f ambientColor = MyVectorV3::Vector3f(0,0,0);
		MyVectorV3::Vector3f diffuseColor = MyVectorV3::Vector3f(0,0,0);
		MyVectorV3::Vector3f specularColor = MyVectorV3::Vector3f(0,0,0);

		MyTextureV2::Texture diffuseTexture;
		MyTextureV2::Texture specularTexture;
	};



	//ANA B�L�M

	class Model {


	private:



		enum BUFFER_TYPE {
			INDEX_BUFFER = 0,
			POS_VB = 1,
			TEXCOORD_VB = 2,
			NORMAL_VB = 3,
			BONE_VB = 4,
			NUM_BUFFERS = 5
		};

		struct BasicMeshEntry {
			BasicMeshEntry()
			{
				NumIndices = 0;
				BaseVertex = 0;
				BaseIndex = 0;
				MaterialIndex = -1;
			}

			unsigned int NumIndices;
			unsigned int BaseVertex;
			unsigned int BaseIndex;
			unsigned int MaterialIndex;
		};


		struct VertexBoneData {

			unsigned int BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
			float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };

			VertexBoneData() {}
			void AddBoneData(unsigned int BoneID, float Weight)
			{
				for (unsigned int i = 0; i < ARRAY_SIZE_AS_ELEMENTS(BoneIDs); i++) {
					if (Weights[i] == 0.0) {
						BoneIDs[i] = BoneID;
						Weights[i] = Weight;
						//printf("Adding bone %d weight %f at index %i\n", BoneID, Weight, i);
						return;
					}
				}

				// should never get here - more bones than we have space for
				cout << "there is more bone than maximum bones that i specified" << endl;
				assert(0);
			}

		};


		struct BoneInfo
		{
			MyMatrixV3::Matrix4f OffsetMatrix;
			MyMatrixV3::Matrix4f FinalTransformation;

			BoneInfo(MyMatrixV3::Matrix4f& Offset)
			{
				OffsetMatrix = Offset;
				FinalTransformation.SetZero();
				
			}
		};



		Assimp::Importer Importer;
		const aiScene* pScene = NULL;
		vector<BasicMeshEntry> m_Meshes;
		vector<Material> m_Materials;


		vector<MyVectorV3::Vector3f> m_Positions; //vec3 test edildi her sey tamam
		vector<MyVectorV3::Vector3f> m_Normals;
		vector<MyVectorV3::Vector2f> m_TexCoords; //vec2 test edildi cok iyi
		vector<unsigned int> m_Indices;
		vector<VertexBoneData> m_Bones;

		map<string, unsigned int> m_BoneNameToIndexMap;
		vector<BoneInfo> m_BoneInfo;

		MyOglWorldTransform::WorldTrans worldMatrix;
		unsigned int m_VAO = 0;
		unsigned int m_Buffers[NUM_BUFFERS] = { 0 };
		void Clear();



		void InitFromScene(const aiScene* pScene, string filename);
		void CountVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices);
		void ReserveSpace(unsigned int NumVertices, unsigned int NumIndices);
		void InitAllMeshes(const aiScene* pScene);
		void InitSingleMesh(unsigned int MeshIndex, const aiMesh* paiMesh);
		void LoadMeshBones(unsigned int MeshIndex, const aiMesh* pMesh);
		void LoadSingleBone(unsigned int MeshIndex, const aiBone* pBone);
		int  GetBoneId(const aiBone* pBone);
		void InitMaterials(const aiScene* pScene, const string& Filename);
		string GetDirectoryFromFile(string file);
		void LoadTextures(const string& Dir, const aiMaterial* pMaterial, int index);
		void LoadDiffuseTexture(string Dir, const aiMaterial* pMaterial, int index);
		void LoadSpecularTexture(string Dir, const aiMaterial* pMaterial, int index);
		void LoadColors(const aiMaterial* pMaterial, int index);
		void PopulateBuffers();


		MyMatrixV3::Matrix4f m_GlobalInverseTransform; //mat4row, aimatrix4x4, ogldev matrix4f test edildi, s�per


	public:


		~Model(); //for clear method
		void LoadMesh(string file);
		MyOglWorldTransform::WorldTrans* getWorldTransform();//for light calculation of local dir pos etc
		Material getMaterial();
		unsigned int FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
		unsigned int FindRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
		unsigned int FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
		void ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* pNode, MyMatrixV3::Matrix4f& ParentTransform);
		void GetBoneTransforms(float TimeInSeconds, vector<MyMatrixV3::Matrix4f>& Transforms);
		const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string& NodeName);


		void Render();

	};


}





#endif
