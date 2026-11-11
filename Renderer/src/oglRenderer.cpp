#include "oglRenderer.h"
#include <random>

namespace OglRenderer {




	//---------------Renderer specific functions--------------------



	void Renderer::meshStandardMaterialSpecific(OglScene::Scene* scene, OglObject3D::Mesh* mesh,MyVectorV3::Vector3f camWorldPos) {


		OglObject3D::MeshStandardMaterial* standardMaterial = dynamic_cast<OglObject3D::MeshStandardMaterial*>(mesh->material);
	
		standardMaterial->setUniform("directionalLightNumber", (int)scene->directionalLights.size());
		standardMaterial->enablePointLight(int(scene->directionalLights.size()));


		auto itDir = scene->directionalLights.begin();
		int index = 0;
		ostringstream oss;
		for (; itDir != scene->directionalLights.end(); itDir++) {


			(*itDir)->CalcLocalDirection(*mesh->getWorldTransform());

			oss << index;
			function<string(string)> getUni = [&oss](string uni)->string {
				string::size_type find = uni.find("?");
				uni.replace(find, 1, oss.str());
				return uni;
				};

			string color = getUni("gDirectionalLights[?].Base.Color");
			string ambientIntensity= getUni("gDirectionalLights[?].Base.AmbientIntensity");
			string diffuseIntensity = getUni("gDirectionalLights[?].Base.DiffuseIntensity");
			string specularIntensity = getUni("gDirectionalLights[?].Base.SpecularIntensity");
			string direction = getUni("gDirectionalLights[?].Direction");
			string worldDirection = getUni("gDirectionalLights[?].WorldDirection");
			string isShadowEnabled = getUni("gDirectionalLights[?].Base.isShadowEnabled");

			standardMaterial->setUniform(color, (*itDir)->Color);
			standardMaterial->setUniform(ambientIntensity, (*itDir)->AmbientIntensity);
			standardMaterial->setUniform(diffuseIntensity, (*itDir)->DiffuseIntensity);
			standardMaterial->setUniform(specularIntensity, (*itDir)->SpecularIntensity);
			standardMaterial->setUniform(direction, (*itDir)->getLocalDirection());
			standardMaterial->setUniform(worldDirection, (*itDir)->WorldDirection.Normalize());
			standardMaterial->setUniform(isShadowEnabled, 0.0f);


			if ((*itDir)->shadow.castShadow && this->shadowEnabled) {


				if (standardMaterial->shadow.receiveShadow) {

					
					string lightProjectionViewMatrix = getUni("gDirectionalLights[?].Base.lightProjectionViewMatrix");
					MyMatrixV3::Matrix4f projViewMatrix = (*itDir)->shadowMap.GetLightProjectionViewMatrix();
					string shadowMap = getUni("gDirectionalLights[?].shadowMap");
					string biasFactor = getUni("gDirectionalLights[?].biasFactor");
					string minBias = getUni("gDirectionalLights[?].minBias");



					//OglObject3D::MeshStandardMaterialModifier::enableDirectionalLightShadowShader(standardMaterial);
					standardMaterial->setUniform(isShadowEnabled, 1.0f);
					standardMaterial->setUniform(lightProjectionViewMatrix, projViewMatrix);
					standardMaterial->setUniform(shadowMap, (*itDir)->shadowMap.GetShadowMapTexture());
					standardMaterial->setUniform(biasFactor, (*itDir)->shadowMap.biasFactor);
					standardMaterial->setUniform(minBias, (*itDir)->shadowMap.minBias);



				}

			}

			index++;
			oss.str("");
			oss.clear();
		}

		
			standardMaterial->setUniform("pointLightNumber", (int)scene->pointLights.size());
			standardMaterial->enablePointLight(int(scene->pointLights.size()));

			//update each point light according to current mesh local coordinate
			auto it = scene->pointLights.begin();
			index = 0;
			oss.str("");
			oss.clear();
			for (; it != scene->pointLights.end(); it++) {


				(*it)->CalcLocalPosition(*mesh->getWorldTransform());
				oss << index;
				function<string(string)> getUni = [&oss](string uni)->string {
					string::size_type find = uni.find("?");
					uni.replace(find, 1, oss.str());
					return uni;
					};



				string color = getUni("gPointLights[?].Base.Color");
				string ambient = getUni("gPointLights[?].Base.AmbientIntensity");
				string diffuse = getUni("gPointLights[?].Base.DiffuseIntensity");
				string specular = getUni("gPointLights[?].Base.SpecularIntensity");
				string localPos = getUni("gPointLights[?].LocalPos");
				string attenConstant = getUni("gPointLights[?].Atten.Constant");
				string attenLinear = getUni("gPointLights[?].Atten.Linear");
				string attenExp = getUni("gPointLights[?].Atten.Exp");


				string isShadowEnabled = getUni("gPointLights[?].Base.isShadowEnabled");
				string shadowMap = getUni("gPointLights[?].shadowMapCube");
				string worldPos = getUni("gPointLights[?].WorldPos");
				string cameraWorldPos = getUni("gPointLights[?].CameraWorldPos");
				string farPlane = getUni("gPointLights[?].farPlane");
				string bias = getUni("gPointLights[?].bias");
				string diskRadius = getUni("gPointLights[?].diskRadius");



				standardMaterial->setUniform(color, (*it)->Color);
				standardMaterial->setUniform(ambient, (*it)->AmbientIntensity);
				standardMaterial->setUniform(diffuse, (*it)->DiffuseIntensity);
				standardMaterial->setUniform(specular, (*it)->SpecularIntensity);
				standardMaterial->setUniform(localPos, (*it)->GetLocalPosition());
				standardMaterial->setUniform(attenConstant, (*it)->Attenuation.Constant);
				standardMaterial->setUniform(attenLinear, (*it)->Attenuation.Linear);
				standardMaterial->setUniform(attenExp, (*it)->Attenuation.Exp);
				standardMaterial->setUniform(isShadowEnabled, 0.0f);


				if (standardMaterial->shadow.receiveShadow && this->shadowEnabled && (*it)->shadow.castShadow) {
				

							
							standardMaterial->setUniform(shadowMap, (*it)->shadowMap.GetShadowMapTexture());
							standardMaterial->setUniform(worldPos, (*it)->WorldPosition);
							standardMaterial->setUniform(cameraWorldPos, camWorldPos);
							standardMaterial->setUniform(isShadowEnabled, 1.0f);
							standardMaterial->setUniform(farPlane, (*it)->shadowMap.perspectiveCamera.zFar);
							standardMaterial->setUniform(bias, (*it)->shadowMap.bias);
							standardMaterial->setUniform(diskRadius, (*it)->shadowMap.diskRadius);

				
				}

				index++;
				oss.clear();
				oss.str("");
			}

		

	
			standardMaterial->setUniform("spotLightNumber", (int)scene->spotLights.size());
			standardMaterial->enableSpotLight(int(scene->spotLights.size()));
			
			//update each spot light according to current mesh local coordinate
			auto it2 = scene->spotLights.begin();

			index = 0;
			oss.str("");
			oss.clear();
			for (; it2 != scene->spotLights.end(); it2++) {

				(*it2)->CalcLocalDirectionAndPosition(*mesh->getWorldTransform());
				oss << index;
				function<string(string)> getUni = [&oss](string uni)->string {
					string::size_type find = uni.find("?");
					uni.replace(find, 1, oss.str());
					return uni;
					};


				string color = getUni("gSpotLights[?].Base.Color");
				string ambient = getUni("gSpotLights[?].Base.AmbientIntensity");
				string diffuse = getUni("gSpotLights[?].Base.DiffuseIntensity");
				string specular = getUni("gSpotLights[?].Base.SpecularIntensity");
				string localPos = getUni("gSpotLights[?].LocalPos");
				string attenConstant = getUni("gSpotLights[?].Atten.Constant");
				string attenLinear = getUni("gSpotLights[?].Atten.Linear");
				string attenExp = getUni("gSpotLights[?].Atten.Exp");
				string direction = getUni("gSpotLights[?].Direction");
				string worldDirection = getUni("gSpotLights[?].WorldDirection");
				string worldPosition = getUni("gSpotLights[?].WorldPosition");
				string innercutoff = getUni("gSpotLights[?].InnerCutoff");
				string outercutoff = getUni("gSpotLights[?].OuterCutoff");

				string isShadowEnabled = getUni("gSpotLights[?].Base.isShadowEnabled");
				string lightProjectionViewMatrix = getUni("gSpotLights[?].Base.lightProjectionViewMatrix");
				string shadowMap = getUni("gSpotLights[?].shadowMap");
				string biasFactor = getUni("gSpotLights[?].biasFactor");
				string minBias = getUni("gSpotLights[?].minBias");

			
				function<float(float degree)> toCos = [](float degree) {
					float radian = degree * 3.14159265f / 180.0f;
					return cosf(radian);
					};

				standardMaterial->setUniform(color, (*it2)->Color);
				standardMaterial->setUniform(ambient, (*it2)->AmbientIntensity);
				standardMaterial->setUniform(diffuse, (*it2)->DiffuseIntensity);
				standardMaterial->setUniform(specular, (*it2)->SpecularIntensity);
				standardMaterial->setUniform(localPos, (*it2)->GetLocalPosition());
				standardMaterial->setUniform(attenConstant, (*it2)->Attenuation.Constant);
				standardMaterial->setUniform(attenLinear, (*it2)->Attenuation.Linear);
				standardMaterial->setUniform(attenExp, (*it2)->Attenuation.Exp);
				standardMaterial->setUniform(direction, (*it2)->GetLocalDirection());
				standardMaterial->setUniform(worldDirection, (*it2)->WorldDirection.Normalize());
				standardMaterial->setUniform(worldPosition, (*it2)->WorldPosition);
				standardMaterial->setUniform(innercutoff, toCos((*it2)->InnerCutoff));
				standardMaterial->setUniform(outercutoff, toCos((*it2)->OuterCutoff));
				standardMaterial->setUniform(isShadowEnabled, 0.0f);


				if (standardMaterial->shadow.receiveShadow && this->shadowEnabled && (*it2)->shadow.castShadow) {

					
					MyMatrixV3::Matrix4f projectionViewMatrix = (*it2)->shadowMap.GetLightProjectionViewMatrix();

							standardMaterial->setUniform(isShadowEnabled, 1.0f);
							standardMaterial->setUniform(lightProjectionViewMatrix, projectionViewMatrix);
							standardMaterial->setUniform(shadowMap, (*it2)->shadowMap.GetShadowMapTexture());
							standardMaterial->setUniform(biasFactor, (*it2)->shadowMap.biasFactor);
							standardMaterial->setUniform(minBias, (*it2)->shadowMap.minBias);

					
				
				}
				


				index++;
				oss.clear();
				oss.str("");
			}

		




	}


	//-----------RENDERER PART----------------------------------


	Renderer::Renderer() {

		this->currentRenderTarget = GLuint(0); //default framebuffer
		this->ResetDebugLog();
		

	}

	Renderer::~Renderer() {
	
		
	
	}


	void Renderer::ResetDebugLog() {


		ofstream off;
		off.open(ENGINE_MATERIAL_SHADERS_DIR "/Debug/debugShaders.txt");
		off << "---"<<endl;
		off.close();

	}


	void Renderer::BindCurrentRenderTarget() {

		std::visit([](auto& arg) {
			
			using T = std::decay_t<decltype(arg)>;
			
			if constexpr (is_same_v<T, GLuint>) {
				glBindFramebuffer(GL_FRAMEBUFFER, arg);
				
			}
			else if constexpr (is_same_v<T, OglRenderTarget::RenderTarget*>) {
				glBindFramebuffer(GL_FRAMEBUFFER, arg->getFBO());
				//cout << arg->getFBO() << endl;
			}
			}, this->currentRenderTarget);

	}


	void Renderer::enableIfDirLightShadowMaps(OglScene::Scene& scene) {


		if (this->shadowEnabled) {
			
			
			auto it = scene.directionalLights.begin();
			for (; it != scene.directionalLights.end(); it++) {

				if ((*it)->shadow.castShadow && !(*it)->shadowMap.isLoaded()) {

					(*it)->shadowMap.Load();
					

				}
			}
		}
	}

	void Renderer::enableIfSpotLightShadowMaps(OglScene::Scene& scene) {

		if (this->shadowEnabled) {


			auto it = scene.spotLights.begin();
			for (; it != scene.spotLights.end(); it++) {

				if ((*it)->shadow.castShadow && !(*it)->shadowMap.isLoaded()) {

					(*it)->shadowMap.Load();


				}
			}
		}

	}

	void Renderer::enableIfPointLightShadowMaps(OglScene::Scene& scene) {


		if (this->shadowEnabled) {


			auto it = scene.pointLights.begin();
			for (; it != scene.pointLights.end(); it++) {

				if ((*it)->shadow.castShadow && !(*it)->shadowMap.isLoaded()) {

					(*it)->shadowMap.Load();


				}
			}
		}


	}

	void Renderer::clearShadowMaps(OglScene::Scene& scene) {

		for_each(scene.directionalLights.begin(), scene.directionalLights.end(),
			[](MyOglLightV2::DirectionalLight* dirLight) {

				dirLight->shadowMap.ClearFBO();

			});

		

		for_each(scene.pointLights.begin(), scene.pointLights.end(),
			[](MyOglLightV2::PointLight* pointLight) {

				pointLight->shadowMap.ClearFBO();

			});

		for_each(scene.spotLights.begin(), scene.spotLights.end(),
			[](MyOglLightV2::SpotLight* spotLight) {

				spotLight->shadowMap.ClearFBO();

			});

	
	}

	void Renderer::ApplyWorldMatrixAssimp(OglScene::Scene& scene) {

		for_each(scene.models.begin(), scene.models.end(), [](OglAssimp::ModelData* model) {

			auto it = model->meshes.begin();

			MyMatrixV3::Matrix4f translate;
			translate.InitTranslationTransform(model->position);

			MyMatrixV3::Matrix4f scale;
			scale.InitScaleTransform(model->scale);
			

			MyMatrixV3::Matrix4f rotation = model->rotation.ToMatrix4();

			MyMatrixV3::Matrix4f world = translate * rotation * scale;

			for (; it != model->meshes.end(); it++) {

				it->ApplyMatrix(world);

			}

			});

	}
	void Renderer::ApplyReverseWorldMatrixAssimp(OglScene::Scene& scene) {

		for_each(scene.models.begin(), scene.models.end(), [](OglAssimp::ModelData* model) {

			auto it = model->meshes.begin();

			MyMatrixV3::Matrix4f translate;
			translate.InitTranslationTransform(model->position);

			MyMatrixV3::Matrix4f scale;
			scale.InitScaleTransform(model->scale);

			MyMatrixV3::Matrix4f rotation = model->rotation.ToMatrix4();

			MyMatrixV3::Matrix4f worldInverse = translate * rotation * scale;
			worldInverse = worldInverse.Inverse();

			for (; it != model->meshes.end(); it++) {

				it->ApplyMatrix(worldInverse);


			}

			});

	}

	void Renderer::RenderPointLightShadowMaps(OglScene::Scene& scene) {

		list<OglObject3D::Mesh*> meshes = scene.GetMeshes();



		list<OglObject3D::Mesh*> meshesCastingShadow;
		if (this->shadowEnabled) {


			auto it = meshes.begin();
			for (; it != meshes.end(); it++) {

				OglObject3D::Mesh* mesh = *it;
				OglObject3D::MeshStandardMaterial* standardMaterial =
					dynamic_cast<OglObject3D::MeshStandardMaterial*>(mesh->material);

				if (standardMaterial != nullptr) {

					if (standardMaterial->shadow.castShadow) {
						meshesCastingShadow.insert(meshesCastingShadow.end(), mesh);
					}
				}

			}

			auto itPointLight = scene.pointLights.begin();
			for (; itPointLight != scene.pointLights.end(); itPointLight++) {

				MyOglLightV2::PointLight* pointLight = *itPointLight;
				if (pointLight->shadow.castShadow) {

					pointLight->shadowMap.BindFBO();
					pointLight->shadowMap.CreateViewMatrices(pointLight->WorldPosition);
					pointLight->shadowMap.CreatePerspectiveMatrix();

					vector<MyMatrixV3::Matrix4f> shadowMatrices = pointLight->shadowMap.GetLightProjectionViewMatrices();
					
					list<OglObject3D::Mesh*>::iterator it = meshesCastingShadow.begin();
					for (; it != meshesCastingShadow.end(); it++) {

						OglObject3D::Mesh& mesh_ = **it;

						if (!mesh_.isShadowMapMaterialSet) {

							mesh_.isShadowMapMaterialSet = true;
							mesh_.setShadowMapMaterial();


						}

						mesh_.shadowMapMaterialPointLight.BindShader();
						mesh_.shadowMapMaterialPointLight.uniforms = mesh_.material->uniforms;

						
						MyMatrixV3::Matrix4f identity;
						identity.InitIdentity();
						mesh_.shadowMapMaterialPointLight.setUniform("viewMatrix", identity);
						mesh_.shadowMapMaterialPointLight.setUniform("viewProjectionMatrix", identity);
						mesh_.shadowMapMaterialPointLight.setUniform("projectionMatrix", identity);

						mesh_.shadowMapMaterialPointLight.setUniform("lightPos", pointLight->WorldPosition);
						mesh_.shadowMapMaterialPointLight.setUniform("far_plane", pointLight->shadowMap.perspectiveCamera.zFar);


					
						ostringstream oss;
						for(int i=0;i<6;i++){
							
							oss << "shadowMatrices[" << i << "]";
							mesh_.shadowMapMaterialPointLight.setUniform(oss.str(), shadowMatrices[i]);
							oss.str("");
							oss.clear();
						}

						mesh_.shadowMapMaterialPointLight.setUniformLocations();

						//binding uniforms

						for_each(mesh_.shadowMapMaterialPointLight.uniforms.begin(), mesh_.shadowMapMaterialPointLight.uniforms.end(), [](MaterialUniform::UniformVariant& uniform_) {

							std::visit([](auto v) {


								v.setUniformToShader();


								}, uniform_);

							});

						glBindVertexArray(mesh_.VAO);

						list<OglObject3D::SSBOAttribute> extraSSBOAttributes = mesh_.geometry->getExtraSSBOAttributes();
						for_each(extraSSBOAttributes.begin(), extraSSBOAttributes.end(), [](OglObject3D::SSBOAttribute& ssbo) {
							ssbo.Use();
							});

						if (mesh_.personalData["errorMesh"] == "false") {

							glDrawElements(GL_TRIANGLES, mesh_.geometry->indices.size(), GL_UNSIGNED_INT, 0);
						}

						// Make sure the VAO is not changed from the outside
						glBindVertexArray(0);

						int maxAvailableTextureUnits = MyTextureV2::Texture::MaxTextureCount;
						MyTextureV2::Texture::ResetAvailableTextureUnits(maxAvailableTextureUnits);


					}

				}
			}


		}

	}

	void Renderer::RenderSpotLightShadowMaps(OglScene::Scene& scene) {

		list<OglObject3D::Mesh*> meshes = scene.GetMeshes();

		

		list<OglObject3D::Mesh*> meshesCastingShadow;
		if (this->shadowEnabled) {


			auto it = meshes.begin();
			for (; it != meshes.end(); it++) {

				OglObject3D::Mesh* mesh = *it;
				OglObject3D::MeshStandardMaterial* standardMaterial =
					dynamic_cast<OglObject3D::MeshStandardMaterial*>(mesh->material);

				if (standardMaterial != nullptr) {

					if (standardMaterial->shadow.castShadow) {
						meshesCastingShadow.insert(meshesCastingShadow.end(), mesh);
					}
				}

			}

			auto itSpotLight = scene.spotLights.begin();
			for (; itSpotLight != scene.spotLights.end(); itSpotLight++) {

				MyOglLightV2::SpotLight* spotLight = *itSpotLight;
				if (spotLight->shadow.castShadow) {

					spotLight->shadowMap.BindFBO();
					spotLight->shadowMap.CreateViewMatrix(spotLight->WorldPosition, spotLight->WorldDirection);
					spotLight->shadowMap.perspectiveCamera.FOV = spotLight->OuterCutoff * 2.0f; //to cover the whole cone of light
					spotLight->shadowMap.CreatePerspectiveMatrix();
				

					list<OglObject3D::Mesh*>::iterator it = meshesCastingShadow.begin();
					for (; it != meshesCastingShadow.end(); it++) {

						OglObject3D::Mesh& mesh_ = **it;

						if (!mesh_.isShadowMapMaterialSet) {

							mesh_.isShadowMapMaterialSet = true;
							mesh_.setShadowMapMaterial();


						}

						mesh_.shadowMapMaterial.BindShader();
						mesh_.shadowMapMaterial.uniforms = mesh_.material->uniforms;

						mesh_.shadowMapMaterial.setUniform("viewMatrix", spotLight->shadowMap.GetLightViewMatrix());
						mesh_.shadowMapMaterial.setUniform("projectionMatrix", spotLight->shadowMap.GetLightProjectionMatrix());
						mesh_.shadowMapMaterial.setUniform("viewProjectionMatrix", spotLight->shadowMap.GetLightProjectionViewMatrix());


						
						mesh_.shadowMapMaterial.setUniformLocations();

						//binding uniforms

						for_each(mesh_.shadowMapMaterial.uniforms.begin(), mesh_.shadowMapMaterial.uniforms.end(), [](MaterialUniform::UniformVariant& uniform_) {

							std::visit([](auto v) {


								v.setUniformToShader();


								}, uniform_);

							});

						glBindVertexArray(mesh_.VAO);

						list<OglObject3D::SSBOAttribute> extraSSBOAttributes = mesh_.geometry->getExtraSSBOAttributes();
						for_each(extraSSBOAttributes.begin(), extraSSBOAttributes.end(), [](OglObject3D::SSBOAttribute& ssbo) {
							ssbo.Use();
							});

						if (mesh_.personalData["errorMesh"] == "false") {

							glDrawElements(GL_TRIANGLES, mesh_.geometry->indices.size(), GL_UNSIGNED_INT, 0);
						}

						// Make sure the VAO is not changed from the outside
						glBindVertexArray(0);

						int maxAvailableTextureUnits = MyTextureV2::Texture::MaxTextureCount;
						MyTextureV2::Texture::ResetAvailableTextureUnits(maxAvailableTextureUnits);


					}

				}
			}

		}
	}

	void Renderer::RenderDirectionalLightShadowMap(OglScene::Scene& scene) {

		list<OglObject3D::Mesh*> meshes = scene.GetMeshes();
		
		list<OglObject3D::Mesh*> meshesCastingShadow;
		if (this->shadowEnabled) {


			auto it = meshes.begin();
			for (; it != meshes.end(); it++) {

				OglObject3D::Mesh* mesh = *it;
				OglObject3D::MeshStandardMaterial* standardMaterial =
					dynamic_cast<OglObject3D::MeshStandardMaterial*>(mesh->material);

				if (standardMaterial != nullptr) {

					if (standardMaterial->shadow.castShadow) {
						meshesCastingShadow.insert(meshesCastingShadow.end(), mesh);
					}
				}

			}

			auto itDirLight = scene.directionalLights.begin();
			for (; itDirLight != scene.directionalLights.end(); itDirLight++) {

				MyOglLightV2::DirectionalLight* dirLight = *itDirLight;
				if (dirLight->shadow.castShadow) {
					dirLight->shadowMap.BindFBO();

					float dirLightShadowCamPosScale = (dirLight->shadowMap.orthographicCamera.far_ - dirLight->shadowMap.orthographicCamera.near_) / 2; //to position light camera far from origin
					dirLight->shadowMap.SetScale(dirLightShadowCamPosScale);

					dirLight->shadowMap.CreateViewMatrix(dirLight->WorldDirection);
					dirLight->shadowMap.CreateOrthographicMatrix();

					list<OglObject3D::Mesh*>::iterator it = meshesCastingShadow.begin();
					for (; it != meshesCastingShadow.end(); it++) {

						OglObject3D::Mesh& mesh_ = **it;

						if (!mesh_.isShadowMapMaterialSet) {

							mesh_.isShadowMapMaterialSet = true;
							mesh_.setShadowMapMaterial();


						}

						mesh_.shadowMapMaterial.BindShader();
						mesh_.shadowMapMaterial.uniforms = mesh_.material->uniforms;

						mesh_.shadowMapMaterial.setUniform("viewMatrix", dirLight->shadowMap.GetLightViewMatrix());
						mesh_.shadowMapMaterial.setUniform("projectionMatrix", dirLight->shadowMap.GetLightProjectionMatrix());
						mesh_.shadowMapMaterial.setUniform("viewProjectionMatrix",dirLight->shadowMap.GetLightProjectionViewMatrix()
						);
						mesh_.shadowMapMaterial.setUniformLocations();
						
						//binding uniforms
						
						for_each(mesh_.shadowMapMaterial.uniforms.begin(), mesh_.shadowMapMaterial.uniforms.end(), [](MaterialUniform::UniformVariant& uniform_) {

							std::visit([](auto v) {


								v.setUniformToShader();


								}, uniform_);

							});
						
						glBindVertexArray(mesh_.VAO);

						list<OglObject3D::SSBOAttribute> extraSSBOAttributes = mesh_.geometry->getExtraSSBOAttributes();
						for_each(extraSSBOAttributes.begin(), extraSSBOAttributes.end(), [](OglObject3D::SSBOAttribute& ssbo) {
							ssbo.Use();
							});

						if (mesh_.personalData["errorMesh"] == "false") {

							glDrawElements(GL_TRIANGLES, mesh_.geometry->indices.size(), GL_UNSIGNED_INT, 0);
						}

						// Make sure the VAO is not changed from the outside
						glBindVertexArray(0);

						int maxAvailableTextureUnits = MyTextureV2::Texture::MaxTextureCount;
						MyTextureV2::Texture::ResetAvailableTextureUnits(maxAvailableTextureUnits);


					}
					
				}

			} //-
		}


	}

	void Renderer::UpdateMeshMaterialIfEnvironmentMap(OglScene::Scene& scene, MyVectorV3::Vector3f camWorldPos) {

		//this part only once
		if (!isUpdateMeshMaterialIfEnvironmentMap) {

		isUpdateMeshMaterialIfEnvironmentMap = true;

		if (!scene.isEnvironmentMapSet()) {
			return;
		}

		list<OglObject3D::Mesh*> meshes = scene.GetMeshes();

		for_each(meshes.begin(), meshes.end(), [](OglObject3D::Mesh* mesh) {

			OglObject3D::MeshStandardMaterial* stdMat = dynamic_cast<OglObject3D::MeshStandardMaterial*>(mesh->material);

			if (stdMat != nullptr) {
				stdMat->enableEnvironmentMap();
			}

			});


		}

		//this part uniform every render frame
		if (scene.isEnvironmentMapSet()) {
			
			list<OglObject3D::Mesh*> meshes = scene.GetMeshes();

			for_each(meshes.begin(), meshes.end(), [camWorldPos,&scene](OglObject3D::Mesh* mesh) {

				OglObject3D::MeshStandardMaterial* stdMat = dynamic_cast<OglObject3D::MeshStandardMaterial*>(mesh->material);

				if (stdMat != nullptr) {
					
					stdMat->setUniform("envReflection", stdMat->envMapProperties.reflection);
					stdMat->setUniform("envRefraction", stdMat->envMapProperties.refraction);
					stdMat->setUniform("refractionRatio", stdMat->envMapProperties.refractionRatio);

					MyMatrixV3::Matrix3f normalMatrix(mesh->GetMatrix());
					normalMatrix=normalMatrix.Inverse().Transpose();
					stdMat->setUniform("normalMatrix", normalMatrix);

					stdMat->setUniform("cameraWorldPos", camWorldPos);
					stdMat->setUniform("environmentMap", scene.GetEnvironmentMapTexture());


				}

				});



		}


	}

	void Renderer::RenderEnvironmentMap(OglScene::Scene& scene, MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix) {

		if (scene.isEnvironmentMapSet()) {


			MyMatrixV3::Matrix4f viewMatrixWithoutTranslation = viewMatrix;
			viewMatrixWithoutTranslation.m[0][3] = 0;
			viewMatrixWithoutTranslation.m[1][3] = 0;
			viewMatrixWithoutTranslation.m[2][3] = 0;


			//every child of material has those	

			OglObject3D::Mesh* mesh = scene.GetEnvironmentMap();
			mesh->material->setViewProjectionMatrix(viewMatrixWithoutTranslation, projMatrix);
			mesh->material->setModelMatrix(mesh->GetMatrix());

			
			glDepthFunc(GL_LEQUAL);
			mesh->Render();
			glDepthFunc(GL_LESS);
		}

	}

	void Renderer::RenderMeshes(OglScene::Scene& scene, MyVectorV3::Vector3f camWorldPos, MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix) {

		//glEnable(GL_FRAMEBUFFER_SRGB);

		list<OglObject3D::Mesh*> meshes = scene.GetMeshes();

		OglShadow::ShadowMap::windowWidth = OglWindow::Window::width;
		OglShadow::ShadowMap::windowHeight = OglWindow::Window::height;

		//rendering shadow map
		this->enableIfDirLightShadowMaps(scene); //set shadow map's dir light variable
		this->enableIfSpotLightShadowMaps(scene); //set shadow map's spot light variable
		this->enableIfPointLightShadowMaps(scene); //set shadow map's point light variable
		


		this->BindCurrentRenderTarget();
		//glViewport(0, 0, OglWindow::Window::width, OglWindow::Window::height);
		this->ClearCurrentRenderTarget();
		

		list<OglObject3D::Mesh*> meshes_ = scene.GetMeshes();
		for_each(meshes_.begin(), meshes_.end(), [this, &scene, projMatrix, viewMatrix, camWorldPos](OglObject3D::Mesh* mesh) {
			//cout << i++ <<": "<<"opaq" << endl;
			RenderSingleMesh(mesh, scene, camWorldPos, viewMatrix, projMatrix);


			});

		this->clearShadowMaps(scene);



		this->RenderDirectionalLightShadowMap(scene);
		this->RenderSpotLightShadowMaps(scene);
		this->RenderPointLightShadowMaps(scene);

		


		
	}


	void Renderer::RenderSingleMesh(OglObject3D::Mesh* mesh, OglScene::Scene& scene, MyVectorV3::Vector3f camWorldPos,
		MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix) {


		MyMatrixV3::Matrix4f inverseModelMatrix = mesh->GetMatrix().Inverse();


		MyVectorV3::Vector4f camWorld4f(camWorldPos.x, camWorldPos.y, camWorldPos.z, 1);
		MyVectorV3::Vector4f camLocalPos = inverseModelMatrix * camWorld4f;



		//every child of material has those		
		mesh->material->setViewProjectionMatrix(viewMatrix, projMatrix);
		mesh->material->setModelMatrix(mesh->GetMatrix());

		
		OglObject3D::MeshBasicMaterial* basicMaterial = dynamic_cast<OglObject3D::MeshBasicMaterial*>(mesh->material);
		OglObject3D::MeshStandardMaterial* standardMaterial = dynamic_cast<OglObject3D::MeshStandardMaterial*>(mesh->material);


		//special for meshbasicmaterial

		if (basicMaterial != nullptr) {
			basicMaterial->setCameraLocalPos(MyVectorV3::Vector3f(camLocalPos.x, camLocalPos.y, camLocalPos.z));
		
			MyMatrixV3::Matrix3f normalMatrix(mesh->GetMatrix());
			normalMatrix = normalMatrix.Inverse().Transpose();
			basicMaterial->setUniform("normalMatrix", normalMatrix);


		}


		//special for meshstandardmaterial

		if (standardMaterial != nullptr) {

			standardMaterial->setCameraLocalPos(MyVectorV3::Vector3f(camLocalPos.x, camLocalPos.y, camLocalPos.z));
			meshStandardMaterialSpecific(&scene, mesh, camWorldPos);

		}

		mesh->Render();


	}


	void Renderer::RenderMouseEventMeshes(OglScene::Scene& scene, MyVectorV3::Vector3f camWorldPos, MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix) {
		//glEnable(GL_FRAMEBUFFER_SRGB);

		list<OglObject3D::Mesh*> meshes = scene.GetMeshes();


		list<OglObject3D::Mesh*> meshes_ = scene.GetMeshes();
		for_each(meshes_.begin(), meshes_.end(), [this, &scene, projMatrix, viewMatrix, camWorldPos](OglObject3D::Mesh* mesh) {
			//cout << i++ <<": "<<"opaq" << endl;
			RenderMouseEventSingleMesh(mesh, scene, camWorldPos, viewMatrix, projMatrix);


			});




	}
	void Renderer::RenderMouseEventSingleMesh(OglObject3D::Mesh* mesh, OglScene::Scene& scene, MyVectorV3::Vector3f camWorldPos, MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix) {

		MyMatrixV3::Matrix4f inverseModelMatrix = mesh->GetMatrix().Inverse();


		MyVectorV3::Vector4f camWorld4f(camWorldPos.x, camWorldPos.y, camWorldPos.z, 1);
		MyVectorV3::Vector4f camLocalPos = inverseModelMatrix * camWorld4f;



		//every child of material has those		
		mesh->material->setViewProjectionMatrix(viewMatrix, projMatrix);
		mesh->material->setModelMatrix(mesh->GetMatrix());


		OglObject3D::MeshBasicMaterial* basicMaterial = dynamic_cast<OglObject3D::MeshBasicMaterial*>(mesh->material);
		OglObject3D::MeshStandardMaterial* standardMaterial = dynamic_cast<OglObject3D::MeshStandardMaterial*>(mesh->material);


		//special for meshbasicmaterial

		if (basicMaterial != nullptr) {
			basicMaterial->setCameraLocalPos(MyVectorV3::Vector3f(camLocalPos.x, camLocalPos.y, camLocalPos.z));
		}


		//special for meshstandardmaterial

		if (standardMaterial != nullptr) {

			standardMaterial->setCameraLocalPos(MyVectorV3::Vector3f(camLocalPos.x, camLocalPos.y, camLocalPos.z));
			meshStandardMaterialSpecific(&scene, mesh, camWorldPos);

		}

		mesh->Render();


	}


	OglMouseEvents::MouseEventHandler& Renderer::getMouseEventHandler() {
		return this->mouseEventHandler;

	}

	void Renderer::RenderMouseEvent(OglScene::Scene& scene, MyCamera::BasicCamera& cam){
		
		if (!mouseEventHandler.isMouseEventSceneSet()) {
			mouseEventHandler.setMouseEventScene(&scene);
		}

	

		MyMatrixV3::Matrix4f viewMatrix = cam.GetViewMatrix();
		MyMatrixV3::Matrix4f projMatrix = cam.GetProjectionMat();

		variant<OglRenderTarget::RenderTarget*, GLuint> oldRenderTarget = currentRenderTarget;
		setRenderTarget(&mouseEventHandler.getRenderTarget());
		this->BindCurrentRenderTarget();
		this->ClearCurrentRenderTarget();
		mouseEventHandler.Update();
		this->RenderMouseEventMeshes(mouseEventHandler.getScene(), cam.GetPos(), viewMatrix, projMatrix);
		currentRenderTarget = oldRenderTarget;


		if (std::holds_alternative<GLuint>(oldRenderTarget)) {
			setRenderTarget(get<GLuint>(oldRenderTarget));

		}

		if (std::holds_alternative<OglRenderTarget::RenderTarget*>(oldRenderTarget)) {
			setRenderTarget(get<OglRenderTarget::RenderTarget*>(oldRenderTarget));

		}

	}
	void Renderer::RenderMouseEvent(OglScene::Scene& scene, MyCamera::OrbitalCamera& cam){
		
		if (!mouseEventHandler.isMouseEventSceneSet()) {
			mouseEventHandler.setMouseEventScene(&scene);
		}

	

		MyMatrixV3::Matrix4f viewMatrix = cam.GetViewMatrix();
		MyMatrixV3::Matrix4f projMatrix = cam.GetProjectionMatrix();


		variant<OglRenderTarget::RenderTarget*, GLuint> oldRenderTarget = currentRenderTarget;
		setRenderTarget(&mouseEventHandler.getRenderTarget());
		this->BindCurrentRenderTarget();
		this->ClearCurrentRenderTarget();
		mouseEventHandler.Update();
		this->RenderMouseEventMeshes(mouseEventHandler.getScene(), cam.GetPos(), viewMatrix, projMatrix);
		currentRenderTarget = oldRenderTarget;

		if (std::holds_alternative<GLuint>(oldRenderTarget)) {
			setRenderTarget(get<GLuint>(oldRenderTarget));

		}

		if (std::holds_alternative<OglRenderTarget::RenderTarget*>(oldRenderTarget)) {
			setRenderTarget(get<OglRenderTarget::RenderTarget*>(oldRenderTarget));

		}

	
	}
	void Renderer::RenderMouseEvent(OglScene::Scene& scene, MyCamera::Camera& cam) {
	
		if (!mouseEventHandler.isMouseEventSceneSet()) {
			mouseEventHandler.setMouseEventScene(&scene);
		}

		MyMatrixV3::Matrix4f viewProjectionMatrix = cam.GetViewProjectionMatrix();

		
		variant<OglRenderTarget::RenderTarget*, GLuint> oldRenderTarget = currentRenderTarget;
		auto oldClearFunc = this->ClearCurrentRenderTarget;
		setRenderTarget(&mouseEventHandler.getRenderTarget());
		this->BindCurrentRenderTarget();
		this->ClearCurrentRenderTarget();
		mouseEventHandler.Update();
		this->RenderMouseEventMeshes(mouseEventHandler.getScene(), cam.GetPosition(), cam.GetViewMatrix(), cam.GetProjectionMatrix());
		OglWindow::Window::isMouseMove = false;
		
		if (std::holds_alternative<GLuint>(oldRenderTarget)) {
			setRenderTarget(get<GLuint>(oldRenderTarget));
			this->ClearCurrentRenderTarget = oldClearFunc;
			
		}

		if (std::holds_alternative<OglRenderTarget::RenderTarget*>(oldRenderTarget)) {
			setRenderTarget(get<OglRenderTarget::RenderTarget*>(oldRenderTarget));

		}
		
	}


	void Renderer::Render(OglScene::Scene& scene, MyCamera::BasicCamera& cam) {

		cam.OnRender();

		this->BindCurrentRenderTarget();
		this->ClearCurrentRenderTarget();

		//apply world transform of assimp model to each mesh
		this->ApplyWorldMatrixAssimp(scene);

		this->UpdateMeshMaterialIfEnvironmentMap(scene, cam.GetPos());


		MyMatrixV3::Matrix4f viewMatrix = cam.GetViewMatrix();
		MyMatrixV3::Matrix4f projMatrix = cam.GetProjectionMat();

		this->RenderMeshes(scene, cam.GetPos(), viewMatrix, projMatrix);

		if (environmentMapRenderingEnabled) {

			this->RenderEnvironmentMap(scene, viewMatrix, projMatrix);
		}
		if (mouseEventRenderingEnabled) {

			this->RenderMouseEvent(scene, cam);
		}
		//reverse world matrix of assimp model
		this->ApplyReverseWorldMatrixAssimp(scene);
		
	}
	void Renderer::Render(OglScene::Scene& scene, MyCamera::OrbitalCamera& cam) {

		cam.updateCamera();


		this->BindCurrentRenderTarget();
		this->ClearCurrentRenderTarget();

		//apply world transform of assimp model to each mesh
		this->ApplyWorldMatrixAssimp(scene);


		this->UpdateMeshMaterialIfEnvironmentMap(scene, cam.GetPos());


		MyMatrixV3::Matrix4f viewMatrix = cam.GetViewMatrix();
		MyMatrixV3::Matrix4f projMatrix = cam.GetProjectionMatrix();

		this->RenderMeshes(scene, cam.GetPos(), viewMatrix, projMatrix);

		if (environmentMapRenderingEnabled) {

			this->RenderEnvironmentMap(scene, viewMatrix, projMatrix);
		}
		if (mouseEventRenderingEnabled) {

			this->RenderMouseEvent(scene, cam);
		}

		//reverse world matrix of assimp model
		this->ApplyReverseWorldMatrixAssimp(scene);

		
	}

	

	void Renderer::Render(OglScene::Scene& scene, MyCamera::Camera& cam) {

		
		this->BindCurrentRenderTarget();
		this->ClearCurrentRenderTarget();

	
		//apply world transform of assimp model to each mesh
		this->ApplyWorldMatrixAssimp(scene);

		this->UpdateMeshMaterialIfEnvironmentMap(scene, cam.GetPosition());

		
		MyMatrixV3::Matrix4f viewProjectionMatrix = cam.GetViewProjectionMatrix();
	
		this->RenderMeshes(scene, cam.GetPosition(), cam.GetViewMatrix(), cam.GetProjectionMatrix());
		
	
		if (environmentMapRenderingEnabled) {

		this->RenderEnvironmentMap(scene, cam.GetViewMatrix(), cam.GetProjectionMatrix());
		}
		
		if (mouseEventRenderingEnabled) {

		this->RenderMouseEvent(scene, cam);
		}
		

		//reverse world matrix of assimp model
		this->ApplyReverseWorldMatrixAssimp(scene);

		
	}


}