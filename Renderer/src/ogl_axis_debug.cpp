#include "ogl_axis_debug.h"

namespace OglAxisDebug {


	AxesHelper::AxesHelper() {

		this->size = 1.0f;
		this->createAxis(this->size);
		this->setPosition(0, 0, 0);

	}
	AxesHelper::AxesHelper(float size) {

		this->size = size;
		this->createAxis(this->size);
		this->setPosition(0, 0, 0);

	}


	void AxesHelper::createAxis(float size) {


		OglObject3D::BoxGeometry x_axis_geometry(size, 0.01, 0.01, 1, 1, 1);
		OglObject3D::BoxGeometry y_axis_geometry(0.01, size, 0.01, 1, 1, 1);
		OglObject3D::BoxGeometry z_axis_geometry(0.01,0.01, size, 1, 1, 1);

		OglObject3D::MeshBasicMaterial material;
		

		this->x_axis.geometry = x_axis_geometry.clone();
		this->y_axis.geometry = y_axis_geometry.clone();
		this->z_axis.geometry = z_axis_geometry.clone();

		material.setColor(1, 0, 0, 1);
		this->x_axis.material = material.clone();

		material.setColor(0, 1, 0, 1);
		this->y_axis.material = material.clone();

		material.setColor(0, 0, 1, 1);
		this->z_axis.material = material.clone();
		
	}
	void AxesHelper::setPosition(float x, float y, float z) {

		MyVectorV3::Vector3f x_axis_offset = MyVectorV3::Vector3f(this->size / 2.0f, 0, 0);
		MyVectorV3::Vector3f y_axis_offset = MyVectorV3::Vector3f(0, this->size / 2.0f, 0);
		MyVectorV3::Vector3f z_axis_offset = MyVectorV3::Vector3f(0, 0, this->size / 2.0f);

		MyMatrixV3::Quaternion rotation;
		rotation = y_axis.rotation;

		x_axis_offset = rotation.Rotate(x_axis_offset);
		y_axis_offset = rotation.Rotate(y_axis_offset);
		z_axis_offset = rotation.Rotate(z_axis_offset);

		this->position = MyVectorV3::Vector3f(x, y, z);
		x_axis.position = MyVectorV3::Vector3f(x, y, z) + x_axis_offset;
		y_axis.position = MyVectorV3::Vector3f(x, y , z) + y_axis_offset;
		z_axis.position = MyVectorV3::Vector3f(x, y, z) + z_axis_offset;
		
	}

	void AxesHelper::RotateOnAxis(float x, float y, float z, string order ){
		this->x_axis.RotateOnAxis(x, y, z, order);
		this->y_axis.RotateOnAxis(x, y, z, order);
		this->z_axis.RotateOnAxis(x, y, z, order);
		setPosition(position.x, position.y, position.z);
	}
	void AxesHelper::RotateOnAxis(float angle, const MyVectorV3::Vector3f& axis){
		this->x_axis.RotateOnAxis(angle, axis);
		this->y_axis.RotateOnAxis(angle, axis);
		this->z_axis.RotateOnAxis(angle, axis);
		setPosition(position.x, position.y, position.z);
	}
	void AxesHelper::RotateOnAxis(const MyMatrixV3::Matrix4f& rotation){
		this->x_axis.RotateOnAxis(rotation);
		this->y_axis.RotateOnAxis(rotation);
		this->z_axis.RotateOnAxis(rotation);
		setPosition(position.x, position.y, position.z);
	}
	void AxesHelper::RotateOnAxis(const MyMatrixV3::Quaternion& q){
		this->x_axis.RotateOnAxis(q);
		this->y_axis.RotateOnAxis(q);
		this->z_axis.RotateOnAxis(q);
		setPosition(position.x, position.y, position.z);
	}
	void AxesHelper::RotateOnWorldAxis(float x, float y, float z, string order){
		this->x_axis.RotateOnWorldAxis(x, y, z, order);
		this->y_axis.RotateOnWorldAxis(x, y, z, order);
		this->z_axis.RotateOnWorldAxis(x, y, z, order);
		setPosition(position.x, position.y, position.z);
	}
	void AxesHelper::RotateOnWorldAxis(float angle, const MyVectorV3::Vector3f& axis){
		this->x_axis.RotateOnWorldAxis(angle, axis);
		this->y_axis.RotateOnWorldAxis(angle, axis);
		this->z_axis.RotateOnWorldAxis(angle, axis);
		setPosition(position.x, position.y, position.z);
	}
	void AxesHelper::RotateOnWorldAxis(const MyMatrixV3::Matrix4f& rotation){
		this->x_axis.RotateOnWorldAxis(rotation);
		this->y_axis.RotateOnWorldAxis(rotation);
		this->z_axis.RotateOnWorldAxis(rotation);
		setPosition(position.x, position.y, position.z);
	}
	void AxesHelper::RotateOnWorldAxis(const MyMatrixV3::Quaternion& q){
		this->x_axis.RotateOnWorldAxis(q);
		this->y_axis.RotateOnWorldAxis(q);
		this->z_axis.RotateOnWorldAxis(q);
		setPosition(position.x, position.y, position.z);
	}
	void AxesHelper::SetPosition(float x, float y, float z){
	
		this->setPosition(x, y, z);
	}
	void AxesHelper::SetPosition(const MyVectorV3::Vector3f& pos){
		this->setPosition(pos.x, pos.y, pos.z);
	}

	void AxesHelper::SetRotation(float x, float y, float z, string order ) {

		this->x_axis.rotation.SetFromEuler(x, y, z, order);
		this->y_axis.rotation.SetFromEuler(x, y, z, order);
		this->z_axis.rotation.SetFromEuler(x, y, z, order);
		setPosition(position.x, position.y, position.z);
	}
	void AxesHelper::SetRotation(float angle, const MyVectorV3::Vector3f& axis) {

		this->x_axis.rotation.SetFromAxisAngle(angle, axis);
		this->y_axis.rotation.SetFromAxisAngle(angle, axis);
		this->z_axis.rotation.SetFromAxisAngle(angle, axis);
		setPosition(position.x, position.y, position.z);
	}
	void AxesHelper::SetRotation(const MyMatrixV3::Matrix4f& rotation) {

		this->x_axis.rotation.SetFromRotationMatrix(rotation);
		this->y_axis.rotation.SetFromRotationMatrix(rotation);
		this->z_axis.rotation.SetFromRotationMatrix(rotation);
		setPosition(position.x, position.y, position.z);
	}

	void AxesHelper::SetRotation(const MyMatrixV3::Quaternion& q) {

		this->x_axis.rotation = q;
		this->y_axis.rotation = q;
		this->z_axis.rotation = q;
		setPosition(position.x, position.y, position.z);

	}
	void AxesHelper::SetSize(float size) {

		
		float scaleFactor = size / this->size;
		this->size = size;
		this->x_axis.scale.x  *= scaleFactor;
		this->y_axis.scale.y *= scaleFactor;
		this->z_axis.scale.z *= scaleFactor;

		this->setPosition(position.x, position.y, position.z);
	}


}
