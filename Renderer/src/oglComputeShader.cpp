#include "oglComputeShader.h"

namespace OglComputeShader {

	//-------------------------------SSBO-------------------------------------------------
	SSBOBuffer::SSBOBuffer(int binding) {

		this->binding = binding;
		glGenBuffers(1, &bufferId);
	}
	SSBOBuffer::SSBOBuffer(int binding, GLuint bufferId) {

		this->binding = binding;
		this->bufferId = bufferId;
	}

	void SSBOBuffer::setBinding(int binding) {
		this->binding = binding;
	}

	void SSBOBuffer::setSize(int totalSizeAsByte) {


		this->totalSizeAsByte = totalSizeAsByte;
	}
	void SSBOBuffer::Create() {

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferId);
		glBufferData(GL_SHADER_STORAGE_BUFFER, totalSizeAsByte, nullptr, GL_DYNAMIC_DRAW); // allocate/orphan
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	}
	
	

	void SSBOBuffer::Bind() {

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding,bufferId);

	}

	//IMAGE2D ------------------------------------------------


	Image2D::Image2D(int binding) {

		this->binding = binding;
		glGenTextures(1, &bufferId);

	}
	Image2D::Image2D(int binding, GLuint bufferId){

		this->binding = binding;
		this->bufferId = bufferId;
	}

	void Image2D::setBinding(int binding) {
		this->binding = binding;
	}

	void Image2D::setDimensions(int width, int height) {

		this->width = width;
		this->height = height;

	}
	void Image2D::Create() {

		glBindTexture(GL_TEXTURE_2D, this->bufferId);

		glTexStorage2D(
			GL_TEXTURE_2D,
			1,
			this->internalFormat,
			width,
			height
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->magFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapT);
	
	

		glBindTexture(GL_TEXTURE_2D, 0);
	}


	void Image2D::Bind() {

		glBindImageTexture(
			this->binding,              // binding = 0 in shader
			this->bufferId,
			0,              // mip level
			GL_FALSE,       // not layered for 2D texture
			0,              // layer ignored
			GL_READ_WRITE,  // shader writes
			this->internalFormat
		);
	}


	MyTextureV2::Texture Image2D::getTexture() {


		MyTextureV2::Texture tex(this->bufferId);
		return tex;

	}

	// IMAGE3D ------------------------------------------------


	Image3D::Image3D(int binding){

		this->binding = binding;
		glGenTextures(1, &bufferId);

		}
	Image3D::Image3D(int binding, GLuint bufferId){

		this->binding = binding;
		this->bufferId = bufferId;

		}

	void Image3D::setBinding(int binding) {
		this->binding = binding;
	}

	void Image3D::setDimensions(int width, int height, int depth){

		this->width = width;
		this->height = height;
		this->depth = depth;

		}
	void Image3D::Create() {


		glBindTexture(GL_TEXTURE_3D, bufferId);

		glTexStorage3D(
			GL_TEXTURE_3D,
			1,
			this->internalFormat,
			width,
			height,
			depth
		);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, this->minFilter);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, this->magFilter);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, this->wrapS);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, this->wrapT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, this->wrapR);

		glBindTexture(GL_TEXTURE_3D, 0);

	}

	void Image3D::Bind() {
		glBindImageTexture(
			this->binding,              // binding = 0 in shader
			this->bufferId,
			0,              // mip level
			GL_TRUE,       // layered for 3D texture
			0,              // layer ignored
			GL_READ_WRITE,  // shader writes
			this->internalFormat
		);
	}

	MyTextureV2::Texture Image3D::getTexture() {

		MyTextureV2::Texture tex(this->bufferId);
		tex.textureType = MyTextureV2::Texture3D;
		return tex;
	}

	// COMPUTE SHADER ------------------------------------------------
	string LoadComputeShaderFromFile(string computeShaderFilePath) {

		ifstream iff;
		iff.open(computeShaderFilePath);

		if (!iff.is_open()) {
			cout << "Error! Unable to open file: " << computeShaderFilePath << endl;
			return "";
		}

		ostringstream oss;

		while (!iff.eof()) {
			string line;
			getline(iff, line, '\n');
			oss << line << endl;
		}

		iff.close();

		return oss.str();
	}





	Shader::Shader(string computeShaderCode) {


		this->GenerateShaderProgram(computeShaderCode);
		


	}

	


	void Shader::GenerateShaderProgram(string computeShaderCode) {


		GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
		 const char* shaderSource = computeShaderCode.c_str();
		glShaderSource(shader, 1, &shaderSource, nullptr);
		glCompileShader(shader);

		// Check compilation
		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char log[512];
			glGetShaderInfoLog(shader, 512, nullptr, log);
			std::cerr << "Compute Shader Compile Error:\n" << log << std::endl;
			
		}

		this->shaderProgram  = glCreateProgram();
		glAttachShader(shaderProgram, shader);
		glLinkProgram(shaderProgram);
		
		GLint ok = GL_FALSE;                                   //  NEW
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);             //  NEW
		if (!ok) {
			GLchar log[4096];
			glGetProgramInfoLog(shaderProgram, sizeof(log), nullptr, log);
			std::cerr << "LINK FAILURE in hierarchicalMerge.comp:\n"
				<< log << std::endl;
		}
		glDeleteShader(shader); // shader is now linked into program



	}

	void Shader::AddSSBOBuffer(SSBOBuffer& ssbo) {

		this->ssboBuffers.insert(this->ssboBuffers.end(), ssbo);

	}

	void Shader::AddImage2DBuffer(Image2D& image) {
		this->image2DBuffers.insert(this->image2DBuffers.end(), image);
	}
	
	void Shader::AddImage3DBuffer(Image3D& image) {

		this->image3DBuffers.insert(this->image3DBuffers.end(), image);
	}

	void Shader::setGlobalWorkGroupSize(float x, float y, float z) {


		this->globalWorkGroupSizeX = x;
		this->globalWorkGroupSizeY = y;
		this->globalWorkGroupSizeZ = z;

	

	}
	
	void  Shader::addUniformIfNotExists(MaterialUniform::UniformVariant& uni) {

		std::visit([this](auto& uniform) {
			uniform.uniformLocation = glGetUniformLocation(shaderProgram, uniform.uniformName.c_str());
			}, uni);

		this->uniforms.insert(this->uniforms.end(), uni);
	}

	void Shader::Run() {


		glUseProgram(shaderProgram);

		for (auto& uniform : uniforms) {
			std::visit([this](auto& v) {
				v.setUniformToShader();
				}, uniform);
		}

		for_each(ssboBuffers.begin(), ssboBuffers.end(), [this](SSBOBuffer& buffer) {
			buffer.Bind();
			});

		for_each(image2DBuffers.begin(), image2DBuffers.end(), [this](Image2D& image) {
			image.Bind();
			});

		for_each(image3DBuffers.begin(), image3DBuffers.end(), [this](Image3D& image) {
			image.Bind();
			});

		glDispatchCompute(globalWorkGroupSizeX, globalWorkGroupSizeY, globalWorkGroupSizeZ);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT ); // VBO/attrib visibility

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
			GL_TEXTURE_FETCH_BARRIER_BIT);

	}

}