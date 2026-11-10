#include "oglAssimp.h"

namespace OglAssimp {

	void ModelLoader::ModelData::addEventListener(string eventName, function<void()> func) {


		if (eventName == "mouseenter") {

			this->mouseEvents.mouseEnterEventFunc.push_back(func);
			
		}
		else if (eventName == "mouseleave") {
			this->mouseEvents.mouseLeaveEventFunc.push_back(func);
		
		}
		for_each(this->meshes.begin(), this->meshes.end(), [&eventName, func,this](OglObject3D::Mesh& mesh) {

			std::transform(eventName.begin(), eventName.end(), eventName.begin(),
				[](unsigned char c) {
					return static_cast<char>(std::tolower(c));
				});

			if (eventName == "mouseenter") {

				mesh.mouseEvents.mouseEnterEventFunc.push_back([this] {
				
					
					for (function<void()> func : this->mouseEvents.mouseEnterEventFunc) {
						func();
					}
					
					});
				mesh.mouseEvents.isMouseEnterEventFuncSet = true;
			}
			else if (eventName == "mouseleave") {
				mesh.mouseEvents.mouseLeaveEventFunc.push_back([this]() {
				

							for (function<void()> func : this->mouseEvents.mouseLeaveEventFunc) {
								func();
							}
						

						
					});
				mesh.mouseEvents.isMouseLeaveEventFuncSet = true;
			}
			else if (eventName == "click") {
				mesh.mouseEvents.mouseClickEventFunc.push_back(func);
				mesh.mouseEvents.isMouseClickEventFuncSet = true;

			}
			else if (eventName == "mousemove") {
				mesh.mouseEvents.mouseMoveEventFunc.push_back(func);
				mesh.mouseEvents.isMouseMoveEventFuncSet = true;
			}
			});
	}

	void ModelLoader::ModelData::onBeforeCompile(function<void(string* vs,string* fs)> func) {

		for_each(this->meshes.begin(), this->meshes.end(), [func](OglObject3D::Mesh& mesh) {
			OglObject3D::MeshStandardMaterial* material = dynamic_cast<OglObject3D::MeshStandardMaterial*>(mesh.material);
			if (material != nullptr) {
				material->onBeforeCompile(func);
			}
			});
	}

	ModelLoader::ModelLoader() {

	}
	ModelLoader::~ModelLoader() {

	}


	GLuint ModelLoader::gFlatNormalTex = 0;

	void ModelLoader::CreateFlatNormalTexture() {

		if (gFlatNormalTex != 0) {
			return; //already created
		}


		const GLubyte pixel[3] = { 128, 128, 255 }; // (0.5, 0.5, 1.0)

		glGenTextures(1, &gFlatNormalTex);
		glBindTexture(GL_TEXTURE_2D, gFlatNormalTex);

		// Important: normal maps are **linear** data, do NOT use sRGB internal formats.
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB8,        // linear 8-bit RGB
			1, 1,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			pixel);

		// No filtering needed (1×1), but set sane defaults
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // or GL_LINEAR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // or GL_LINEAR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	string ModelLoader::GetDirFromFileName(const string& fileName) {

		string::size_type SlashIndex = fileName.find_last_of("/\\");
		string Dir;

		if (SlashIndex == string::npos) {
			Dir = "./"; //no slash found, so current directory
		}
		else if (SlashIndex == 0) {
			Dir = "/"; //slash at the beginning, so root directory
		}
		else {

			Dir = fileName.substr(0, SlashIndex + 1); //slash found, so return the directory part
		}

		return Dir;

	}

	GLuint ModelLoader::LoadEmbeddedTexture(const aiTexture* texture, bool gamma, bool* hasAlphaChannel, bool* anyAlphaLT255) {

		GLuint textureID;
		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);

		int width, height, nrComponents;
		unsigned char* imageData = nullptr;;

		if (texture->mHeight == 0) {
			// Compressed texture (JPEG/PNG in memory)
			imageData = stbi_load_from_memory(
				reinterpret_cast<unsigned char*>(texture->pcData),
				texture->mWidth,
				&width, &height, &nrComponents, 0);
		}
		else {
			// Raw pixel data
			imageData = stbi_load_from_memory(
				reinterpret_cast<unsigned char*>(texture->pcData),
				texture->mWidth * texture->mHeight,
				&width, &height, &nrComponents, 0);
		}

		if (!imageData) {
			std::cerr << "Failed to load embedded texture\n";
			return 0;
		}

		GLenum format = (nrComponents == 1) ? GL_RED :
			(nrComponents == 3) ? GL_RGB : GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0,
			(gamma && format == GL_RGB) ? GL_SRGB :
			(gamma && format == GL_RGBA) ? GL_SRGB_ALPHA : format,
			width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(imageData);
		return textureID;


	}
	GLuint ModelLoader::LoadTextureFromFile(string texturePath, bool gamma, bool* hasAlphaChannel, bool* anyAlphaLT255) {

		std::string filename = texturePath;

		int width, height, nrComponents;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (!data) {
			std::cerr << "Texture failed to load: " << filename << "\n";
			return 0;
		}

		if (hasAlphaChannel != nullptr && anyAlphaLT255 != nullptr) {


			if (nrComponents == 4) {
				*hasAlphaChannel = true;
				*anyAlphaLT255 = false;

				//scan any alpha lower than 255
				bool earlyStop = false;
				const int stride = width * 4;

				for (int y = 0; y < height; ++y) {

					const unsigned char* row = data + size_t(y) * size_t(stride);

					for (int x = 0; x < width; ++x) {

						unsigned char a = row[x * 4 + 3];

						if (a < 255) {
							*anyAlphaLT255 = true;
							earlyStop = true;
							break;
						}
					}

					if (earlyStop) {
						break;
					}

				}

			}
			else {
				*hasAlphaChannel = false;
				*anyAlphaLT255 = false;
			}
		}

		GLenum format = (nrComponents == 1) ? GL_RED : (nrComponents == 2) ? GL_RG :
			(nrComponents == 3) ? GL_RGB : GL_RGBA;

		GLuint textureID;
		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0,
			(gamma && format == GL_RGB) ? GL_SRGB :
			(gamma && format == GL_RGBA) ? GL_SRGB_ALPHA : format,
			width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLint swz[4];

		switch (nrComponents) {
		case 1: // R8 (grayscale)
			// Make rgba = (R, R, R, R). Now .a == R.
			swz[0] = GL_RED; swz[1] = GL_RED; swz[2] = GL_RED; swz[3] = GL_RED;
			glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swz);
			break;

		case 2: // RG8 (gray + alpha)
			// Make rgba = (R, R, R, G). Now .a == G (the alpha channel).
			swz[0] = GL_RED; swz[1] = GL_RED; swz[2] = GL_RED; swz[3] = GL_GREEN;
			glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swz);
			break;

		case 3: // RGB8 / SRGB8
			// Make rgba = (R, G, B, 1). .a == 1.0 (opaque).
			swz[0] = GL_RED; swz[1] = GL_GREEN; swz[2] = GL_BLUE; swz[3] = GL_ONE;
			glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swz);
			break;

		case 4: // RGBA8 / SRGB8_ALPHA8
			// Default mapping already correct; optional to set explicitly:
			swz[0] = GL_RED; swz[1] = GL_GREEN; swz[2] = GL_BLUE; swz[3] = GL_ALPHA;
			glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swz);
			break;
		}


		stbi_image_free(data);
		return textureID;
	}



	bool ModelLoader::Load(string fileLocation, function<void(ModelData&)> meshCallback) {

		this->loadedModels.insert(this->loadedModels.end(), ModelData());
		ModelData& modelData = this->loadedModels.back();
		modelData.pScene = Importer.ReadFile(fileLocation.c_str(), ASSIMP_LOAD_FLAGS);;

		if (modelData.pScene) {

			bool ret = this->InitFromScene(modelData.pScene, modelData, fileLocation);
			if (ret) {
				meshCallback(modelData);
			}
			return ret;
		}
		else {
			cout << "Error loading model: " << Importer.GetErrorString() << endl;
			return false;

		}
	}

	bool ModelLoader::InitFromScene(const aiScene* pScene, ModelLoader::ModelData& modelData, string fileLocation) {

		OglObject3D::BufferGeometry bufferGeometry;
		OglObject3D::MeshStandardMaterial meshStandardMaterial;


		//modelData.meshes.resize(pScene->mNumMeshes,
			//OglObject3D::Mesh(bufferGeometry,meshStandardMaterial));
		modelData.materials.resize(pScene->mNumMaterials);


		InitAllMaterials(pScene, modelData, fileLocation);
		InitAllMeshes(pScene, modelData);

		return true;
	}
	void ModelLoader::InitAllMaterials(const aiScene* pScene, ModelData& modelData, string fileLocation) {

		string Dir = ModelLoader::GetDirFromFileName(fileLocation);
		int index = 0;
		for_each(modelData.materials.begin(), modelData.materials.end(), [this, &index, Dir, pScene, &modelData, fileLocation](MaterialData& material) {
			InitSingleMaterial(pScene, material, index, Dir);
			index++;
			});

	}
	void ModelLoader::InitSingleMaterial(const aiScene* pScene, MaterialData& material,
		int index, string Dir) {


		const aiMaterial* pMaterial = pScene->mMaterials[index];

		float opacity = 1.0f;
		pMaterial->Get(AI_MATKEY_OPACITY, opacity); // 1 = opaque, 0 = fully transparent (if present)

		float transparency = 0.0f;
		// Some formats provide "transparency factor" instead of opacity (0=opaque, 1=fully transparent)
		if (pMaterial->Get(AI_MATKEY_TRANSPARENCYFACTOR, transparency) == AI_SUCCESS) {
			opacity *= (1.0f - transparency);
		}

		// Some formats put alpha in Kd (diffuse) color:
		aiColor4D kd(1, 1, 1, 1);
		if (pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, kd) == AI_SUCCESS) {
			opacity *= kd.a; // often 1; may be <1 for transparent
		}


		GLuint diffuseTexture = this->LoadMaterialTexture(pScene, pMaterial, aiTextureType_DIFFUSE, Dir, true,
			&material.hasAlphaChannel, &material.anyAlphaLT255);
		GLuint heightTexture = this->LoadMaterialTexture(pScene, pMaterial, aiTextureType_HEIGHT, Dir, false);
		GLuint displacementTexture = this->LoadMaterialTexture(pScene, pMaterial, aiTextureType_DISPLACEMENT, Dir, false); //same as height map if model contains both use this one
		GLuint normalTexture = this->LoadMaterialTexture(pScene, pMaterial, aiTextureType_NORMALS, Dir, false);
		GLuint opacityTexture = this->LoadMaterialTexture(pScene, pMaterial, aiTextureType_OPACITY, Dir, false);

		material.diffuseTexture = diffuseTexture;
		material.heightTexture = heightTexture;
		material.displacementTexture = displacementTexture;
		material.normalTexture = normalTexture;
		material.opacityTexture = opacityTexture;
		material.diffuseTextureOpacity = opacity;

	}

	GLuint ModelLoader::LoadMaterialTexture(const aiScene* pScene, const aiMaterial* pMaterial, aiTextureType type,
		const string& dir, bool gamma, bool* hasAlphaChannel, bool* anyAlphaLT255) {


		if (pMaterial->GetTextureCount(type) > 0) {


			aiString str;
			if (!pMaterial->GetTexture(type, 0, &str) == AI_SUCCESS) {
				return 0;
			};
			string p = str.C_Str();

			//check if texture was loaded before
			bool isLoaded = false;
			GLuint texID = 0;
			for_each(loadedTextures.begin(), loadedTextures.end(), [&p, &isLoaded, &texID](LoadedTexture& tex) {
				if (tex.path == p) {
					isLoaded = true;
					texID = tex.id;
				}
				});



			if (isLoaded) {

				return texID; //return previously loaded texture
			}
			else {

				string path = p;

				if (p[0] == '*') {


					int texIndex = std::atoi(p.c_str() + 1); //convert to int, if it is not a number, it will return 0

					const aiTexture* pTexture = pScene->mTextures[texIndex];

					texID = ModelLoader::LoadEmbeddedTexture(pTexture, gamma, hasAlphaChannel, anyAlphaLT255); //load embedded texture
					this->loadedTextures.emplace_back(path, texID); //add to loaded textures list


					return texID; //load embedded texture

				}
				else {


					if (p.substr(0, 2) == ".\\" || p.substr(0, 2) == "./") {
						p = p.substr(2, p.size() - 2);
					}
					string fileName = dir + p;

					texID = ModelLoader::LoadTextureFromFile(fileName, gamma, hasAlphaChannel, anyAlphaLT255); //load embedded texture
					this->loadedTextures.emplace_back(path, texID); //add to loaded textures list


					return texID;//load texture from file

				}


			}

		}
		else {
			return 0; //no texture found
		}
	}
	void ModelLoader::InitAllMeshes(const aiScene* pScene, ModelData& modelData) {


		MyMatrixV3::Matrix4f I;
		I.InitIdentity();
		ProcessNode(pScene->mRootNode, I, modelData, pScene);


	}
	void ModelLoader::ProcessNode(aiNode* node, MyMatrixV3::Matrix4f& parent, ModelData& modelData, const aiScene* pScene) {

		MyMatrixV3::Matrix4f nodeM(node->mTransformation);
		MyMatrixV3::Matrix4f global = parent * nodeM;

		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			OglObject3D::BufferGeometry bufferGeometry;
			OglObject3D::MeshStandardMaterial meshStandardMaterial;


			const aiMesh* paiMesh = pScene->mMeshes[node->mMeshes[i]];
			modelData.meshes.emplace_back(new OglObject3D::BufferGeometry(), new OglObject3D::MeshStandardMaterial());


			auto& mesh = modelData.meshes.back();

			ostringstream oss;
			oss << node->mMeshes[i];
			mesh.personalData["meshIndex"] = oss.str(); //store mesh index in personal data for later use in animation and skinning
			mesh.personalData["hasBones"] = (paiMesh->HasBones()) ? "true" : "false"; //store if mesh has bones for later use in animation and skinning

			bool mirrored = (global.Determinant3x3() < 0.0f); // implement Determinant3x3 on your Matrix4f
			if (mirrored) glFrontFace(GL_CW);
			else          glFrontFace(GL_CCW);


			
				//mesh.ApplyMatrix(global);
			

			InitSingleMesh(pScene, paiMesh, mesh, modelData);

		}
		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], global, modelData, pScene);
		}

	}

	void ModelLoader::InitSingleMesh(const aiScene* pScene, const aiMesh* paiMesh, OglObject3D::Mesh& mesh,
		ModelData& modelData) {


		OglObject3D::MeshStandardMaterial* meshMaterial = dynamic_cast<OglObject3D::MeshStandardMaterial*>(mesh.material);
		meshMaterial->enableAssimpModelHooks();
		meshMaterial->shadow.castShadow = true;
		meshMaterial->shadow.receiveShadow = true;

		auto it = modelData.materials.begin();
		for (int i = 0; i < paiMesh->mMaterialIndex; i++) {
			it++;
		}

		//meshMaterial->setUniform("uOpacity", it->diffuseTextureOpacity);

		if (it->hasAlphaChannel && it->anyAlphaLT255) {


			//meshMaterial->setUniform("uAlphaMode", 1); //blend
			meshMaterial->transparent = true;




		}
		else {
			//meshMaterial->setUniform("uAlphaMode", 0); //opaque
			meshMaterial->transparent = false;

		}

		if (it->diffuseTexture != 0) {
			MyTextureV2::Texture diffuseTex_(it->diffuseTexture);
			diffuseTex_.internalFormat = GL_SRGB_ALPHA;
			diffuseTex_.format = GL_SRGB_ALPHA;
			meshMaterial->setMap(diffuseTex_);
		}

		if (it->opacityTexture != 0) {

			//meshMaterial->setUniform("uOpacityMap", MyTextureV2::Texture(it->opacityTexture));
			//meshMaterial->setUniform("uIsOpacityMap", true);
		}
		else {
			//meshMaterial->setUniform("uIsOpacityMap", false);
		}

		if (it->normalTexture != 0) {
			meshMaterial->setNormalMap(MyTextureV2::Texture(it->normalTexture));
		}
		else {

			ModelLoader::CreateFlatNormalTexture();//place holder flat normal map for meshes without normal map
			meshMaterial->setNormalMap(MyTextureV2::Texture(ModelLoader::gFlatNormalTex));


			//cout << "No normal map found for this mesh" << endl;
		}


		if (it->displacementTexture != 0) {
			meshMaterial->setDisplacementMap(MyTextureV2::Texture(it->displacementTexture), 0.3);
		}
		if (it->heightTexture != 0 && it->displacementTexture == 0) {
			meshMaterial->setDisplacementMap(MyTextureV2::Texture(it->heightTexture), 0.3);
		}


		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);


		// Populate the vertex attribute vectors
		for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {

			const aiVector3D& pPos = paiMesh->mVertices[i];
			mesh.geometry->position.values.push_back(pPos.x);
			mesh.geometry->position.values.push_back(pPos.y);
			mesh.geometry->position.values.push_back(pPos.z);


			if (paiMesh->mNormals) {
				const aiVector3D& pNormal = paiMesh->mNormals[i];
				mesh.geometry->normal.values.push_back(pNormal.x);
				mesh.geometry->normal.values.push_back(pNormal.y);
				mesh.geometry->normal.values.push_back(pNormal.z);
			}
			else {
				aiVector3D Normal(0.0f, 1.0f, 0.0f);
				mesh.geometry->normal.values.push_back(Normal.x);
				mesh.geometry->normal.values.push_back(Normal.y);
				mesh.geometry->normal.values.push_back(Normal.z);
			}

			const aiVector3D& pTexCoord = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : Zero3D;
			mesh.geometry->texcoord.values.push_back(pTexCoord.x);
			mesh.geometry->texcoord.values.push_back(pTexCoord.y);
		}

		// Populate the index buffer
		for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
			const aiFace& Face = paiMesh->mFaces[i];

			mesh.geometry->indices.push_back(Face.mIndices[0]);
			mesh.geometry->indices.push_back(Face.mIndices[1]);
			mesh.geometry->indices.push_back(Face.mIndices[2]);
		}



	}



	//---------------------------------ANIMATION, SKINNING AND BONES----------------------------------------------------





	AnimationMixer::AnimationMixer(ModelLoader::ModelData& modelData_) :modelData(modelData_) {
		/*
		auto eraseIt = remove_if(modelData.meshes.begin(), modelData.meshes.end(), [](OglObject3D::Mesh& mesh) {
			return mesh.personalData["hasBones"] == "true";
			});
		modelData.meshes.erase(eraseIt, modelData.meshes.end()); //keep only meshes with bones for animation and skinning
		*/
		
		
		
		this->Load();
	}


	void AnimationMixer::Load() {


		if (modelData.pScene->HasAnimations()) {
			globalInverseTransform = MyMatrixV3::Matrix4f(modelData.pScene->mRootNode->mTransformation);
			globalInverseTransform.Inverse();
			this->meshBonesInfluences.resize(modelData.pScene->mNumMeshes);
			this->bones.reserve(200);

			this->LoadAllMeshBones();
			this->LoadAttributesAllMeshes();//ainfstart, ainfcount
			this->LoadSsboBoneMeshData();//ssbo with bone influences for each vertex of each mesh
			this->LoadBindingPose();

		}
		else {
			cout << "No animations found in the model." << endl;
		}

	}
	void AnimationMixer::LoadAllMeshBones() {

		
		for_each(modelData.meshes.begin(), modelData.meshes.end(), [this ](OglObject3D::Mesh& mesh) {
			int assimpMeshIndex = std::stoi(mesh.personalData["meshIndex"]);
			const aiMesh* paiMesh = modelData.pScene->mMeshes[assimpMeshIndex];


			
			if (paiMesh->HasBones()) {
				this->LoadSingleMeshBones(paiMesh, mesh, assimpMeshIndex);
			}
			//mesh.isAssimpAnimatedMesh = true;

		
			});


	}
	void AnimationMixer::LoadSingleMeshBones(const aiMesh* paiMesh, OglObject3D::Mesh& mesh, int meshIndex) {


		OglObject3D::MeshStandardMaterial* meshMaterial = dynamic_cast<OglObject3D::MeshStandardMaterial*>(mesh.material);
		meshMaterial->enableSkinning();
		mesh.isAssimpAnimatedMesh = true;

		MeshBoneData& meshBoneData = this->meshBonesInfluences[meshIndex];
		meshBoneData.vertices.resize(paiMesh->mNumVertices);

		for (int i = 0; i < paiMesh->mNumBones; i++) {


			const aiBone* pBone = paiMesh->mBones[i];
			int BoneId = this->GetBoneId(pBone);

			if (BoneId == bones.size()) {
				Bone newBone;
				newBone.offsetMatrix = MyMatrixV3::Matrix4f(pBone->mOffsetMatrix);

				bones.push_back(newBone);
			}

			for (int i = 0; i < pBone->mNumWeights; i++) {
				const aiVertexWeight& vw = pBone->mWeights[i];
				int vertexId = pBone->mWeights[i].mVertexId;

				meshBoneData.vertices[vertexId].influences.push_back(Influence(BoneId, vw.mWeight));
			}

		}

	}

	int AnimationMixer::GetBoneId(const aiBone* pBone) {

		int BoneIndex = 0;
		string BoneName(pBone->mName.C_Str());

		if (boneNameToIndex.find(BoneName) == boneNameToIndex.end()) {
			// Allocate an index for a new bone
			BoneIndex = (int)boneNameToIndex.size();
			boneNameToIndex[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = boneNameToIndex[BoneName];
		}

		return BoneIndex;

	}

	void AnimationMixer::LoadAttributesAllMeshes() {


		
		for_each(modelData.meshes.begin(), modelData.meshes.end(), [this](OglObject3D::Mesh& mesh) {
			int assimpMeshIndex = std::stoi(mesh.personalData["meshIndex"]);
			const aiMesh* paiMesh = modelData.pScene->mMeshes[assimpMeshIndex];

			if (paiMesh->HasBones()) {

				MeshBoneData& meshBoneData = this->meshBonesInfluences[assimpMeshIndex];
				LoadAttributesSingleMesh(mesh, meshBoneData);
			}
			
			
			});



	}

	void AnimationMixer::LoadAttributesSingleMesh(OglObject3D::Mesh& mesh, MeshBoneData meshBoneData) {

		vector<int> aInfStartArray;
		vector<int> aInfCountArray;

		int currentStart = 0;
		for_each(meshBoneData.vertices.begin(), meshBoneData.vertices.end(), [this, &mesh, &aInfStartArray, &aInfCountArray, &currentStart]
		(AnimationMixer::BoneInfluenceData& boneInfluenceData) {
				aInfStartArray.push_back(currentStart);
				aInfCountArray.push_back(boneInfluenceData.influences.size());
				currentStart += boneInfluenceData.influences.size();
			});

		mesh.geometry->setAttribute("aInfStart", OglObject3D::BufferAttribute(aInfStartArray, 1));
		mesh.geometry->setAttribute("aInfCount", OglObject3D::BufferAttribute(aInfCountArray, 1));
	
	

		
	}

	void AnimationMixer::LoadSsboBoneMeshData() {


	
		for_each(modelData.meshes.begin(), modelData.meshes.end(), [this](OglObject3D::Mesh& mesh) {

			int assimpMeshIndex = std::stoi(mesh.personalData["meshIndex"]);
			const aiMesh* paiMesh = modelData.pScene->mMeshes[assimpMeshIndex];

			if (paiMesh->HasBones()) {

				MeshBoneData& meshBoneData = this->meshBonesInfluences[assimpMeshIndex];
				vector<Influence> singleMeshAllInfluences;

				for_each(meshBoneData.vertices.begin(), meshBoneData.vertices.end(), [this, &singleMeshAllInfluences](BoneInfluenceData& boneInfluenceData) {

					singleMeshAllInfluences.insert(singleMeshAllInfluences.end(),
						boneInfluenceData.influences.begin(), boneInfluenceData.influences.end());
					});


				OglComputeShader::SSBOBuffer ssboMeshBoneData(0);
				ssboMeshBoneData.setSize(sizeof(Influence) * singleMeshAllInfluences.size());
				ssboMeshBoneData.Create();
				ssboMeshBoneData.FillData(singleMeshAllInfluences.data());

			

				OglObject3D::SSBOAttribute ssboAttribute;
				ssboAttribute.setSSBO(ssboMeshBoneData.getBufferId(), 0);
				mesh.geometry->addSSBOAttribute(ssboAttribute);

			}
			
			});
	}


	void AnimationMixer::LoadBindingPose() {


		//Without animation, just set the final transform to the global inverse transform * node transform
		MyMatrixV3::Matrix4f Identity;
		Identity.InitIdentity();
		this->ReadNodeHierarchy(nullptr, 0.0f, modelData.pScene->mRootNode, Identity);
	}


	void AnimationMixer::ReadNodeHierarchy(const aiAnimation* pAnimation, float AnimationTimeTicks, const aiNode* pNode, MyMatrixV3::Matrix4f& ParentTransform) {


		string NodeName(pNode->mName.data);

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
			MyMatrixV3::Matrix4f RotationM = MyMatrixV3::Matrix4f(RotationQ.GetMatrix());

			// Interpolate translation and generate translation transformation matrix
			aiVector3D Translation;
			CalcInterpolatedPosition(Translation, AnimationTimeTicks, pNodeAnim);
			MyMatrixV3::Matrix4f TranslationM;
			TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

			// Combine the above transformations
			NodeTransformation = TranslationM * RotationM * ScalingM;

		
		}
		else {
			
		}

		MyMatrixV3::Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

		if (this->boneNameToIndex.find(NodeName) != boneNameToIndex.end()) {
			int BoneIndex = boneNameToIndex[NodeName];

			bones[BoneIndex].finalTransform = this->globalInverseTransform * GlobalTransformation * bones[BoneIndex].offsetMatrix;
			
			
		}
		else {

			
		}

		
		for (int i = 0; i < pNode->mNumChildren; i++) {
			ReadNodeHierarchy(pAnimation, AnimationTimeTicks, pNode->mChildren[i], GlobalTransformation);
		}


	}

	int AnimationMixer::FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		for (int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
			float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;
			if (AnimationTimeTicks < t) {
				return i;
			}
		}

		return 0;
	}


	void AnimationMixer::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumPositionKeys == 1) {
			Out = pNodeAnim->mPositionKeys[0].mValue;
			return;
		}

		int PositionIndex = FindPosition(AnimationTimeTicks, pNodeAnim);
		int NextPositionIndex = PositionIndex + 1;
		//assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
		float t1 = (float)pNodeAnim->mPositionKeys[PositionIndex].mTime;
		float t2 = (float)pNodeAnim->mPositionKeys[NextPositionIndex].mTime;
		float DeltaTime = t2 - t1;
		float Factor = (AnimationTimeTicks - t1) / DeltaTime;
		//assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}


	int AnimationMixer::FindRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		//assert(pNodeAnim->mNumRotationKeys > 0);

		for (int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
			float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;
			if (AnimationTimeTicks < t) {
				return i;
			}
		}

		return 0;
	}


	void AnimationMixer::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumRotationKeys == 1) {
			Out = pNodeAnim->mRotationKeys[0].mValue;
			return;
		}

		int RotationIndex = FindRotation(AnimationTimeTicks, pNodeAnim);
		int NextRotationIndex = RotationIndex + 1;
		//assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
		float t1 = (float)pNodeAnim->mRotationKeys[RotationIndex].mTime;
		float t2 = (float)pNodeAnim->mRotationKeys[NextRotationIndex].mTime;
		float DeltaTime = t2 - t1;
		float Factor = (AnimationTimeTicks - t1) / DeltaTime;
		//assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
		aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		Out.Normalize();
	}


	int AnimationMixer::FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		//assert(pNodeAnim->mNumScalingKeys > 0);

		for (int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
			float t = (float)pNodeAnim->mScalingKeys[i + 1].mTime;
			if (AnimationTimeTicks < t) {
				return i;
			}
		}

		return 0;
	}


	void AnimationMixer::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumScalingKeys == 1) {
			Out = pNodeAnim->mScalingKeys[0].mValue;
			return;
		}

		int ScalingIndex = FindScaling(AnimationTimeTicks, pNodeAnim);
		int NextScalingIndex = ScalingIndex + 1;
		//assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		float t1 = (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime;
		float t2 = (float)pNodeAnim->mScalingKeys[NextScalingIndex].mTime;
		float DeltaTime = t2 - t1;
		float Factor = (AnimationTimeTicks - (float)t1) / DeltaTime;
		//assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}


	const aiNodeAnim* AnimationMixer::FindNodeAnim(const aiAnimation* pAnimation, const string& NodeName)
	{

		if (pAnimation == nullptr) return NULL;

		for (int i = 0; i < pAnimation->mNumChannels; i++) {
			const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

			if (string(pNodeAnim->mNodeName.data) == NodeName) {
				return pNodeAnim;
			}
		}

		return NULL;
	}


	vector<MyMatrixV3::Matrix4f> AnimationMixer::GetBoneTransforms(int animationIndex, float TimeInTicks) {

		vector<MyMatrixV3::Matrix4f> Transforms;


		MyMatrixV3::Matrix4f Identity;
		Identity.InitIdentity();


		float AnimationTimeTicks = fmod(TimeInTicks, (float)modelData.pScene->mAnimations[animationIndex]->mDuration);

		ReadNodeHierarchy(modelData.pScene->mAnimations[animationIndex], AnimationTimeTicks,
			modelData.pScene->mRootNode, Identity);
		Transforms.resize(this->bones.size());

		auto it = this->bones.begin();
		for (int i = 0; i < bones.size(); i++) {

			Transforms[i] = it->finalTransform.Transpose();
			it++;
		}


		return Transforms;
	}



	void AnimationMixer::LoadNodeHierarchy(AnimationClip& clip,
		const AnimationClip::NodeData* parentNode, const aiNode* pNode, float deltaTime) {

		clip.nodes.push_back(AnimationClip::NodeData());
		auto node = clip.nodes.end();
		node--;

		if (parentNode) {
			node->parentNode = parentNode;
		}

		double deltaTimeTicks = (deltaTime / 1000.0) * clip.ticksPerSecond;
		int totalFrame = (int)std::max(1.0, clip.aiAnim->mDuration / deltaTimeTicks);

		node->pNode = pNode;
		node->GlobalTransform.resize(totalFrame);
		node->BoneFinalTransformation.resize(totalFrame);


		string NodeName(pNode->mName.data);
		MyMatrixV3::Matrix4f NodeTransformation(pNode->mTransformation);

		const aiNodeAnim* pNodeAnim = FindNodeAnim(clip.aiAnim, NodeName);


		for (unsigned int i = 0; i < node->pNode->mNumMeshes; i++)
		{

			int index = node->pNode->mMeshes[i];


			auto it = modelData.meshes.begin();
			for (; it != modelData.meshes.end(); it++) {
				istringstream iss(it->personalData["meshIndex"]);
				int meshIndex;
				iss >> meshIndex;

				if (meshIndex == index) {
					node->meshes.push_back(&*it);
					break;
				}

			}

		}

		function<void(int)> SetBoneFinalTransform = [this, &node, NodeName](int frameIndex) {

			if (this->boneNameToIndex.find(NodeName) != boneNameToIndex.end()) {
				node->boneIndex = boneNameToIndex[NodeName];
				node->BoneFinalTransformation[frameIndex] = (this->globalInverseTransform * node->GlobalTransform[frameIndex] * bones[node->boneIndex].offsetMatrix).Transpose();

			}

			};

		if (pNodeAnim) {
			node->pNodeAnim = pNodeAnim;


			// Interpolate scaling and generate scaling transformation matrix
			vector<MyMatrixV3::Matrix4f> scaleMatrix(totalFrame + 1);
			CalcInterpolatedScaling(scaleMatrix, pNodeAnim, deltaTimeTicks);

			// Interpolate rotation and generate rotation transformation matrix
			vector<MyMatrixV3::Matrix4f> rotateMatrix(totalFrame + 1);
			CalcInterpolatedRotation(rotateMatrix, pNodeAnim, deltaTimeTicks);

			// Interpolate translation and generate translation transformation matrix
			vector<MyMatrixV3::Matrix4f> translateMatrix(totalFrame + 1);
			CalcInterpolatedPosition(translateMatrix, pNodeAnim, deltaTimeTicks);





			// Combine the above transformations
			if (parentNode) {

				for (int i = 0; i < node->GlobalTransform.size(); i++) {
					NodeTransformation = translateMatrix[i] * rotateMatrix[i] * scaleMatrix[i];
					node->GlobalTransform[i] = parentNode->GlobalTransform[i] * NodeTransformation;
					SetBoneFinalTransform(i);
				}
			}
			else {

				for (int i = 0; i < node->GlobalTransform.size(); i++) {
					node->GlobalTransform[i] = translateMatrix[i] * rotateMatrix[i] * scaleMatrix[i];
					SetBoneFinalTransform(i);
				}
			}



		}
		else {

			if (parentNode) {

				for (int i = 0; i < node->GlobalTransform.size(); i++) {
					node->GlobalTransform[i] = parentNode->GlobalTransform[i] * NodeTransformation;
					SetBoneFinalTransform(i);
				}
			}
			else {

				for (int i = 0; i < node->GlobalTransform.size(); i++) {
					node->GlobalTransform[i] = NodeTransformation;
					SetBoneFinalTransform(i);
				}
			}

		}


		for (int i = 0; i < pNode->mNumChildren; i++) {
			LoadNodeHierarchy(clip, &*node, pNode->mChildren[i], deltaTime);
		}



	}

	void AnimationMixer::CalcInterpolatedScaling(vector<MyMatrixV3::Matrix4f>& outMatrix, const aiNodeAnim* pNodeAnim, float deltaTimeTicks) {

		// we need at least two values to interpolate...
		if (pNodeAnim->mNumScalingKeys == 1) {

			for_each(outMatrix.begin(), outMatrix.end(),
				[pNodeAnim](MyMatrixV3::Matrix4f& mat) {

					aiVector3D Scaling = pNodeAnim->mScalingKeys[0].mValue;
					mat.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
				});

			return;
		}

		int frameAfterIndex = 1;
		for (int i = 0; i < outMatrix.size(); i++) {

			float frameTimeTicks = i * deltaTimeTicks;
			while (frameTimeTicks > (float)pNodeAnim->mScalingKeys[frameAfterIndex].mTime) {
				frameAfterIndex++;
				if (frameAfterIndex >= pNodeAnim->mNumScalingKeys) {
					frameAfterIndex = pNodeAnim->mNumScalingKeys - 1;
					break;
				}
			}

			int frameBeforeIndex = frameAfterIndex - 1;

			float t1 = (float)pNodeAnim->mScalingKeys[frameBeforeIndex].mTime;
			float t2 = (float)pNodeAnim->mScalingKeys[frameAfterIndex].mTime;
			float DeltaTime = t2 - t1;
			float Factor = (DeltaTime != 0.0f) ? ((frameTimeTicks - t1) / DeltaTime) : 0.0f;
			if (Factor < 0.0f) Factor = 0.0f;
			if (Factor > 1.0f) Factor = 1.0f;
			//assert(Factor >= 0.0f && Factor <= 1.0f);
			const aiVector3D& Start = pNodeAnim->mScalingKeys[frameBeforeIndex].mValue;
			const aiVector3D& End = pNodeAnim->mScalingKeys[frameAfterIndex].mValue;
			aiVector3D Delta = End - Start;
			aiVector3D Out = Start + Factor * Delta;

			outMatrix[i].InitScaleTransform(Out.x, Out.y, Out.z);
		}


	}

	void AnimationMixer::CalcInterpolatedRotation(vector<MyMatrixV3::Matrix4f>& outMatrix, const aiNodeAnim* pNodeAnim, float deltaTimeTicks) {

		// we need at least two values to interpolate...
		if (pNodeAnim->mNumRotationKeys == 1) {

			for_each(outMatrix.begin(), outMatrix.end(),
				[pNodeAnim](MyMatrixV3::Matrix4f& mat) {

					const aiQuaternion& RotationQ = pNodeAnim->mRotationKeys[0].mValue;
					mat = MyMatrixV3::Matrix4f(RotationQ.GetMatrix());
				});

			return;
		}

		int frameAfterIndex = 1;
		for (int i = 0; i < outMatrix.size(); i++) {

			float frameTimeTicks = i * deltaTimeTicks;
			while (frameTimeTicks > (float)pNodeAnim->mRotationKeys[frameAfterIndex].mTime) {
				frameAfterIndex++;
				if (frameAfterIndex >= pNodeAnim->mNumRotationKeys) {
					frameAfterIndex = pNodeAnim->mNumRotationKeys - 1;
					break;
				}
			}

			int frameBeforeIndex = frameAfterIndex - 1;

			float t1 = (float)pNodeAnim->mRotationKeys[frameBeforeIndex].mTime;
			float t2 = (float)pNodeAnim->mRotationKeys[frameAfterIndex].mTime;
			float DeltaTime = t2 - t1;
			float Factor = (DeltaTime != 0.0f) ? ((frameTimeTicks - t1) / DeltaTime) : 0.0f;
			if (Factor < 0.0f) Factor = 0.0f;
			if (Factor > 1.0f) Factor = 1.0f;
			//assert(Factor >= 0.0f && Factor <= 1.0f);
			const aiQuaternion& Start = pNodeAnim->mRotationKeys[frameBeforeIndex].mValue;
			const aiQuaternion& End = pNodeAnim->mRotationKeys[frameAfterIndex].mValue;
			aiQuaternion Out;
			aiQuaternion::Interpolate(Out, Start, End, Factor);
			Out.Normalize();
			outMatrix[i] = MyMatrixV3::Matrix4f(Out.GetMatrix());
		}



	}


	void AnimationMixer::CalcInterpolatedPosition(vector<MyMatrixV3::Matrix4f>& outMatrix, const aiNodeAnim* pNodeAnim, float deltaTimeTicks) {


		// we need at least two values to interpolate...
		if (pNodeAnim->mNumPositionKeys == 1) {

			for_each(outMatrix.begin(), outMatrix.end(),
				[pNodeAnim](MyMatrixV3::Matrix4f& mat) {

					aiVector3D Translation = pNodeAnim->mPositionKeys[0].mValue;
					mat.InitTranslationTransform(Translation.x, Translation.y, Translation.z);
				});

			return;
		}

		int frameAfterIndex = 1;
		for (int i = 0; i < outMatrix.size(); i++) {

			float frameTimeTicks = i * deltaTimeTicks;
			while (frameTimeTicks > (float)pNodeAnim->mPositionKeys[frameAfterIndex].mTime) {
				frameAfterIndex++;
				if (frameAfterIndex >= pNodeAnim->mNumPositionKeys) {
					frameAfterIndex = pNodeAnim->mNumPositionKeys - 1;
					break;
				}
			}

			int frameBeforeIndex = frameAfterIndex - 1;

			float t1 = (float)pNodeAnim->mPositionKeys[frameBeforeIndex].mTime;
			float t2 = (float)pNodeAnim->mPositionKeys[frameAfterIndex].mTime;
			float DeltaTime = t2 - t1;

			float Factor = (DeltaTime != 0.0f) ? ((frameTimeTicks - t1) / DeltaTime) : 0.0f;
			if (Factor < 0.0f) Factor = 0.0f;
			if (Factor > 1.0f) Factor = 1.0f;

			//assert(Factor >= 0.0f && Factor <= 1.0f);
			const aiVector3D& Start = pNodeAnim->mPositionKeys[frameBeforeIndex].mValue;
			const aiVector3D& End = pNodeAnim->mPositionKeys[frameAfterIndex].mValue;
			aiVector3D Delta = End - Start;
			aiVector3D Out = Start + Factor * Delta;

			outMatrix[i].InitTranslationTransform(Out.x, Out.y, Out.z);
		}

	}

	void AnimationMixer::UpdateBoneFrameTransforms(int animationIndex, float AnimTimeMs) {

		auto it = this->animationClips.begin();
		for (; it != this->animationClips.end(); it++) {
			if (it->index == animationIndex) {
				break;
			}
		}


		double frameRatio = ((double)AnimTimeMs / it->durationMs);
		//frameRatio = 0;

		for_each(it->nodes.begin(), it->nodes.end(), [this, &it, frameRatio](AnimationClip::NodeData& node) {

			if (node.boneIndex != -1) {

				int frameIndex = frameRatio * node.BoneFinalTransformation.size();
				if (frameIndex >= node.BoneFinalTransformation.size()) {
					frameIndex = node.BoneFinalTransformation.size() - 1;
				}

				bones[node.boneIndex].finalTransform = node.BoneFinalTransformation[frameIndex];

			}
			else {

				int frameIndex = frameRatio * node.GlobalTransform.size();
				if (frameIndex >= node.GlobalTransform.size()) {
					frameIndex = node.GlobalTransform.size() - 1;
				}


				for_each(node.meshes.begin(), node.meshes.end(),
					[this, &node, frameIndex](OglObject3D::Mesh* mesh)
					{
						if (mesh->personalData["hasBones"] == "true")
							return;

						MyMatrixV3::Matrix4f transform = this->globalInverseTransform * node.GlobalTransform[frameIndex];
						transform.Decompose(mesh->position, mesh->rotation, mesh->scale);
					});

			}


			});
	}


	void AnimationMixer::Update(double deltaTimeMs) {

		for_each(animationClips.begin(), animationClips.end(), [this, deltaTimeMs](AnimationClip& clip) {
			if (clip.state == AnimationClip::AnimationState::Playing) {

				if (deltaTimeMs != 0) {

					clip.mixer->UpdateBoneFrameTransforms(clip.index, clip.currentTimeMs);



					std::vector<MyMatrixV3::Matrix4f> palette;
					palette.reserve(bones.size());
					for (const auto& b : bones) {
						palette.push_back(b.finalTransform);  // already transposed in your pipeline
					}

					if (clip.isSsboFirstTimeSet) {
						clip.isSsboFirstTimeSet = false;
						clip.ssboBoneTransforms.setSize(sizeof(MyMatrixV3::Matrix4f) * palette.size());
						clip.ssboBoneTransforms.Create();
						clip.ssboBoneTransforms.FillData(palette.data());
					}
					else {

						clip.ssboBoneTransforms.FillData(palette.data());
					}

					glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


				}

				clip.currentTimeMs += (deltaTimeMs * clip.speed); //calculate current time
				if (clip.currentTimeMs >= clip.durationMs) {
					clip.currentTimeMs = 0.0; //reset current time to 0

					if (clip.isInfinite) {
						clip.state = AnimationClip::AnimationState::Playing; //keep playing
					}
					else {
						clip.state = AnimationClip::AnimationState::Stopped; //set state to stopped
					}
				}
			}
			else if (clip.state == AnimationClip::AnimationState::Paused) {
				//do nothing, just keep the current time
			}
			else if (clip.state == AnimationClip::AnimationState::Stopped) {
				clip.currentTimeMs = 0.0; //reset current time to 0
			}

			}
		);
	}

	AnimationClip* AnimationMixer::clipAction(int index) {


		this->animationClips.push_back(AnimationClip());
		list<AnimationClip>::iterator animation = animationClips.end();
		animation--;

		animation->mixer = this;
		animation->aiAnim = modelData.pScene->mAnimations[index];
		animation->index = index;
		animation->ticksPerSecond = animation->aiAnim->mTicksPerSecond > 0 ? animation->aiAnim->mTicksPerSecond : 25.0; //default to 25 if not set
		animation->durationMs = animation->ConvertTicksToMilliseconds(animation->aiAnim->mDuration);
		animation->currentTimeMs = 0.0; //current time is 0
		animation->state = AnimationClip::AnimationState::NotStarted;
		animation->pScene = modelData.pScene;

		this->LoadNodeHierarchy(*animation, nullptr, modelData.pScene->mRootNode, 16);


		return &(*animation);

	}

	//---------------------AnimationClip-------------------------------------------------------------



	double AnimationClip::ConvertTicksToSeconds(double ticks) {
		return ticks / ticksPerSecond;
	}
	double AnimationClip::ConvertSecondsToTicks(double seconds) {
		return seconds * ticksPerSecond;
	}
	double AnimationClip::ConvertTicksToMilliseconds(double ticks) {
		return ConvertTicksToSeconds(ticks) * 1000.0;
	}
	double AnimationClip::ConvertMillisecondsToTicks(double milliseconds) {
		return (milliseconds / 1000.0) * ticksPerSecond;
	}


	void AnimationClip::SetSsboAttributesOfEachMesh() {

		auto itMesh = this->mixer->modelData.meshes.begin();

		for (; itMesh != this->mixer->modelData.meshes.end(); itMesh++) {
			OglObject3D::Mesh& mesh = *itMesh;
			if (mesh.personalData["hasBones"] != "true")
				continue;

			OglObject3D::SSBOAttribute ssboAttribute;
			ssboAttribute.setSSBO(this->ssboBoneTransforms.getBufferId(), 1);
			mesh.geometry->addSSBOAttribute(ssboAttribute);
		}

	}

	void AnimationClip::Play() {


		this->SetSsboAttributesOfEachMesh();


		for_each(mixer->animationClips.begin(), mixer->animationClips.end(), [this](AnimationClip& clip) {
			if (&clip != this) {
				clip.state = AnimationState::Stopped; //stop all other animations
				clip.currentTimeMs = 0.0;
			}
			});




		if (this->state == AnimationState::NotStarted) {

			this->currentTimeMs = 0.0; //reset current time to 0
			this->state = AnimationState::Playing;
		}
		else if (this->state == AnimationState::Paused) {

			this->state = AnimationState::Playing;
		}
		else if (this->state == AnimationState::Stopped) {

			this->currentTimeMs = 0.0;
			this->state = AnimationState::Playing;
		}

	}
	void AnimationClip::Pause() {

		this->state = AnimationState::Paused;
	}
	void AnimationClip::Stop() {

		this->state = AnimationState::Stopped; //set state to stopped
		this->currentTimeMs = 0.0;

	}

	void AnimationClip::Update(double deltaTimeMs) {



		if (this->state == AnimationState::Playing) {

			if (deltaTimeMs != 0) {


				vector<MyMatrixV3::Matrix4f> Transforms =
					mixer->GetBoneTransforms(this->index, ConvertMillisecondsToTicks(this->currentTimeMs));


				if (this->isSsboFirstTimeSet) {
					isSsboFirstTimeSet = false;
					ssboBoneTransforms.setSize(sizeof(MyMatrixV3::Matrix4f) * Transforms.size());
					ssboBoneTransforms.Create();
					ssboBoneTransforms.FillData(Transforms.data());
				}
				else {


					ssboBoneTransforms.FillData(Transforms.data());
				}

				glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


			}

			this->currentTimeMs += (deltaTimeMs * speed); //calculate current time
			if (this->currentTimeMs >= this->durationMs) {
				this->currentTimeMs = 0.0; //reset current time to 0

				if (isInfinite) {
					this->state = AnimationState::Playing; //keep playing
				}
				else {
					this->state = AnimationState::Stopped; //set state to stopped
				}
			}
		}
		else if (this->state == AnimationState::Paused) {
			//do nothing, just keep the current time
		}
		else if (this->state == AnimationState::Stopped) {
			this->currentTimeMs = 0.0; //reset current time to 0
		}




	}




	long long GetCurrentTimeAsMilliseconds() { //like 1704848483
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count();
	}

}