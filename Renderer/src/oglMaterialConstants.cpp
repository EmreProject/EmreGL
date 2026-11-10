#include "oglMaterialConstants.h"

namespace OglMaterialConstants
{
	void NoBlending() {

		glDisable(GL_BLEND);

	}
	void NormalBlending(){

		glEnable(GL_BLEND);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		}
	void AdditiveBlending(){


		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		}
	void SubtractiveBlending(){

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);

		}
	void MultiplyBlending(){

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);

		}



	//depth modes

	void AlwaysDepthTest(){

		glDepthFunc(GL_ALWAYS);

	}
	void NeverDepthTest(){

		glDepthFunc(GL_NEVER);
	}
	void LessDepthTest(){

		glDepthFunc(GL_LESS);

	}
	void LessEqualDepthTest(){

		glDepthFunc(GL_LEQUAL);

	}
	void GreaterDepthTest(){

		glDepthFunc(GL_GREATER);

	}
	void GreaterEqualDepthTest(){

		glDepthFunc(GL_GEQUAL);

	}
	void EqualDepthTest(){

		glDepthFunc(GL_EQUAL);

	}
	void NotEqualDepthTest(){

		glDepthFunc(GL_NOTEQUAL);

	}




}