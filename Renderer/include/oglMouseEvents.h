#ifndef ogl_mouse_events_h
#define ogl_mouse_events_h


#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Projection.h"
#include "ogl_Camera.h"
#include "ogl_lightning_v2.h"
#include "ogl_model_v2.h"
#include "ogl_shader_v2.h"
#include "ogl_texture.h"
#include "ogl_World.h"
#include "ogl_default_meshes.h"
#include "oglScene.h"
#include "oglShadowMap.h"
#include "oglRenderTarget.h"
#include <type_traits>
#include <variant>

namespace OglMouseEvents {


	class MouseEventHandler {


	private:

		struct MouseEventMeshData {

			OglObject3D::Mesh* originalMesh;
			OglObject3D::Mesh* mouseEventMesh;
			float id;
			
		};

		OglRenderTarget::RenderTarget mouseRenderTarget;
		OglRenderTarget::DataTexture2DAttachment objectIdTexture;
		void createRenderTarget();
		void setMouseEvents();

		bool isSceneSet = false;
		OglScene::Scene mouseEventScene; 
		list<MouseEventMeshData> mouseEventMeshDataList;
		
		float oldMouseTouchingObjectId = -1;
		float currentMouseTouchingObjectId = -1;
		float clickedObjectId = -1;

		MouseEventMeshData createMouseEventMesh(OglObject3D::Mesh& mesh,float id);


	public:

		MouseEventHandler();
		~MouseEventHandler();

		void setMouseEventScene(OglScene::Scene* scene_);
		bool isMouseEventSceneSet();

		OglScene::Scene& getScene();

		OglRenderTarget::RenderTarget& getRenderTarget();
		


		MyTextureV2::Texture getTexture();//for debug purpose
		void Update();


	};

};


#endif