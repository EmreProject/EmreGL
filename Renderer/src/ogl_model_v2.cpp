#include "ogl_model_v2.h"




namespace OglModelV2 {



	// MODEL-------------------------------------------------------------------
	Model::~Model() {

		this->Clear();
	}

	void Model::Clear() {

		if (m_Buffers[0] != 0) {
			glDeleteBuffers(ARRAY_SIZE_AS_ELEMENTS(m_Buffers), m_Buffers);
		}

		if (m_VAO != 0) {
			glDeleteVertexArrays(1, &m_VAO);
			m_VAO = 0;
		}
	}
	MyOglWorldTransform::WorldTrans* Model::getWorldTransform() {

		return &this->worldMatrix;

	}

	OglModelV2::Material Model::getMaterial() {

		for (unsigned int i = 0; i < m_Materials.size(); i++) {
			if (m_Materials[i].ambientColor != MyVectorV3::Vector3f(0.0f, 0.0f, 0.0f)) {
				return m_Materials[i];
			}
		}

		return m_Materials[0];

	}

	void Model::LoadMesh(string file) {

		this->Clear();
		this->m_GlobalInverseTransform.InitIdentity();

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glGenBuffers(ARRAY_SIZE_AS_ELEMENTS(m_Buffers), m_Buffers);

		pScene = Importer.ReadFile(file.c_str(), OGLDEV_ASSIMP_LOAD_FLAGS);

		if (pScene) {

			aiMatrix4x4& rootTransformation = pScene->mRootNode->mTransformation;
			m_GlobalInverseTransform = MyMatrixV3::Matrix4f(rootTransformation);
			m_GlobalInverseTransform = m_GlobalInverseTransform.Inverse();
			InitFromScene(pScene, file);

		}
		else {

			cout << "Error parsing " << file << " : Model::LoadMesh function";
			cout << "ERROR::ASSIMP::" << Importer.GetErrorString() << endl;
		}


		glBindVertexArray(0);

	}


	void Model::InitFromScene(const aiScene* pScene, string filename) {

		m_Meshes.resize(pScene->mNumMeshes);
		m_Materials.resize(pScene->mNumMaterials);

		unsigned int NumVertices = 0;
		unsigned int NumIndices = 0;

		CountVerticesAndIndices(pScene, NumVertices, NumIndices);

		ReserveSpace(NumVertices, NumIndices);

		InitAllMeshes(pScene);

		InitMaterials(pScene, filename);

		PopulateBuffers();

	}

	void Model::CountVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices) {


		for (unsigned int i = 0; i < m_Meshes.size(); i++) {
			m_Meshes[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
			m_Meshes[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
			m_Meshes[i].BaseVertex = NumVertices;
			m_Meshes[i].BaseIndex = NumIndices;

			NumVertices += pScene->mMeshes[i]->mNumVertices;
			NumIndices += m_Meshes[i].NumIndices;
		}
	}

	void Model::ReserveSpace(unsigned int NumVertices, unsigned int NumIndices)
	{
		m_Positions.reserve(NumVertices);
		m_Normals.reserve(NumVertices);
		m_TexCoords.reserve(NumVertices);
		m_Indices.reserve(NumIndices);
		m_Bones.resize(NumVertices);

	}

	void Model::InitAllMeshes(const aiScene* pScene) {

		for (unsigned int i = 0; i < m_Meshes.size(); i++) {
			const aiMesh* paiMesh = pScene->mMeshes[i];
			InitSingleMesh(i, paiMesh);
		}

	}

	void Model::InitSingleMesh(unsigned int MeshIndex, const aiMesh* paiMesh) {

		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {

			const aiVector3D& pPos = paiMesh->mVertices[i];
			m_Positions.push_back(MyVectorV3::Vector3f(pPos.x, pPos.y, pPos.z));

			if (paiMesh->mNormals) {
				const aiVector3D& pNormal = paiMesh->mNormals[i];
				m_Normals.push_back(MyVectorV3::Vector3f(pNormal.x, pNormal.y, pNormal.z));
			}
			else {
				aiVector3D Normal(0.0f, 1.0f, 0.0f);
				m_Normals.push_back(MyVectorV3::Vector3f(Normal.x, Normal.y, Normal.z));
			}

			const aiVector3D& pTexCoord = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : Zero3D;
			m_TexCoords.push_back(MyVectorV3::Vector2f(pTexCoord.x, pTexCoord.y));
		}

		LoadMeshBones(MeshIndex, paiMesh);

		// Populate the index buffer
		for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
			const aiFace& Face = paiMesh->mFaces[i];
			//        printf("num indices %d\n", Face.mNumIndices);
			//        assert(Face.mNumIndices == 3);
			m_Indices.push_back(Face.mIndices[0]);
			m_Indices.push_back(Face.mIndices[1]);
			m_Indices.push_back(Face.mIndices[2]);
		}

	}

	void Model::LoadMeshBones(unsigned int MeshIndex, const aiMesh* pMesh)
	{
		for (unsigned int i = 0; i < pMesh->mNumBones; i++) {
			LoadSingleBone(MeshIndex, pMesh->mBones[i]);
		}
	}


	void Model::LoadSingleBone(unsigned int MeshIndex, const aiBone* pBone) {

		int BoneId = GetBoneId(pBone);

		if (BoneId == m_BoneInfo.size()) {
			MyMatrixV3::Matrix4f mOffsetMat(pBone->mOffsetMatrix);
			BoneInfo bi(mOffsetMat);
			m_BoneInfo.push_back(bi);
		}

		for (unsigned int i = 0; i < pBone->mNumWeights; i++) {
			const aiVertexWeight& vw = pBone->mWeights[i];
			unsigned int GlobalVertexID = m_Meshes[MeshIndex].BaseVertex + pBone->mWeights[i].mVertexId;
			m_Bones[GlobalVertexID].AddBoneData(BoneId, vw.mWeight);
		}

	}

	int Model::GetBoneId(const aiBone* pBone) {

		int BoneIndex = 0;
		string BoneName(pBone->mName.C_Str());

		if (m_BoneNameToIndexMap.find(BoneName) == m_BoneNameToIndexMap.end()) {
			// Allocate an index for a new bone
			BoneIndex = (unsigned int)m_BoneNameToIndexMap.size();
			m_BoneNameToIndexMap[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = m_BoneNameToIndexMap[BoneName];
		}

		return BoneIndex;


	}


	void Model::InitMaterials(const aiScene* pScene, const string& Filename)
	{
		string Dir = GetDirectoryFromFile(Filename);


		printf("Num materials: %d\n", pScene->mNumMaterials);

		// Initialize the materials
		for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
			const aiMaterial* pMaterial = pScene->mMaterials[i];

			LoadTextures(Dir, pMaterial, i);

			LoadColors(pMaterial, i);
		}


	}

	string Model::GetDirectoryFromFile(string file) {

		string::size_type SlashIndex;

		SlashIndex = file.find_last_of("\\");

		if (SlashIndex == -1) {
			SlashIndex = file.find_last_of("/");
		}

		string Dir;

		if (SlashIndex == string::npos) {
			Dir = ".";
		}
		else {
			Dir = file.substr(0, SlashIndex);
		}

		return Dir;
	}

	void Model::LoadTextures(const string& Dir, const aiMaterial* pMaterial, int index)
	{
		LoadDiffuseTexture(Dir, pMaterial, index);
		LoadSpecularTexture(Dir, pMaterial, index);
	}


	void Model::LoadDiffuseTexture(string Dir, const aiMaterial* pMaterial, int index)
	{
		//m_Materials[index].pDiffuse = NULL;

		// / veya \\ biri haline getir ben / bu yapcam
		void(*convertSlashSame)(string&) = [](string& path) {
			string::size_type find = path.find("\\");
			while (find != string::npos) {
				path.replace(find, 1, "/");
				find = path.find("\\", find);
			}
		};

		convertSlashSame(Dir);

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				string p(Path.data);
				convertSlashSame(p);

				if (p.substr(0, 2) == ".\\" || p.substr(0, 2) == "./") {
					p = p.substr(2, p.size() - 2);
				}

				string FullPath = Dir + "/" + p;


				m_Materials[index].diffuseTexture.LoadTexture(FullPath.c_str());

				printf("Loaded diffuse texture '%s'\n", FullPath.c_str());

			}
		}
	}


	void Model::LoadSpecularTexture(string Dir, const aiMaterial* pMaterial, int index)
	{
		//m_Materials[index].pSpecularExponent = NULL;

		void(*convertSlashSame)(string&) = [](string& path) {
			string::size_type find = path.find("\\");
			while (find != string::npos) {
				path.replace(find, 1, "/");
				find = path.find("\\", find);
			}
		};

		convertSlashSame(Dir);

		if (pMaterial->GetTextureCount(aiTextureType_SHININESS) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_SHININESS, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				string p(Path.data);
				convertSlashSame(p);

				if (p.substr(0, 2) == ".\\" || p.substr(0, 2) == "./") {
					p = p.substr(2, p.size() - 2);
				}

				string FullPath = Dir + "/" + p;

				m_Materials[index].specularTexture.LoadTexture(FullPath.c_str());

				printf("Loaded specular texture '%s'\n", FullPath.c_str());

			}
		}
	}


	void Model::LoadColors(const aiMaterial* pMaterial, int index)
	{
		aiColor3D AmbientColor(0.0f, 0.0f, 0.0f);
		MyVectorV3::Vector3f AllOnes(1.0f, 1.0f, 1.0f);

		int ShadingModel = 0;
		if (pMaterial->Get(AI_MATKEY_SHADING_MODEL, ShadingModel) == AI_SUCCESS) {
			printf("Shading model %d\n", ShadingModel);
		}


		if (pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, AmbientColor) == AI_SUCCESS) {
			printf("Loaded ambient color [%f %f %f]\n", AmbientColor.r, AmbientColor.g, AmbientColor.b);
			m_Materials[index].ambientColor.r = AmbientColor.r;
			m_Materials[index].ambientColor.g = AmbientColor.g;
			m_Materials[index].ambientColor.b = AmbientColor.b;
		}
		else {
			m_Materials[index].ambientColor = AllOnes;
		}

		aiColor3D DiffuseColor(0.0f, 0.0f, 0.0f);

		if (pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor) == AI_SUCCESS) {
			printf("Loaded diffuse color [%f %f %f]\n", DiffuseColor.r, DiffuseColor.g, DiffuseColor.b);
			m_Materials[index].diffuseColor.r = DiffuseColor.r;
			m_Materials[index].diffuseColor.g = DiffuseColor.g;
			m_Materials[index].diffuseColor.b = DiffuseColor.b;
		}

		aiColor3D SpecularColor(0.0f, 0.0f, 0.0f);

		if (pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, SpecularColor) == AI_SUCCESS) {
			printf("Loaded specular color [%f %f %f]\n", SpecularColor.r, SpecularColor.g, SpecularColor.b);
			m_Materials[index].specularColor.r = SpecularColor.r;
			m_Materials[index].specularColor.g = SpecularColor.g;
			m_Materials[index].specularColor.b = SpecularColor.b;
		}
	}


	void Model::PopulateBuffers() {

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions[0]) * m_Positions.size(), &m_Positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(POSITION_LOCATION);
		glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0]) * m_TexCoords.size(), &m_TexCoords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(TEX_COORD_LOCATION);
		glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0]) * m_Normals.size(), &m_Normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(NORMAL_LOCATION);
		glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_Bones[0]) * m_Bones.size(), &m_Bones[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(BONE_ID_LOCATION);
		glVertexAttribIPointer(BONE_ID_LOCATION, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
		glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
		glVertexAttribPointer(BONE_WEIGHT_LOCATION, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData),
			(const GLvoid*)(MAX_NUM_BONES_PER_VERTEX * sizeof(unsigned int)));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);


	}

	void Model::Render() {

		glBindVertexArray(m_VAO);

		for (unsigned int i = 0; i < m_Meshes.size(); i++) {
			unsigned int MaterialIndex = m_Meshes[i].MaterialIndex;


			if (m_Materials[MaterialIndex].diffuseTexture.isLoaded()) {
				m_Materials[MaterialIndex].diffuseTexture.BindTexture(MyTextureV2::Texture::GetCurrentTextureUnitIndex());
			}

			if (m_Materials[MaterialIndex].specularTexture.isLoaded()) {
				m_Materials[MaterialIndex].specularTexture.BindTexture(MyTextureV2::Texture::GetCurrentTextureUnitIndex());
			}

			glDrawElementsBaseVertex(GL_TRIANGLES,
				m_Meshes[i].NumIndices,
				GL_UNSIGNED_INT,
				(void*)(sizeof(unsigned int) * m_Meshes[i].BaseIndex),
				m_Meshes[i].BaseVertex);
		}

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

	}



	unsigned int Model::FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
			float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;
			if (AnimationTimeTicks < t) {
				return i;
			}
		}

		return 0;
	}


	void Model::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumPositionKeys == 1) {
			Out = pNodeAnim->mPositionKeys[0].mValue;
			return;
		}

		unsigned int PositionIndex = FindPosition(AnimationTimeTicks, pNodeAnim);
		unsigned int NextPositionIndex = PositionIndex + 1;
		assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
		float t1 = (float)pNodeAnim->mPositionKeys[PositionIndex].mTime;
		float t2 = (float)pNodeAnim->mPositionKeys[NextPositionIndex].mTime;
		float DeltaTime = t2 - t1;
		float Factor = (AnimationTimeTicks - t1) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}


	unsigned int Model::FindRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumRotationKeys > 0);

		for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
			float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;
			if (AnimationTimeTicks < t) {
				return i;
			}
		}

		return 0;
	}


	void Model::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumRotationKeys == 1) {
			Out = pNodeAnim->mRotationKeys[0].mValue;
			return;
		}

		unsigned int RotationIndex = FindRotation(AnimationTimeTicks, pNodeAnim);
		unsigned int NextRotationIndex = RotationIndex + 1;
		assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
		float t1 = (float)pNodeAnim->mRotationKeys[RotationIndex].mTime;
		float t2 = (float)pNodeAnim->mRotationKeys[NextRotationIndex].mTime;
		float DeltaTime = t2 - t1;
		float Factor = (AnimationTimeTicks - t1) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
		aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		Out.Normalize();
	}


	unsigned int Model::FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumScalingKeys > 0);

		for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
			float t = (float)pNodeAnim->mScalingKeys[i + 1].mTime;
			if (AnimationTimeTicks < t) {
				return i;
			}
		}

		return 0;
	}


	void Model::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumScalingKeys == 1) {
			Out = pNodeAnim->mScalingKeys[0].mValue;
			return;
		}

		unsigned int ScalingIndex = FindScaling(AnimationTimeTicks, pNodeAnim);
		unsigned int NextScalingIndex = ScalingIndex + 1;
		assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		float t1 = (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime;
		float t2 = (float)pNodeAnim->mScalingKeys[NextScalingIndex].mTime;
		float DeltaTime = t2 - t1;
		float Factor = (AnimationTimeTicks - (float)t1) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}

	void Model::ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* pNode, MyMatrixV3::Matrix4f& ParentTransform)
	{
		string NodeName(pNode->mName.data);

		const aiAnimation* pAnimation = pScene->mAnimations[0];

		MyMatrixV3::Matrix4f NodeTransformation(pNode->mTransformation);

		const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

		if (pNodeAnim) {
			// Interpolate scaling and generate scaling transformation matrix
			aiVector3D Scaling;
			CalcInterpolatedScaling(Scaling, AnimationTimeTicks, pNodeAnim);
			MyMatrixV3::Matrix4f ScalingM;
			ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

			// Interpolate rotation and generate rotation transformation matrix
			aiQuaternion RotationQ;
			CalcInterpolatedRotation(RotationQ, AnimationTimeTicks, pNodeAnim);
			MyMatrixV3::Matrix4f RotationM(RotationQ.GetMatrix());
			


			// Interpolate translation and generate translation transformation matrix
			aiVector3D Translation;
			CalcInterpolatedPosition(Translation, AnimationTimeTicks, pNodeAnim);
			MyMatrixV3::Matrix4f TranslationM;
			TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

			// Combine the above transformations
			NodeTransformation = TranslationM * RotationM * ScalingM;
		}


		MyMatrixV3::Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

		if (m_BoneNameToIndexMap.find(NodeName) != m_BoneNameToIndexMap.end()) {
			unsigned int BoneIndex = m_BoneNameToIndexMap[NodeName];
			m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].OffsetMatrix;
		}

		for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
			ReadNodeHierarchy(AnimationTimeTicks, pNode->mChildren[i], GlobalTransformation);
		}

	}


	void Model::GetBoneTransforms(float TimeInSeconds, vector<MyMatrixV3::Matrix4f>& Transforms)
	{

		MyMatrixV3::Matrix4f Identity;
		Identity.InitIdentity();

	
			float TicksPerSecond = (float)(pScene->mAnimations[0]->mTicksPerSecond != 0 ? pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
			float TimeInTicks = TimeInSeconds * TicksPerSecond;
			float AnimationTimeTicks = fmod(TimeInTicks, (float)pScene->mAnimations[0]->mDuration);

			ReadNodeHierarchy(AnimationTimeTicks, pScene->mRootNode, Identity);
			Transforms.resize(m_BoneInfo.size());

			for (unsigned int i = 0; i < m_BoneInfo.size(); i++) {
				Transforms[i] = m_BoneInfo[i].FinalTransformation;
			}
		

	}


	const aiNodeAnim* Model::FindNodeAnim(const aiAnimation* pAnimation, const string& NodeName)
	{
		for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
			const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

			if (string(pNodeAnim->mNodeName.data) == NodeName) {
				return pNodeAnim;
			}
		}

		return NULL;
	}


}