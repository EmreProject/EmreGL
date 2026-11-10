#ifndef OGL_AXIS_DEBUG_H
#define OGL_AXIS_DEBUG_H

#include "ogl_Matrix.h"
#include "ogl_Vector.h"
#include "oglMeshes.h"


using namespace std;


namespace OglAxisDebug {

	class AxesHelper {

	private:


	


		float size = 1.0f;
		MyVectorV3::Vector3f position = MyVectorV3::Vector3f(0, 0, 0);
		void setPosition(float x, float y, float z);
		void createAxis(float size);

	public:

		OglObject3D::Mesh x_axis;
		OglObject3D::Mesh y_axis;
		OglObject3D::Mesh z_axis;

		AxesHelper();
		AxesHelper(float size);

		void RotateOnAxis(float x, float y, float z, string order = "XYZ");
		void RotateOnAxis(float angle, const MyVectorV3::Vector3f& axis); //angle in degree
		void RotateOnAxis(const MyMatrixV3::Matrix4f& rotation);
		void RotateOnAxis(const MyMatrixV3::Quaternion& q);
		void RotateOnWorldAxis(float x, float y, float z, string order = "XYZ");
		void RotateOnWorldAxis(float angle, const MyVectorV3::Vector3f& axis); //angle in degree
		void RotateOnWorldAxis(const MyMatrixV3::Matrix4f& rotation);
		void RotateOnWorldAxis(const MyMatrixV3::Quaternion& q);

		void SetRotation(float x, float y, float z, string order = "XYZ");
		void SetRotation(float angle, const MyVectorV3::Vector3f& axis); //angle in degree
		void SetRotation(const MyMatrixV3::Matrix4f& rotation);
		void SetRotation(const MyMatrixV3::Quaternion& q);
		void SetPosition(float x, float y, float z);
		void SetPosition(const MyVectorV3::Vector3f& pos);

		void SetSize(float size);

		
	};


}


#endif