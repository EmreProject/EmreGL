#ifndef ogl_compute_shader_h
#define ogl_compute_shader_h


#include <GL/glew.h>
#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Projection.h"
#include "ogl_Camera.h"
#include "oglWindow.h"
#include <chrono>


using namespace std;



namespace OglComputeShader {



	string LoadComputeShaderFromFile(string computeShaderFilePath);

	class SSBOBuffer {

	private:
		int binding;
		GLuint bufferId;
		
		int totalSizeAsByte;
	public:

		SSBOBuffer(int binding);
		SSBOBuffer(int binding, GLuint bufferId);

		GLuint getBufferId() { return bufferId; }
		void setBinding(int binding);

		//must
		void setSize(int totalSizeAsByte);
		void Create();

		//optional if fill or change data on cpu side after Create() is called
		template<typename t>
		void FillData(t* data) {

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferId);

			glBufferSubData(
				GL_SHADER_STORAGE_BUFFER,
				0,                          // byte offset
				totalSizeAsByte, // update size in bytes
				data                 // new CPU data
			);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}


		template<typename t>
		void FillData(t* data,int byteOffset, int dataSizeAsByte) {

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferId);

			glBufferSubData(
				GL_SHADER_STORAGE_BUFFER,
				byteOffset,                          // byte offset
				dataSizeAsByte, // update size in bytes
				data                 // new CPU data
			);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
	
		void Bind();

		
	};


	class Image2D {

	private:
		
		int binding;
		GLuint bufferId;
		
		int width;
		int height;

	public:
	
		GLenum internalFormat = GL_RGBA32F; //RGBA32F, RGBA16F, RGBA8...
		GLenum wrapS = GL_CLAMP_TO_EDGE; //default
		GLenum wrapT = GL_CLAMP_TO_EDGE; //default
		GLenum minFilter = GL_NEAREST; //default
		GLenum magFilter = GL_NEAREST; //default

		Image2D(int binding);
		Image2D(int binding,GLuint bufferId);
		
		void setBinding(int binding);

		//must
		void setDimensions(int width, int height);
		void Create();


		//optional if fill or change data on cpu side after Create() is called
		template<typename t> 
		void FillData(t* data) {
			glBindTexture(GL_TEXTURE_2D, bufferId);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, data);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		template<typename t> //x, y, width, height these are pixels
		void FillData(t* data, int x, int y, int width, int height) {
			glBindTexture(GL_TEXTURE_2D, bufferId);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_FLOAT, data);
			glBindTexture(GL_TEXTURE_2D, 0);
		}


		void Bind();

		MyTextureV2::Texture getTexture();

	};

	class Image3D {

	private:

		int binding;
		GLuint bufferId;

		int width;
		int height;
		int depth;

	public:

		GLenum internalFormat = GL_RGBA32F; //RGBA32F, RGBA16F, RGBA8...
		GLenum wrapS = GL_CLAMP_TO_EDGE; //default
		GLenum wrapT = GL_CLAMP_TO_EDGE; //default
		GLenum wrapR = GL_CLAMP_TO_EDGE; //default
		GLenum minFilter = GL_NEAREST; //default
		GLenum magFilter = GL_NEAREST; //default

		Image3D(int binding);
		Image3D(int binding, GLuint bufferId);

		void setBinding(int binding);

		//must
		void setDimensions(int width, int height,int depth);
		void Create();


		//optional if fill or change data on cpu side after Create() is called
		template<typename t>
		void FillData(t* data) {
			glBindTexture(GL_TEXTURE_3D, bufferId);

			glTexSubImage3D(
				GL_TEXTURE_3D,
				0,              // mip level
				0, 0, 0,         // xoffset, yoffset, zoffset
				width,
				height,
				depth,
				GL_RGBA,         // CPU data layout
				GL_FLOAT,        // CPU data type
				data);

			glBindTexture(GL_TEXTURE_3D, 0);
		}

		template<typename t> //x, y, width, height these are pixels
		void FillData(t* data, int x, int y, int z, int width, int height, int depth) {
			glBindTexture(GL_TEXTURE_3D, bufferId);

			glTexSubImage3D(
				GL_TEXTURE_3D,
				0,              // mip level
				x,y,z,         // xoffset, yoffset, zoffset
				width,
				height,
				depth,
				GL_RGBA,         // CPU data layout
				GL_FLOAT,        // CPU data type
				data);

			glBindTexture(GL_TEXTURE_3D, 0);
		}


		void Bind();


		MyTextureV2::Texture getTexture();

	};


	class Shader {
	private:
	
		
		list<MaterialUniform::UniformVariant> uniforms;
		template<typename t>
		bool updateIfUniformAlreadyExists(string uniName, t value) {

			bool ret = false;
			auto it = this->uniforms.begin();
			for (; it != uniforms.end(); it++) {

				std::visit([&](auto& v) {
					using U = std::decay_t<decltype(v)>;
					if (v.uniformName == uniName) {
						if constexpr (std::is_same_v<decltype(v.value), t>) {
							v.value = value;
							ret = true;
						}
					}
					}, *it); 
			}

			return ret;
		}
		void addUniformIfNotExists(MaterialUniform::UniformVariant& uniformVariant);

		void GenerateShaderProgram(string computeShaderCode);

		unsigned int globalWorkGroupSizeX = 1;
		unsigned int globalWorkGroupSizeY = 1;
		unsigned int globalWorkGroupSizeZ = 1;

		list<SSBOBuffer> ssboBuffers;
		list<Image2D> image2DBuffers;
		list<Image3D> image3DBuffers;

		

	public:

		GLuint shaderProgram;
	

		Shader(string computeShaderCode);
		~Shader(){}
		
		bool debug = false; //if true, then print debug messages
	
		void setGlobalWorkGroupSize(float x, float y, float z);

		void AddSSBOBuffer(SSBOBuffer& ssbo);
		void AddImage2DBuffer(Image2D& image);
		void AddImage3DBuffer(Image3D& image);
		
		void Run();

		template<typename t>
		void setUniform(string uniName, t value) {}

		template<>
		void setUniform<int>(string uniName, int value) {
			bool isAlready=updateIfUniformAlreadyExists(uniName,value);
			if (!isAlready) {

				MaterialUniform::UniformVariant newUni(MaterialUniform::IntUniform(uniName, value));
				addUniformIfNotExists(newUni);

			}
		}


		template<>
		void setUniform<bool>(string uniName, bool value) {
			bool isAlready = updateIfUniformAlreadyExists(uniName, value);
			if (!isAlready) {

				MaterialUniform::UniformVariant newUni(MaterialUniform::BoolUniform(uniName, value));
				addUniformIfNotExists(newUni);

			}
		}

		template<>
		void setUniform<float>(string uniName, float value) {
			bool isAlready=updateIfUniformAlreadyExists(uniName, value);
			if (!isAlready) {

				MaterialUniform::UniformVariant newUni(MaterialUniform::FloatUniform(uniName, value));
				addUniformIfNotExists(newUni);
			}
		}

		template<>
		void setUniform<MyVectorV3::Vector2f>(string uniName, MyVectorV3::Vector2f value) {
			bool isAlready = updateIfUniformAlreadyExists(uniName, value);
			if (!isAlready) {

				MaterialUniform::UniformVariant newUni(MaterialUniform::Vec2fUniform(uniName, value));
				addUniformIfNotExists(newUni);
			}
		}

		template<>
		void setUniform<MyVectorV3::Vector3f>(string uniName, MyVectorV3::Vector3f value) {
			bool isAlready = updateIfUniformAlreadyExists(uniName, value);
			if (!isAlready) {

				MaterialUniform::UniformVariant newUni(MaterialUniform::Vec3fUniform(uniName, value));
				addUniformIfNotExists(newUni);
			}
		}

		template<>
		void setUniform<MyVectorV3::Vector4f>(string uniName, MyVectorV3::Vector4f value) {
			bool isAlready = updateIfUniformAlreadyExists(uniName, value);
			if (!isAlready) {

				MaterialUniform::UniformVariant newUni(MaterialUniform::Vec4fUniform(uniName, value));
				addUniformIfNotExists(newUni);
			}
		}

		template<>
		void setUniform<MyMatrixV3::Matrix3f>(string uniName, MyMatrixV3::Matrix3f value) {
			bool isAlready=updateIfUniformAlreadyExists(uniName, value);
			if (!isAlready) {

				MaterialUniform::UniformVariant newUni(MaterialUniform::Mat3fUniform(uniName, value));
				addUniformIfNotExists(newUni);
			}
		}

		template<>
		void setUniform<MyMatrixV3::Matrix4f>(string uniName, MyMatrixV3::Matrix4f value) {
			bool isAlready = updateIfUniformAlreadyExists(uniName, value);
			if (!isAlready) {

				MaterialUniform::UniformVariant newUni(MaterialUniform::Mat4fUniform(uniName, value));
				addUniformIfNotExists(newUni);
			}
		}

		template<>
		void setUniform<MyTextureV2::Texture>(string uniName, MyTextureV2::Texture value) {
			bool isAlready = updateIfUniformAlreadyExists(uniName, value);
			if (!isAlready) {

				MaterialUniform::UniformVariant newUni(MaterialUniform::TextureUniform(uniName, value));
				addUniformIfNotExists(newUni);
			}
		}


		
	
	};
} // namespace OglComputeShader


#endif // !ogl_compute_shader_h


