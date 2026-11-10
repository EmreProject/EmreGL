#include "Shaders.h"

myShaderList AllShaders::shaderList;

string infoShader::info()
{
	ostringstream oss;

	oss << "id: " << mySha->shaderProgram<<endl
		<< "vs: " << mySha->vsText<<endl
		<< "fs: " << mySha->fsText << endl;

	return oss.str();

}
void myVertexSha::operator()(myShader* ms)
{

	myReadFile(loc, ms->vsText);

}
void myFragmentSha::operator()(myShader* ms)
{
	myReadFile(loc, ms->fsText);
}

void myGeometrySha::operator()(myShader* ms)
{
	myReadFile(loc, ms->gsText);
}



string* myShader::getVertexShaderText() {

	return &this->vsText;

}
string* myShader::getFragmentShaderText() {

	return &this->fsText;

}

string* myShader::getGeometryShaderText() {

	return &this->gsText;
}

void myShader::createProgram()
{

	glUseProgram(0);
	this->shaderProgram = glCreateProgram();



	vector<GLuint> shaderObjList;;
	auto addShader = [this,&shaderObjList](string shaderText, GLenum shaderType) {

		GLuint ShaderObj = glCreateShader(shaderType);

		if (shaderProgram == 0)
		{
			cout << "error creating shader!!" << endl;
			exit(1);

		}


		if (ShaderObj == 0)
		{
			cout << "error creating shader type " << shaderType << endl;
			exit(0);
		}


		const GLchar* p[1];
		p[0] = shaderText.c_str();

		GLint Lengths[1];
		Lengths[0] = strlen(shaderText.c_str());

		glShaderSource(ShaderObj, 1, p, Lengths);
		glCompileShader(ShaderObj);

		GLint success;
		glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			GLchar infoLog[1024];
			glGetShaderInfoLog(ShaderObj, 1024, NULL, infoLog);
			cout << "error compiling shader type " << shaderType << ": " << infoLog << endl;
			exit(1);
		}

		glAttachShader(shaderProgram, ShaderObj);
		shaderObjList.push_back(ShaderObj);

	};


	addShader(vsText, GL_VERTEX_SHADER);
	addShader(fsText, GL_FRAGMENT_SHADER);
	if (gsText != "empty") {
		addShader(gsText, GL_GEOMETRY_SHADER);
		cout << "no emptyyy gs" << endl;
	}
	else {
		cout << "emptyyy gs" << endl;
	}

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glLinkProgram(shaderProgram);

	for_each(shaderObjList.begin(), shaderObjList.end(), [](GLuint shaderObj) {
		glDeleteShader(shaderObj);
		});


	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}
	/*
	glValidateProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		
		glGetProgramInfoLog(shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}
	*/

	glUseProgram(shaderProgram);
	
}

bool myReadFile(string loc, string& copy)
{
	
	ifstream iff(loc);

	if (!iff)
	{
		cout << "file could not founded" << endl;
		return false;
	}

	ostringstream oss;
	string line;
	while (!iff.eof())
	{

		getline(iff, line, '\n');
		oss << line << endl;

	}

	iff.close();

	copy = oss.str();

	

	return true;
}
