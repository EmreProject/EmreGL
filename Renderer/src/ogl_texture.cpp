#include "ogl_texture.h"


namespace MyTextureV2 {



	int Texture::MaxTextureCount=16;
	bool Texture::IsMaxTextureCountSet=false;
	vector<int> Texture::AvailableTextureUnits;
	
	void Texture::ResetAvailableTextureUnits(int size) {

		if (AvailableTextureUnits.size() > 0) { //if there is already available texture units, then unbind them first
			int last = AvailableTextureUnits[0];
			for (int i = 0; i < last; i++) {
				glActiveTexture(GL_TEXTURE0 + i);              // make unit i current
				glBindTexture(GL_TEXTURE_2D, 0);      // bind (or leave 0)

			}
		}
		AvailableTextureUnits.erase(AvailableTextureUnits.begin(), AvailableTextureUnits.end()); //clear the vector
		AvailableTextureUnits.resize(size);

		int n = 0;
		generate(AvailableTextureUnits.begin(), AvailableTextureUnits.end(), [&n]() { return  n++; });


	}

	void Texture::SetMaxTextureCount() {

		GLint maxCombined = 0;                       // all stages together
		GLint maxFrag = 0;                       // fragment shader only
		GLint maxVertex = 0;                       // vertex  shader only

		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombined);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFrag);
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertex);

		GLint min = std::min(maxCombined, std::min(maxFrag, maxVertex));

		if (min > 0) {
			Texture::MaxTextureCount = min;
			Texture::ResetAvailableTextureUnits(Texture::MaxTextureCount);
			Texture::IsMaxTextureCountSet = true;
		}
		else {
			Texture::MaxTextureCount = 16; //default value
			Texture::ResetAvailableTextureUnits(Texture::MaxTextureCount);
			Texture::IsMaxTextureCountSet = true;
		}

	}

	int Texture::GetCurrentTextureUnitIndex() {


		int ret = Texture::AvailableTextureUnits[0];
		Texture::AvailableTextureUnits.erase(Texture::AvailableTextureUnits.begin()); //remove the first element
		return ret;


	}


	void Texture::LoadTexture(string fileLoc) {

		this->fileLocation = fileLoc;

		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

		// if wrapS or wrapT is GL_CLAMP_TO_BORDER, then set the border color
		
		if (this->wrapS == GL_CLAMP_TO_BORDER || this->wrapT == GL_CLAMP_TO_BORDER) {
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &this->borderColor.x);
		}

		// load and generate the texture
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(fileLocation.c_str(), &width, &height, &nrChannels, 4);
		if (data)
		{
			isLoad = true;
			//Texture::SetCurrentTextureUnitIndex(*this);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
			
			if (isGenerateMipmap) {
			glGenerateMipmap(GL_TEXTURE_2D);
			}
		}
		else
		{
			//std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);

	}

	Texture::Texture(GLuint textureId) {

		if (!Texture::IsMaxTextureCountSet) {

			Texture::SetMaxTextureCount();
		}

		this->texture = textureId;
	
		
		this->isLoad = true; 

	}

	Texture::Texture() {

		if (!Texture::IsMaxTextureCountSet) {

			Texture::SetMaxTextureCount();
		}


		isLoad = false;
	}
	Texture::Texture(string fileLoc,GLint internalFormat, GLenum format, GLenum type,
		GLenum wrapS,GLenum wrapT ,GLenum minFilter,GLenum magFilter,bool isGenerateMipmap) {

				if (!Texture::IsMaxTextureCountSet) {

					Texture::SetMaxTextureCount();
				}

				this->fileLocation = fileLoc;
				 this->internalFormat = internalFormat; //RGBA32...
				 this->format = format; //GL_RGBA...
				 this->type = type; //GL_UNSIGNED_BYTE, GL_FLOAT...
				 this->wrapS = wrapS; //default
				 this->wrapT = wrapT; //default
				 this->minFilter = minFilter; //default
				 this->magFilter = magFilter; //default
				 this->isGenerateMipmap = isGenerateMipmap; //default, if false no mipmap generated

				this->LoadTexture(fileLoc);
	}
	Texture::Texture(const MyTextureV2::Texture& tex) {

		if (!Texture::IsMaxTextureCountSet) {

			Texture::SetMaxTextureCount();
		}


		//this->LoadTexture(tex.fileLocation);

		this->fileLocation = tex.fileLocation;
		this->texture = tex.texture;
		this->internalFormat = tex.internalFormat;
		this->format = tex.format;
		this->type = tex.type;
		this->wrapS = tex.wrapS;
		this->wrapT = tex.wrapT;
		this->minFilter = tex.minFilter;
		this->magFilter = tex.magFilter;
		this->borderColor = tex.borderColor;
		this->isGenerateMipmap = tex.isGenerateMipmap;
		this->textureType = tex.textureType;
		
		this->isLoad = tex.isLoad;


	}


	Texture::~Texture() {


		//Texture::AvailableTextureUnits.insert(Texture::AvailableTextureUnits.end(),this->shaderValue); //add the texture unit back to the available texture units

	}
	

	void Texture::BindTexture(GLenum textureUnit) {

		glActiveTexture(textureUnit);//like GL_TEXTURE0, GL_TEXTURE1

		if(TextureType::Texture2D == this->textureType){
			glBindTexture(GL_TEXTURE_2D, this->texture);
		}
		else if(TextureType::Texture3D == this->textureType){
			glBindTexture(GL_TEXTURE_3D, this->texture);
		}
		
	}

	bool Texture::isLoaded() {

		return isLoad;

	}

	GLuint Texture::getTextureId(){

		return this->texture;


		}


	Texture& Texture::operator=(const MyTextureV2::Texture& tex) {

		if (!Texture::IsMaxTextureCountSet) {

			Texture::SetMaxTextureCount();
		}


		this->fileLocation = tex.fileLocation;
		this->texture = tex.texture;
		this->internalFormat = tex.internalFormat;
		this->format = tex.format;
		this->type = tex.type;
		this->wrapS = tex.wrapS;
		this->wrapT = tex.wrapT;
		this->minFilter = tex.minFilter;
		this->magFilter = tex.magFilter;
		this->borderColor = tex.borderColor;
		this->isGenerateMipmap = tex.isGenerateMipmap;
		this->textureType = tex.textureType;

		this->isLoad = tex.isLoad;

		

		return *this;
	}

	//CUBE TEXTURE MAP!!!

	CubeTexture::CubeTexture()  {

		if (!Texture::IsMaxTextureCountSet) {

			Texture::SetMaxTextureCount();
		}


		isLoad = false;
	}

	CubeTexture::CubeTexture(const CubeTexture& other) {

		if (!Texture::IsMaxTextureCountSet) {

			Texture::SetMaxTextureCount();
		}


		this->faceFileLocations = other.faceFileLocations;
		this->texture = other.texture;
		this->textureType = other.textureType;
		this->internalFormat = other.internalFormat;
		this->format = other.format;
		this->type = other.type;
		this->wrapS = other.wrapS;
		this->wrapT = other.wrapT;
		this->wrapR = other.wrapR;
		this->minFilter = other.minFilter;
		this->magFilter = other.magFilter;
		this->isGenerateMipmap = other.isGenerateMipmap;
		this->isLoad = other.isLoad;

	}
	CubeTexture::CubeTexture(GLuint textureId) {

		if (!Texture::IsMaxTextureCountSet) {

			Texture::SetMaxTextureCount();
		}

		this->isLoad = true;
		this->texture = textureId;
	}

	
	CubeTexture::CubeTexture(vector<string> faceFileLocs, GLint internalFormat, GLenum format,
		GLenum type, GLenum wrapS, GLenum wrapT, GLenum wrapR,
		GLenum minFilter, GLenum magFilter, bool isGenerateMipmap)  {
	
		if (!Texture::IsMaxTextureCountSet) {

			Texture::SetMaxTextureCount();
		}

		
		this->faceFileLocations = faceFileLocs;
		this->internalFormat = internalFormat; //RGBA32...
		this->format = format; //GL_RGBA...
		this->type = type; //GL_UNSIGNED_BYTE, GL_FLOAT...
		this->wrapS = wrapS; //default
		this->wrapT = wrapT; //default
		this->wrapR = wrapR; //default
		this->minFilter = minFilter; //default
		this->magFilter = magFilter; //default
		this->isGenerateMipmap = isGenerateMipmap; //default, if false no mipmap generated

		this->LoadCubeTexture(faceFileLocs);
	}



	void CubeTexture::LoadCubeTexture(vector<string> faceFileLocs) {

		this->faceFileLocations = faceFileLocs;
		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);

		// if wrapS or wrapT is GL_CLAMP_TO_BORDER, then set the border color

		if (this->wrapS == GL_CLAMP_TO_BORDER || this->wrapT == GL_CLAMP_TO_BORDER || this->wrapR == GL_CLAMP_TO_BORDER) {
			glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, &this->borderColor.x);
		}

		this->isLoad = true;

		int width, height, nrComponents;
		stbi_set_flip_vertically_on_load(false); // IMPORTANT for cubemap

		for (unsigned int i = 0; i < faceFileLocs.size(); i++)
		{
			unsigned char* data = stbi_load(faceFileLocs[i].c_str(), &width, &height, &nrComponents, 4);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faceFileLocs[i] << std::endl;
				stbi_image_free(data);
				this->isLoad = false;
			}
		}

	}

	bool CubeTexture::isLoaded() {

		return this->isLoad;
	}
	void CubeTexture::BindTexture(GLenum textureUnit) {

		glActiveTexture(textureUnit);//like GL_TEXTURE0, GL_TEXTURE1
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);
		

	}

	GLuint CubeTexture::getTextureId() {


		return this->texture;
	}

	CubeTexture& CubeTexture::operator=(const CubeTexture& other) {

		if (!Texture::IsMaxTextureCountSet) {

			Texture::SetMaxTextureCount();
		}


		this->faceFileLocations = other.faceFileLocations;
		this->texture = other.texture;
		this->internalFormat = other.internalFormat;
		this->format = other.format;
		this->type = other.type;
		this->wrapS = other.wrapS;
		this->wrapT = other.wrapT;
		this->wrapR = other.wrapR;
		this->minFilter = other.minFilter;
		this->magFilter = other.magFilter;
		this->isGenerateMipmap = other.isGenerateMipmap;
		this->isLoad = other.isLoad;
		this->textureType = other.textureType;
	
		return *this;
	}

}