#ifndef ogl_material_constants_h
#define ogl_material_constants_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <iostream>

using namespace std;

//https://threejs.org/docs/index.html#api/en/constants/Materials

namespace OglMaterialConstants {

	
	using BlendingMode = function<void()>;
	using DepthMode = function<void()>;


	//blending modes
	void NoBlending();//default render
	void NormalBlending();//default blending, Ordinary transparency/glass, depth sort needed(back to front)
	void AdditiveBlending();//Light sprites, fire, glows, no depth sort needed if deptwrite is false
	void SubtractiveBlending(); //Dark smoke, burn-away effects, usually no depth sort needed if depthwrite is false
	void MultiplyBlending();//Decals that tint/darken, ink on paper, usually no depth sort needed if depthwrite is false

	//for safety after opaque rendering, sort transparent objects back to front in each blending



	//depth mode
	void AlwaysDepthTest(); //always depth test, default�
	void NeverDepthTest(); //never depth test, useful for particles and transparent objects
	void LessDepthTest(); //less depth test, useful for particles and transparent objects
	void LessEqualDepthTest(); //less equal depth test, useful for particles and transparent objects
	void GreaterDepthTest(); //greater depth test, useful for particles and transparent objects
	void GreaterEqualDepthTest(); //greater equal depth test, useful for particles and transparent objects
	void EqualDepthTest(); //equal depth test, useful for particles and transparent objects
	void NotEqualDepthTest(); //not equal depth test, useful for particles and transparent objects



}


#endif