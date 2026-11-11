#include "ogl_default_meshes.h"


namespace OglMeshes{

	/*MATERIAL*/
	void Material::Info() {

		cout << "---Material Information---" << endl;

	}

	
	/*
	void ColorMaterial::setToUniform(GLuint uniformAddres) {

		glUniform4fv(uniformAddres, 1, &this->color.x);

	}
	*/

	LightningMaterial::~LightningMaterial() {

		if (albedoMap != nullptr) {
			delete albedoMap;
		}

		if (normalMap != nullptr) {
			delete normalMap;
		}

	}

	void LightningMaterial::Info() {

		Material::Info();
		cout << "Lightning material: " << 2
			<< " number of textures" << endl;
		cout << "-----------------" << endl;

	}

	void LightningMaterial::setAlbedoMap(string file) {

		this->albedoMap = new MyTextureV2::Texture(file);
	}
	void LightningMaterial::setNormalMap(string file) {

		this->normalMap = new MyTextureV2::Texture(file);
	}

	void LightningMaterial::setDepthMap(string file) {

		this->depthMap = new MyTextureV2::Texture(file);
	}

	/*MESHES*/

	MeshesCommon_::MeshesCommon_() {

		this->resetWorldTransform();
		

	}

	void MeshesCommon_::setGeometry() {

		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

	}


	//IT IS FOR 2 SPOT AND POÝNT LÝGHTS JUST MODIFY IT FOR ANY NUMBERS!!!!
	void MeshesCommon_::setShader() {

		shader.reset(new MyOglShader::LightningShader(ENGINE_MATERIAL_SHADERS_DIR "/parallax.vs",
			ENGINE_MATERIAL_SHADERS_DIR "/parallax.fs"));

		shader->setWVPlocation("gWVP");
		shader->setMaterialLocation("gMaterial.AmbientColor", "gMaterial.DiffuseColor", "gMaterial.SpecularColor");
		shader->setSamplerlocation("gSampler");
		shader->setNormalMaplocation("gNormalMap");
		shader->setCameraLocalPosLoc("gCameraLocalPos");
		shader->setDepthMapLocation("gDepthMap");
		shader->setDepthMapScaleLocation("height_scale");

		//dir light
		MyOglShader::LightningShader::DirLightUniformName dirLight;
		dirLight.Color = "gDirectionalLight.Base.Color";
		dirLight.AmbientIntensity = "gDirectionalLight.Base.AmbientIntensity";
		dirLight.DiffuseIntensity = "gDirectionalLight.Base.DiffuseIntensity";
		dirLight.SpecularIntensity = "gDirectionalLight.Base.SpecularIntensity";
		dirLight.Direction = "gDirectionalLight.Direction";

		//2 point lights
		MyOglShader::LightningShader::PointLightUniformName pointLight1;
		pointLight1.Color = "gPointLights[0].Base.Color";
		pointLight1.AmbientIntensity = "gPointLights[0].Base.AmbientIntensity";
		pointLight1.DiffuseIntensity = "gPointLights[0].Base.DiffuseIntensity";
		pointLight1.SpecularIntensity = "gPointLights[0].Base.SpecularIntensity";
		pointLight1.LocalPosition = "gPointLights[0].LocalPos";
		pointLight1.Atten.Constant = "gPointLights[0].Atten.Constant";
		pointLight1.Atten.Linear = "gPointLights[0].Atten.Linear";
		pointLight1.Atten.Exp = "gPointLights[0].Atten.Exp";

		MyOglShader::LightningShader::PointLightUniformName pointLight2;
		pointLight2.Color = "gPointLights[1].Base.Color";
		pointLight2.AmbientIntensity = "gPointLights[1].Base.AmbientIntensity";
		pointLight2.DiffuseIntensity = "gPointLights[1].Base.DiffuseIntensity";
		pointLight2.SpecularIntensity = "gPointLights[1].Base.SpecularIntensity";
		pointLight2.LocalPosition = "gPointLights[1].LocalPos";
		pointLight2.Atten.Constant = "gPointLights[1].Atten.Constant";
		pointLight2.Atten.Linear = "gPointLights[1].Atten.Linear";
		pointLight2.Atten.Exp = "gPointLights[1].Atten.Exp";

		//2 spotlights
		MyOglShader::LightningShader::SpotLightLocationName spotLight1;
		spotLight1.Color = "gSpotLights[0].Base.Base.Color";
		spotLight1.AmbientIntensity = "gSpotLights[0].Base.Base.AmbientIntensity";
		spotLight1.DiffuseIntensity = "gSpotLights[0].Base.Base.DiffuseIntensity";
		spotLight1.SpecularIntensity = "gSpotLights[0].Base.Base.SpecularIntensity";
		spotLight1.LocalPosition = "gSpotLights[0].Base.LocalPos";
		spotLight1.Atten.Constant = "gSpotLights[0].Base.Atten.Constant";
		spotLight1.Atten.Linear = "gSpotLights[0].Base.Atten.Linear";
		spotLight1.Atten.Exp = "gSpotLights[0].Base.Atten.Exp";
		spotLight1.Direction = "gSpotLights[0].Direction";
		spotLight1.InnerCutoff = "gSpotLights[0].InnerCutoff";
		spotLight1.OuterCutoff = "gSpotLights[0].OuterCutoff";

		MyOglShader::LightningShader::SpotLightLocationName spotLight2;
		spotLight2.Color = "gSpotLights[1].Base.Base.Color";
		spotLight2.AmbientIntensity = "gSpotLights[1].Base.Base.AmbientIntensity";
		spotLight2.DiffuseIntensity = "gSpotLights[1].Base.Base.DiffuseIntensity";
		spotLight2.SpecularIntensity = "gSpotLights[1].Base.Base.SpecularIntensity";
		spotLight2.LocalPosition = "gSpotLights[1].Base.LocalPos";
		spotLight2.Atten.Constant = "gSpotLights[1].Base.Atten.Constant";
		spotLight2.Atten.Linear = "gSpotLights[1].Base.Atten.Linear";
		spotLight2.Atten.Exp = "gSpotLights[1].Base.Atten.Exp";
		spotLight2.Direction = "gSpotLights[1].Direction";
		spotLight2.InnerCutoff = "gSpotLights[1].InnerCutoff";
		spotLight2.OuterCutoff = "gSpotLights[1].OuterCutoff";


		shader->setDirectionalLightLocation(dirLight);
		shader->addPointLightLocation(pointLight1);
		shader->addPointLightLocation(pointLight2);
		shader->addSpotLightLocation(spotLight1);
		shader->addSpotLightLocation(spotLight2);
		shader->setMaterialSpecularPropertyLocation("materialSpecular.SpecularIntensity",
			"materialSpecular.SpecularPower");

		shader->setNumPointLightsLocation("gNumPointLights");
		shader->setNumSpotLightsLocation("gNumSpotLights");


	}
	


	LightningMaterial* MeshesCommon_::createLightningMaterial() {

		LightningMaterial* texMat = new LightningMaterial();
		this->material = texMat;



		return texMat;
		
	}

	void MeshesCommon_::bindTextures() {


		LightningMaterial* lightMat = dynamic_cast<LightningMaterial*>(material);
		if (lightMat != nullptr) {

			lightMat->albedoMap->BindTexture(MyTextureV2::Texture::GetCurrentTextureUnitIndex());
			lightMat->normalMap->BindTexture(MyTextureV2::Texture::GetCurrentTextureUnitIndex());

			if (lightMat->depthMap != nullptr) {

				lightMat->depthMap->BindTexture(MyTextureV2::Texture::GetCurrentTextureUnitIndex());
			}

		}

	}
	
	MyOglWorldTransform::WorldTrans* MeshesCommon_::getWorldTransform() {

		return &this->worldTransform;

	}

	void MeshesCommon_::resetWorldTransform() {


		this->worldTransform.SetPosition(0, 0, 0);
		this->worldTransform.SetRotation(90, 0, 0);
		this->worldTransform.SetScale(1);
	}

	void MeshesCommon_::CalculateTangentSpace(MeshesCommon_::Vertex* vertices,int verticesLength, unsigned int* indices,
		int indicesLength) {

		for (int i = 0; i < indicesLength; i += 3) {

			Vertex& v0 = vertices[indices[i]];
			Vertex& v1 = vertices[indices[i + 1]];
			Vertex& v2 = vertices[indices[i + 2]];


			MyVectorV3::Vector3f Edge1 = v1.m_pos - v0.m_pos;
			MyVectorV3::Vector3f Edge2 = v2.m_pos - v0.m_pos;

			float DeltaU1 = v1.m_tex.x - v0.m_tex.x;
			float DeltaV1 = v1.m_tex.y - v0.m_tex.y;
			float DeltaU2 = v2.m_tex.x - v0.m_tex.x;
			float DeltaV2 = v2.m_tex.y - v0.m_tex.y;


			float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

			MyVectorV3::Vector3f Tangent, Bitangent;

			Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
			Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
			Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

			Bitangent.x = f * (-DeltaU2 * Edge1.x + DeltaU1 * Edge2.x);
			Bitangent.y = f * (-DeltaU2 * Edge1.y + DeltaU1 * Edge2.y);
			Bitangent.z = f * (-DeltaU2 * Edge1.z + DeltaU1 * Edge2.z);

			v0.m_tangent += Tangent;
			v1.m_tangent += Tangent;
			v2.m_tangent += Tangent;
		}


		for (unsigned int i = 0; i < verticesLength; i++) {
			vertices[i].m_tangent.Normalize();
		}
	}

	void MeshesCommon_::BindShader() {


		this->shader->bindShader();

	}


	PlaneMesh::PlaneMesh(): MeshesCommon_() {

		//meshcommon tarafýndan shader ve default worldtransform set edilir
		//this->setGeometry();

	}

	

	void PlaneMesh::setGeometry() {

		float vertices[] = {
			// Positions          // Texture Coords   // Normals		//tangent space(calculated next step)
			-0.5f, 0.0f, -0.5f,   0.0f, 0.0f,        0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, // Bottom-left
			 0.5f, 0.0f, -0.5f,   1.0f, 0.0f,        0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, // Bottom-right
			 0.5f, 0.0f,  0.5f,   1.0f, 1.0f,        0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, // Top-right
			-0.5f, 0.0f,  0.5f,   0.0f, 1.0f,        0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f // Top-left
		};

		unsigned int indices[] = {
			0, 1, 2, // First triangle
			0, 2, 3  // Second triangle
		};



		Vertex* vertexArray = (Vertex*)(vertices);
		int indicesLength = sizeof(indices) / sizeof(unsigned int);
		int verticesLength = sizeof(vertices) / sizeof(Vertex);

		this->CalculateTangentSpace(vertexArray,verticesLength, indices, indicesLength);

		for (int i = 0; i < sizeof(vertices) / sizeof(float); i++) {


			if (i % 11 == 0 && i != 0) {
				cout << endl;
			}

			cout << vertices[i] << ", ";

			
		}
		cout << endl;
		MeshesCommon_::setGeometry();

		// Bind VAO
		glBindVertexArray(this->VAO);

		// Bind VBO and upload vertex data
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Bind EBO and upload index data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Position attribute (3 floats per vertex)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Texture coordinate attribute (2 floats per vertex)
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Normal attribute (3 floats per vertex)
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		// tangent attribute (3 floats per vertex)
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);


		// Unbind VAO
		glBindVertexArray(0);


	}
	
	

	void PlaneMesh::Draw() {

		//before draw, bind shader and set uniforms!!


		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}

}