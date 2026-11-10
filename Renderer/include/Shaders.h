#ifndef myShader_h
#define myShader_h

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <list>
#include <functional>
#include <algorithm>
#include <GL/glew.h>

using namespace std;

bool myReadFile(string loc, string& copy);

//extra
template<typename t, typename name>
class listWithName
{
private:

	list<t> liste;
	list<name> names;



public:

	void addItem(t item, name nam)
	{
		liste.insert(liste.end(), item);
		names.push_back(nam);
	}
	template<typename... args>
	void emplaceItem(name nam,args... arg)
	{
		liste.emplace_back(arg...);
		names.push_back(nam);
	}

	t& operator[](name nam)
	{
		t* item1 = nullptr;

		transform(liste.begin(), liste.end(), names.begin(), names.begin(), [&item1, &nam](t& item2, name na) {

			if (na == nam)
			{
				item1 = &item2;
			}
			return na;
			});

			return *item1;
	}

};

class myShader;
typedef listWithName<myShader, string> myShaderList;

class AllShaders
{

public:

	static myShaderList shaderList;

};



class infoShader
{
private:
	
public:
	myShader* mySha;
	infoShader(myShader* mys):mySha(mys){}
	string info();
};
//extrabitti




//MAIN ---------------------------------------------------------

class myShader :public infoShader
{
private:

	unsigned int shaderProgram;
	string vsText = "empty";
	string fsText="empty";
	string gsText = "empty";

public:

	myShader() :infoShader(this) { infoShader::mySha = this; }
	template<typename t, typename... args>
	myShader(t vert1, args... verts) : infoShader(this)
	{
		vert1(this);
		addShader(verts...);
		infoShader::mySha = this;

	}
	myShader(const myShader& mySha) :infoShader(this)
	{
		this->shaderProgram = mySha.shaderProgram;
		vsText = mySha.vsText;
		fsText = mySha.fsText;
		gsText = mySha.gsText;

		infoShader::mySha = this;
	}

	myShader& operator=(const myShader& mySha)
	{
		if (this != &mySha) {
			this->shaderProgram = mySha.shaderProgram;
			vsText = mySha.vsText;
			fsText = mySha.fsText;
			gsText = mySha.gsText;

			infoShader::mySha = this;
		}
		return *this;
	}

	void addShader() {}
	template<typename t, typename... args>
	void addShader(t func, args... arg)
	{
		func(this);
		addShader(arg...);

	}

	unsigned int getShaderAddress(){return shaderProgram;}
	void createProgram();
	void Use() { glUseProgram(shaderProgram); }


	string* getVertexShaderText();
	string* getFragmentShaderText();
	string* getGeometryShaderText();

	friend class myVertexSha;
	friend class myFragmentSha;
	friend class myGeometrySha;
	friend class infoShader;
};

class myVertexSha
{
public:
	string loc;
	myVertexSha(string loc1):loc(loc1){}
	void operator()(myShader*);
};
class myFragmentSha
{
public:

	string loc;
	myFragmentSha(string loc1) :loc(loc1) {}
	void operator()(myShader*);
};

class myGeometrySha
{
public:

	string loc;
	myGeometrySha(string loc1) :loc(loc1) {}
	void operator()(myShader*);
};





#endif
