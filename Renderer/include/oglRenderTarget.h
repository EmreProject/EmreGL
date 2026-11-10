#ifndef oglrendertarget_h
#define oglrendertarget_h




#include <GL/glew.h>
#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Projection.h"
#include "ogl_Camera.h"
#include "ogl_texture.h"
#include "oglWindow.h"

using namespace std;


namespace OglRenderTarget {


	struct Attachment {

		GLuint id;


		Attachment(){}
		virtual MyTextureV2::Texture getTexture()=0;
		virtual ~Attachment() = default;

	};

	struct DataTexture2DAttachment : public Attachment{
		
		GLint internalFormat= GL_RGBA32F; //RGBA32...
		GLenum format=GL_RGBA; //GL_RGBA...
		GLenum type=GL_FLOAT; //GL_UNSIGNED_BYTE, GL_FLOAT...
		GLint minFilter= GL_NEAREST; //for data storage use GL_NEAREST
		GLint magFilter= GL_NEAREST;//for data storage use GL_NEAREST
		GLint wrapS= GL_CLAMP_TO_EDGE;
		GLint wrapT= GL_CLAMP_TO_EDGE;
		GLenum colorAttachment;//GL_COLOR_ATTACHMENT0

		DataTexture2DAttachment() :Attachment() {}

		MyTextureV2::Texture getTexture();
	};


	struct ColorTexture2DAttachment : public Attachment {
		
		GLint internalFormat = GL_RGBA; //RGBA32...
		GLenum format = GL_RGBA; //GL_RGBA...
		GLenum type = GL_UNSIGNED_BYTE; //GL_UNSIGNED_BYTE, GL_FLOAT...
		GLint minFilter = GL_LINEAR; //for data storage use GL_NEAREST
		GLint magFilter = GL_LINEAR;//for data storage use GL_NEAREST
		GLint wrapS= GL_REPEAT; 
		GLint wrapT= GL_REPEAT;
		GLenum colorAttachment;//GL_COLOR_ATTACHMENT0...

		ColorTexture2DAttachment() :Attachment() {}

		MyTextureV2::Texture getTexture();
	};


	struct DepthTexture2DAttachment : public Attachment {
		


		GLuint width;
		GLuint height;
		GLint internalFormat = GL_DEPTH_COMPONENT24; //RGBA32...
		GLenum format = GL_DEPTH_COMPONENT; //GL_RGBA...
		GLenum type = GL_FLOAT; //GL_UNSIGNED_BYTE, GL_FLOAT...
		GLint minFilter = GL_LINEAR; //for data storage use GL_NEAREST
		GLint magFilter = GL_LINEAR;//for data storage use GL_NEAREST
		GLint wrapS= GL_CLAMP_TO_BORDER;
		GLint wrapT= GL_CLAMP_TO_BORDER;
		GLenum depthAttachment= GL_DEPTH_ATTACHMENT;//GL_COLOR_ATTACHMENT0

		DepthTexture2DAttachment(GLint width_, GLint height_) :Attachment() {
			this->width = width_;
			this->height = height_;
		}

		MyTextureV2::Texture getTexture();
	};


	struct RBOAttachment : public Attachment {
		
		GLuint width;
		GLuint height;
		GLenum internalformat= GL_DEPTH24_STENCIL8;
		GLenum depthAttachment= GL_DEPTH_STENCIL_ATTACHMENT;//GL_DEPTH_STENCIL_ATTACHMENT gibi
		
		RBOAttachment(GLint width_, GLint height_) :Attachment() {
			this->width = width_;
			this->height = height_;
		}

		MyTextureV2::Texture getTexture();
	};



	class RenderTarget {

	private:
		GLuint fbo;

		
		list<Attachment*> colorBuffers;
		Attachment* depthBuffer;


		void createFBO();
		GLuint createTexture2D(GLint width,GLint height,GLint internalFormat,GLenum format,GLenum type,
			GLint minFilter,GLint magFilter,GLint wrapS,GLint wrapT);
		GLuint createRbo(GLint width, GLint height, GLenum internalFormat);
		
		vector<GLenum> drawBuffers;
		function<void()> ClearRenderTarget=[this]() {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);

			glViewport(0, 0, width, height);
			glClearColor(0, 0, 0, 1.0f);
			//glEnable(GL_DEPTH_TEST);

			//Clear Buffers
			int colorBufferCount =this->textureCount();

			for (int i = 0; i < colorBufferCount; i++) {
				MyVectorV3::Vector4f clearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClearBufferfv(GL_COLOR, i, &clearColor.x);
			}
			float depthClear = 1.0f;
			glClearBufferfv(GL_DEPTH, 0, &depthClear);
			};	

		bool isCreated = false;

	public:

		int width;
		int height;




		RenderTarget(int width,int height);
		RenderTarget(const RenderTarget& rt);

		RenderTarget& operator=(RenderTarget rt);
		

		void SetDepthBuffer();
		void SetDepthBuffer(Attachment* attachment);
		void SetDepthBuffer(RenderTarget& rt);
		void AddColorBuffer(Attachment* attachment,int count=1);


		void SetClearFunction(function<void()> clearFunc);//optional if you want to set your own clear function
		void Clear();

		GLuint getFBO() { return fbo; }
		MyTextureV2::Texture getDepthTexture();
		MyTextureV2::Texture getColorTexture(int index);

		int textureCount() { return colorBuffers.size(); }
		bool checkStatus();
		

		void Create();
		


	};








}


#endif