#ifndef ogl_texture_h
#define ogl_texture_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Projection.h"
#include "ogl_Camera.h"
#include "stb_image.h"
#include <cassert>

using namespace std;


namespace MyTextureV2 {

	enum TextureType {
		Texture2D, Texture3D, TextureCube
	};

	class Texture {

	private:

		string fileLocation;
		GLuint texture;
		//GLenum textureUnit;//like GL_TEXTURE0, GL_TEXTURE1
		//int shaderValue;//0,1,2,3... for GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3...
		bool isLoad = false;
	
	

	public:

		
		static int MaxTextureCount; //for setting texture unit like GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3...
		static bool IsMaxTextureCountSet; //if true, then set max texture count, if false, then set default texture count
		static vector<int> AvailableTextureUnits; //for checking available texture units, like GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3...
		static void SetMaxTextureCount();
		static void ResetAvailableTextureUnits(int);
		static int GetCurrentTextureUnitIndex();
		
		
		
		GLint internalFormat = GL_RGBA; //RGBA32...
		GLenum format = GL_RGBA; //GL_RGBA...
		GLenum type = GL_UNSIGNED_BYTE; //GL_UNSIGNED_BYTE, GL_FLOAT...
		GLenum wrapS = GL_REPEAT; //default
		GLenum wrapT = GL_REPEAT; //default
		GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR; //default
		GLenum magFilter = GL_LINEAR; //default
		bool isGenerateMipmap = true; //default, if false no mipmap generated
		TextureType textureType = TextureType::Texture2D; //default


		MyVectorV3::Vector4f borderColor = MyVectorV3::Vector4f(1, 1, 1, 1);//if wrap glClampToBorder, then this color is used


		Texture();
		Texture(string fileLoc, GLint internalFormat = GL_RGBA, GLenum format = GL_RGBA,
			GLenum type = GL_UNSIGNED_BYTE,GLenum wrapS = GL_REPEAT,GLenum wrapT = GL_REPEAT,
			GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR,GLenum magFilter = GL_LINEAR,bool isGenerateMipmap = true);
		Texture(GLuint textureId);
		Texture(const MyTextureV2::Texture& tex);
		~Texture();

		void LoadTexture(string fileLoc);
		bool isLoaded();
		void BindTexture(GLenum textureUnit);
		 
		GLuint getTextureId();
		
		Texture& operator=(const MyTextureV2::Texture& tex);

	};



	class CubeTexture {
	private:

		

		vector<string> faceFileLocations;

		GLuint texture;
		bool isLoad = false;
	public:

		GLint internalFormat = GL_RGBA; //RGBA32...
		GLenum format = GL_RGBA; //GL_RGBA...
		GLenum type = GL_UNSIGNED_BYTE; //GL_UNSIGNED_BYTE, GL_FLOAT...
		GLenum wrapS = GL_CLAMP_TO_EDGE; //default
		GLenum wrapT = GL_CLAMP_TO_EDGE; //default
		GLenum wrapR = GL_CLAMP_TO_EDGE; //default
		GLenum minFilter = GL_LINEAR; //default
		GLenum magFilter = GL_LINEAR; //default
		bool isGenerateMipmap = true; //default, if false no mipmap generated
		TextureType textureType = TextureType::TextureCube; //default

		MyVectorV3::Vector4f borderColor = MyVectorV3::Vector4f(1, 1, 1, 1);//if wrap glClampToBorder, then this color is used


		CubeTexture();
		CubeTexture(vector<string> faceFileLocs, GLint internalFormat = GL_RGBA, GLenum format = GL_RGBA,
			GLenum type = GL_UNSIGNED_BYTE, GLenum wrapS = GL_CLAMP_TO_EDGE, GLenum wrapT = GL_CLAMP_TO_EDGE, GLenum wrapR = GL_CLAMP_TO_EDGE,
			GLenum minFilter = GL_LINEAR, GLenum magFilter = GL_LINEAR, bool isGenerateMipmap = false);
		
		CubeTexture(const CubeTexture& other);
		CubeTexture(GLuint textureId);

		//order: right, left, top, bottom, back, front
		void LoadCubeTexture(vector<string> faceFileLocs); 

		bool isLoaded();
		void BindTexture(GLenum textureUnit);

		GLuint getTextureId();

		CubeTexture& operator=(const CubeTexture& other);

		
	};

}

#endif