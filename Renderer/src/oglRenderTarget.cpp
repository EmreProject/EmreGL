#include "oglRenderTarget.h"

namespace OglRenderTarget {

	//Attachment methods

	MyTextureV2::Texture DataTexture2DAttachment::getTexture() {
		MyTextureV2::Texture texture(this->id);
		texture.internalFormat = this->internalFormat;
		texture.format = this->format;
		texture.type = this->type;
		texture.minFilter = this->minFilter;
		texture.magFilter = this->magFilter;
		texture.wrapS = this->wrapS;
		texture.wrapT = this->wrapT;
		texture.isGenerateMipmap = false; //for data storage use GL_NEAREST

		return texture;
	}

	MyTextureV2::Texture ColorTexture2DAttachment::getTexture() {
		MyTextureV2::Texture texture(this->id);
		texture.internalFormat = this->internalFormat;
		texture.format = this->format;
		texture.type = this->type;
		texture.minFilter = this->minFilter;
		texture.magFilter = this->magFilter;
		texture.wrapS = this->wrapS;
		texture.wrapT = this->wrapT;
		texture.isGenerateMipmap = false; //for data storage use GL_NEAREST

		return texture;
	}

	MyTextureV2::Texture DepthTexture2DAttachment::getTexture() {
		MyTextureV2::Texture texture(this->id);
		texture.internalFormat = this->internalFormat;
		texture.format = this->format;
		texture.type = this->type;
		texture.minFilter = this->minFilter;
		texture.magFilter = this->magFilter;
		texture.wrapS = this->wrapS;
		texture.wrapT = this->wrapT;
		texture.isGenerateMipmap = false; //for data storage use GL_NEAREST

		return texture;
	}

	MyTextureV2::Texture RBOAttachment::getTexture() {


		return MyTextureV2::Texture(this->id); //RBOAttachment does not have texture, it is just a renderbuffer object, 
		//just a filler method for virtual
	}

	//Rendertarget methods
	RenderTarget::RenderTarget(int width, int height) {

		this->width = width;
		this->height = height;
		this->depthBuffer = nullptr;


		createFBO();
		
		
	}
	
	RenderTarget::RenderTarget(const RenderTarget& rt) {


		colorBuffers.clear();
		drawBuffers.clear();
		depthBuffer = nullptr;


		this->width = rt.width;
		this->height = rt.height;
		this->isCreated = rt.isCreated;
		this->createFBO();


		auto it= rt.colorBuffers.begin();
		for(;it!= rt.colorBuffers.end();it++) {
		
			this->AddColorBuffer(*it);

		}

		if (rt.depthBuffer) {

		this->SetDepthBuffer(rt.depthBuffer);
		}
		else {
			//SetDepthBuffer();
		}

		if (rt.isCreated) {

		this->Create();
		}
	}

	RenderTarget& RenderTarget::operator=(RenderTarget rt) {


		if (this == &rt)
			return *this;

		// WARNING: ideally delete old GL resources here first.
		colorBuffers.clear();
		drawBuffers.clear();
		depthBuffer = nullptr;

		this->width = rt.width;
		this->height = rt.height;
		this->isCreated = rt.isCreated;
		this->createFBO();



		auto it = rt.colorBuffers.begin();
		for (; it != rt.colorBuffers.end(); it++) {

			this->AddColorBuffer(*it);

		}

		if (rt.depthBuffer!=nullptr) {
			this->SetDepthBuffer(rt.depthBuffer);
		}
		else {
			//this->SetDepthBuffer();
		}

		if (rt.isCreated) {

			this->Create();
		}

		return *this;

	}

	void RenderTarget::createFBO() {


		glGenFramebuffers(1, &fbo);
		cout << "created fbo: " << fbo << endl;
	
	}

	GLuint RenderTarget::createTexture2D(GLint width, GLint height, GLint internalFormat, GLenum format, GLenum type,
		GLint minFilter, GLint magFilter, GLint wrapS, GLint wrapT) {

		GLuint textureColorbuffer;
		glGenTextures(1, &textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

		if (wrapS == GL_CLAMP_TO_BORDER || wrapT == GL_CLAMP_TO_BORDER) {
			float border[4] = { 1.f, 1.f, 1.f, 1.f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		return textureColorbuffer;

	}

	GLuint RenderTarget::createRbo(GLint width, GLint height, GLenum internalFormat) {

		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		return rbo;

	}

	void RenderTarget::SetDepthBuffer() {


		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		

		GLint shadowWidth = this->width;
		GLint shadowHeight = this->height;

		RBOAttachment* depthBuffer_ = new RBOAttachment(shadowWidth, shadowHeight);
		depthBuffer_->id=this->createRbo(depthBuffer_->width, depthBuffer_->height, depthBuffer_->internalformat);
		this->depthBuffer = depthBuffer_;
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, depthBuffer_->depthAttachment, GL_RENDERBUFFER, depthBuffer_->id);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	}
	void RenderTarget::SetDepthBuffer(Attachment* attachment) {

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		
		DepthTexture2DAttachment* depthTexture2d_ = dynamic_cast<DepthTexture2DAttachment*>(attachment);
		RBOAttachment* depthRbo_ = dynamic_cast<RBOAttachment*>(attachment);

		
		if (depthTexture2d_!= nullptr) {
			
			DepthTexture2DAttachment* depthTexture2d = new DepthTexture2DAttachment(*depthTexture2d_);
			depthTexture2d->id = this->createTexture2D(depthTexture2d->width, depthTexture2d->height,
				depthTexture2d->internalFormat, depthTexture2d->format, depthTexture2d->type,
				depthTexture2d->minFilter, depthTexture2d->magFilter, depthTexture2d->wrapS, depthTexture2d->wrapT);
			glFramebufferTexture2D(GL_FRAMEBUFFER, depthTexture2d->depthAttachment, GL_TEXTURE_2D, depthTexture2d->id, 0);
			this->depthBuffer = depthTexture2d;
		}
		else if(depthRbo_!=nullptr) {
			
			RBOAttachment* depthRbo = new RBOAttachment(*depthRbo_);
			depthRbo->id = this->createRbo(depthRbo->width, depthRbo->height, depthRbo->internalformat);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, depthRbo->depthAttachment, GL_RENDERBUFFER, depthRbo->id);
			this->depthBuffer = depthRbo;
		}
		else {
			cout << "Invalid attachment type for depth buffer.\n" << endl;

		}

		

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	void RenderTarget::SetDepthBuffer(RenderTarget& rt) {

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		DepthTexture2DAttachment* depthTexture2d_ = dynamic_cast<DepthTexture2DAttachment*>(rt.depthBuffer);
		RBOAttachment* depthRbo_ = dynamic_cast<RBOAttachment*>(rt.depthBuffer);


		if(depthTexture2d_!=nullptr) {
			
			glFramebufferTexture2D(GL_FRAMEBUFFER, depthTexture2d_->depthAttachment, GL_TEXTURE_2D, depthTexture2d_->id, 0);
			this->depthBuffer = depthTexture2d_;
		}
		else if(depthRbo_!=nullptr) {
			
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, depthRbo_->depthAttachment, GL_RENDERBUFFER, depthRbo_->id);
			this->depthBuffer = depthRbo_;
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void RenderTarget::AddColorBuffer(Attachment* attachment, int count) {

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		ColorTexture2DAttachment* colorTexture2d = dynamic_cast<ColorTexture2DAttachment*>(attachment);
		DataTexture2DAttachment* dataTexture2d = dynamic_cast<DataTexture2DAttachment*>(attachment);
		
		auto createtexture2d = [this]<typename t>(t * attachment,int i) {
			attachment->id = createTexture2D(this->width, this->height,
				attachment->internalFormat, attachment->format, attachment->type,
				attachment->minFilter, attachment->magFilter, attachment->wrapS, attachment->wrapT);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, attachment->id, 0);
			drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		};


		int newCount = colorBuffers.size() + count;
		for (int i = colorBuffers.size(); i < newCount; i++) {

			if (colorTexture2d != nullptr) {

				ColorTexture2DAttachment* newColorTexture = new ColorTexture2DAttachment(*colorTexture2d);
				createtexture2d(newColorTexture, i);

				this->colorBuffers.insert(colorBuffers.end(), newColorTexture);
			}
			else if (dataTexture2d != nullptr) {
				DataTexture2DAttachment* newDataTexture = new DataTexture2DAttachment(*dataTexture2d);
				createtexture2d(newDataTexture, i);
				this->colorBuffers.insert(colorBuffers.end(), newDataTexture);
			}
			else {

				cout << "Invalid attachment type for color buffer.\n" << endl;
			}
			

		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


	}
	void RenderTarget::SetClearFunction(function<void()> clearFunc) {

		this->ClearRenderTarget = clearFunc;

	}

	void RenderTarget::Clear() {

		this->ClearRenderTarget();
	}

	bool RenderTarget::checkStatus() {

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		cout << "FBO incomplete!\n"<< endl;
		return false;
		}
		else {
			cout << "FBO complete!\n" << endl;
			return true;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);             
	}


	MyTextureV2::Texture RenderTarget::getDepthTexture() {
		if (depthBuffer == nullptr) {
			cout << "Depth buffer is not set.\n" << endl;
			return -1;
		}
		return MyTextureV2::Texture(depthBuffer->id);
	
	}
	MyTextureV2::Texture RenderTarget::getColorTexture(int index) {

		if (index >= colorBuffers.size()) {
			cout << "Color buffer index out of range.\n" << endl;
			#define assertm(exp, msg) assert((void(msg), exp))
			assertm(false, "Failed to getColorTexture from RenderTarget. Out of index(oglRenderTarget.cpp)"); // assertion fails
		}


		auto iter = colorBuffers.begin();
	

		for (int i = 0; i < index;i++) {
			iter++;

		}
		
		return MyTextureV2::Texture((*iter)->id);
	
	}


	void RenderTarget::Create() {

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glDrawBuffers(drawBuffers.size(), drawBuffers.data());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		this->isCreated = true;

	}

}