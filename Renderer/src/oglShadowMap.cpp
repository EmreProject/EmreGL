#include "oglShadowMap.h"


namespace OglShadow {

	 float ShadowMap::windowWidth=0;
	 float ShadowMap::windowHeight=0;

	ShadowMap::ShadowMap() {

		

	}

	void ShadowMap::Load() {

		this->isLoaded_ = true;
		CreateFbo();
	}

	void ShadowMap::CreateFbo() {

		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		
		glGenFramebuffers(1, &depthMapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthMap, 0);
		
		GLuint depthRBO;
		glGenRenderbuffers(1, &depthRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);

		// For example, a 24-bit depth buffer:
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, SHADOW_WIDTH, SHADOW_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);

		// Tell OpenGL we will draw to the color attachment 0
		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);


		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	bool ShadowMap::isLoaded() {

		return this->isLoaded_;
	}

	MyTextureV2::Texture ShadowMap::GetShadowMapTexture() {

		MyTextureV2::Texture ret(depthMap);
		ret.wrapS = GL_CLAMP_TO_BORDER;
		ret.wrapT = GL_CLAMP_TO_BORDER;
		return ret;
	}


	void ShadowMap::BindFBO() {


		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	

	}

	void ShadowMap::ClearFBO() {
		
		this->BindFBO();
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		//const float clearColor[2] = { 1.0f, 1.0f };
		//glClearBufferfv(GL_COLOR, 0, clearColor);
	}

	void ShadowMap::UnbindFBO() {

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, ShadowMap::windowWidth,ShadowMap::windowHeight);
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	}


	//DIRECTIONAL SHADOW MAP

	MyMatrixV3::Matrix4f DirectionalShadowMap::CreateViewMatrix(MyVectorV3::Vector3f direction) {

		direction.Normalize();
		MyVectorV3::Vector3f Pos = direction * -scale;

		MyVectorV3::Vector3f Up(0, 1, 0);


		MyVectorV3::Vector3f N = direction;
		N.Normalize();

		MyVectorV3::Vector3f UpNorm = Up;
		UpNorm.Normalize();

		MyVectorV3::Vector3f U;
		U = UpNorm.Cross(N);
		U.Normalize();

		MyVectorV3::Vector3f V = N.Cross(U);
		MyMatrixV3::Matrix4f CameraRotation;


		CameraRotation.m[0][0] = U.x;   CameraRotation.m[0][1] = U.y;   CameraRotation.m[0][2] = U.z;   CameraRotation.m[0][3] = 0.0f;
		CameraRotation.m[1][0] = V.x;   CameraRotation.m[1][1] = V.y;   CameraRotation.m[1][2] = V.z;   CameraRotation.m[1][3] = 0.0f;
		CameraRotation.m[2][0] = N.x;   CameraRotation.m[2][1] = N.y;   CameraRotation.m[2][2] = N.z;   CameraRotation.m[2][3] = 0.0f;
		CameraRotation.m[3][0] = 0.0f;  CameraRotation.m[3][1] = 0.0f;  CameraRotation.m[3][2] = 0.0f;  CameraRotation.m[3][3] = 1.0f;


		MyMatrixV3::Matrix4f CameraTranslation;
		CameraTranslation.InitTranslationTransform(-Pos.x, -Pos.y, -Pos.z);


		this->viewMatrix = CameraRotation * CameraTranslation;
		return  viewMatrix;

	}

	MyMatrixV3::Matrix4f DirectionalShadowMap::CreateOrthographicMatrix() {

	
		
		MyMatrixV3::Matrix4f lightOrthographicProjection(
			2.0f / (orthographicCamera.right - orthographicCamera.left), 0.0f, 0.0f, (-orthographicCamera.left-orthographicCamera.right) / (orthographicCamera.right-orthographicCamera.left),
			0.0f, 2.0f/(orthographicCamera.top-orthographicCamera.bottom), 0.0f, (-orthographicCamera.top-orthographicCamera.bottom)/(orthographicCamera.top-orthographicCamera.bottom),
			0.0f,0.0f,2.0f/(orthographicCamera.far_ - orthographicCamera.near_), (-orthographicCamera.far_ - orthographicCamera.near_)/(orthographicCamera.far_ - orthographicCamera.near_),
			0.0f,0.0f,0.0f,1.0f
			);


		this->projectionMatrix = lightOrthographicProjection;
		return lightOrthographicProjection;

	}

	void DirectionalShadowMap::SetScale(float scale_) {

		this->scale = scale_;
	}

	MyMatrixV3::Matrix4f DirectionalShadowMap::GetLightProjectionViewMatrix() {

		
		return projectionMatrix * viewMatrix;

	}
	MyMatrixV3::Matrix4f DirectionalShadowMap::GetLightViewMatrix() {

		return viewMatrix;
	}
	MyMatrixV3::Matrix4f DirectionalShadowMap::GetLightProjectionMatrix() {

		return projectionMatrix;

	}


	// SPOTLIGHT SHADOW MAP ------------------------------------



	MyMatrixV3::Matrix4f SpotLightShadowMap::CreateViewMatrix(MyVectorV3::Vector3f& position, MyVectorV3::Vector3f& direction) {


		MyMatrixV3::Matrix4f CameraTranslation;
		CameraTranslation.InitTranslationTransform(-position.x, -position.y, -position.z);

		MyMatrixV3::Matrix4f CameraRotateTrans;
		direction.Normalize();
		CameraRotateTrans.InitCameraTransform(direction, up);

	
		this->viewMatrix= CameraRotateTrans * CameraTranslation;
		return viewMatrix;

	}
	
	MyMatrixV3::Matrix4f SpotLightShadowMap::CreatePerspectiveMatrix() {

		MyProjection::PersProjInfo perspective;
		perspective.FOV = perspectiveCamera.FOV; //no need touch it should be twice of spotlight outercutpff
		perspective.Width = perspectiveCamera.width;
		perspective.Height = perspectiveCamera.height;
		perspective.zNear = perspectiveCamera.zNear;
		perspective.zFar = perspectiveCamera.zFar;

		
		this->projectionMatrix.InitPersProjTransform(perspective);

		return projectionMatrix;
	}


	MyMatrixV3::Matrix4f SpotLightShadowMap::GetLightProjectionViewMatrix() {

		return projectionMatrix * viewMatrix;
		

	}
	MyMatrixV3::Matrix4f SpotLightShadowMap::GetLightViewMatrix() {
		return viewMatrix;
	}
	MyMatrixV3::Matrix4f SpotLightShadowMap::GetLightProjectionMatrix() {

		return projectionMatrix;

	}


	// POINT LIGHT SHADOW MAP ------------------------------------------------------------------------------------------

	void PointLightShadowMap::CreateFbo() {
	
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);

		for (int i = 0; i < 6; i++) {
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB32F,              // store depth + depth² (VSM)
				SHADOW_WIDTH,
				SHADOW_HEIGHT,
				0,
				GL_RGB,
				GL_FLOAT,
				nullptr
			);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, borderColor);


		glGenFramebuffers(1, &depthMapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthMap, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, depthMap, 0);

		GLuint depthCube;
		glGenTextures(1, &depthCube);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCube);

		for (int i = 0; i < 6; i++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_DEPTH_COMPONENT24,
				SHADOW_WIDTH, SHADOW_HEIGHT,
				0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


		// layered depth (cubemap)
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCube, 0);

		// Tell OpenGL we will draw to the color attachment 0
		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);


		GLenum s = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (s != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "FBO incomplete status = 0x" << std::hex << s << std::dec << "\n";
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	}

	MyMatrixV3::Matrix4f PointLightShadowMap::GetLightProjectionViewMatrix() {
		//JUST FOR FILLING NO USE....
		MyMatrixV3::Matrix4f projectionMatrix = GetLightProjectionMatrix();
		MyMatrixV3::Matrix4f viewMatrix = GetLightViewMatrix();
		return projectionMatrix * viewMatrix;
	}

	MyMatrixV3::Matrix4f PointLightShadowMap::GetLightViewMatrix() {
		//JUST FOR FILLING NO USE....
		MyMatrixV3::Matrix4f identity;
		return identity;
	}

	MyMatrixV3::Matrix4f PointLightShadowMap::GetLightProjectionMatrix() {
		return projectionMatrix;
	}

	vector<MyMatrixV3::Matrix4f> PointLightShadowMap::GetLightProjectionViewMatrices() {

		vector<MyMatrixV3::Matrix4f> shadowMatrices = this->viewMatrices;
		MyMatrixV3::Matrix4f projMatrix = this->projectionMatrix;
		transform(shadowMatrices.begin(), shadowMatrices.end(), shadowMatrices.begin(), [&projMatrix](MyMatrixV3::Matrix4f& viewMatrix) {

			return projMatrix * viewMatrix;
			});

		return shadowMatrices;

	}
	vector<MyMatrixV3::Matrix4f> PointLightShadowMap::GetLightViewMatrices() {

		return this->viewMatrices;
	}


	MyMatrixV3::Matrix4f PointLightShadowMap::CreatePerspectiveMatrix() {

		MyProjection::PersProjInfo perspective;
		perspective.FOV = perspectiveCamera.FOV; //no need touch it should be twice of spotlight outercutpff
		perspective.Width = perspectiveCamera.width;
		perspective.Height = perspectiveCamera.height;
		perspective.zNear = perspectiveCamera.zNear;
		perspective.zFar = perspectiveCamera.zFar;


	
		projectionMatrix.InitPersProjTransform(perspective);

		return projectionMatrix;


	}



	vector<MyMatrixV3::Matrix4f> PointLightShadowMap::CreateViewMatrices(MyVectorV3::Vector3f lightPos) {

		
		
		MyMatrixV3::Matrix4f right;
		right=CreateViewMatrix(lightPos, MyVectorV3::Vector3f(-1, 0, 0), MyVectorV3::Vector3f(0, -1, 0));

		MyMatrixV3::Matrix4f left;
		left = CreateViewMatrix(lightPos, MyVectorV3::Vector3f(1, 0, 0), MyVectorV3::Vector3f(0, -1, 0));

		MyMatrixV3::Matrix4f up;
		up = CreateViewMatrix(lightPos, MyVectorV3::Vector3f(0, 1, 0), MyVectorV3::Vector3f(0, 0, 1));

		MyMatrixV3::Matrix4f down;
		down = CreateViewMatrix(lightPos, MyVectorV3::Vector3f(0, -1, 0), MyVectorV3::Vector3f(0, 0, -1));

		MyMatrixV3::Matrix4f forward;
		forward = CreateViewMatrix(lightPos, MyVectorV3::Vector3f(0, 0, 1), MyVectorV3::Vector3f(0, -1, 0));

		MyMatrixV3::Matrix4f back;
		back = CreateViewMatrix(lightPos, MyVectorV3::Vector3f(0, 0, -1), MyVectorV3::Vector3f(0, -1, 0));

		viewMatrices = vector<MyMatrixV3::Matrix4f>(6);
		viewMatrices[0]=right;
		viewMatrices[1]=left;
		viewMatrices[2]=up;
		viewMatrices[3]=down;
		viewMatrices[4]=forward;
		viewMatrices[5]=back;
		
		
		
		return viewMatrices;


	}


	MyMatrixV3::Matrix4f PointLightShadowMap::CreateViewMatrix(MyVectorV3::Vector3f position, MyVectorV3::Vector3f direction, MyVectorV3::Vector3f up) {


		MyMatrixV3::Matrix4f CameraTranslation;
		CameraTranslation.InitTranslationTransform(-position.x, -position.y, -position.z);

		MyMatrixV3::Matrix4f CameraRotateTrans;
		CameraRotateTrans.InitCameraTransform(direction, up);


		return CameraRotateTrans * CameraTranslation;

	}


	MyTextureV2::CubeTexture PointLightShadowMap::GetShadowMapTexture() {

		MyTextureV2::CubeTexture ret(depthMap);
		return ret;

	}

}