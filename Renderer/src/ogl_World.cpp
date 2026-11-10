#include "ogl_World.h"


namespace MyOglWorldTransform {




void WorldTrans::SetScale(float scale)
{
    m_scale = MyVectorV3::Vector3f(scale,scale,scale);
}


void WorldTrans::SetScale(float scaleX, float scaleY, float scaleZ) {

    m_scale = MyVectorV3::Vector3f(scaleX, scaleY, scaleZ);
}


void WorldTrans::SetScale(const MyVectorV3::Vector3f& scale){

	m_scale = scale;
}


void WorldTrans::SetRotation(float x, float y, float z, string order)
{
	this->m_quaternion.SetFromEuler(x, y, z, order);
}

void WorldTrans::SetRotation(const MyVectorV3::Vector3f& rot1, string order) {

    this->m_quaternion.SetFromEuler(rot1.x,rot1.y,rot1.z, order);


}

void WorldTrans::SetRotation(const MyMatrixV3::Quaternion& q) {

	this->m_quaternion = q;
}
void WorldTrans::SetPosition(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
}


void WorldTrans::SetPosition(const MyVectorV3::Vector3f& WorldPos)
{
    m_position = WorldPos;
}


void WorldTrans::Rotate(float x, float y, float z,string order)
{
	MyMatrixV3::Quaternion q;
	q.SetFromEuler(x, y, z, order);

	this->m_quaternion = this->m_quaternion.Premultiply(q);
}


MyMatrixV3::Matrix4f WorldTrans::GetMatrix() 
{
    MyMatrixV3::Matrix4f Scale;
    Scale.InitScaleTransform(m_scale.x, m_scale.y, m_scale.z);

	MyMatrixV3::Matrix4f Rotation = m_quaternion.ToMatrix4();

    MyMatrixV3::Matrix4f Translation;
    Translation.InitTranslationTransform(m_position.x, m_position.y, m_position.z);

    MyMatrixV3::Matrix4f WorldTransformation = Translation * Rotation * Scale;

    return WorldTransformation;
}


MyMatrixV3::Matrix4f WorldTrans::GetReversedTranslationMatrix() const
{
    MyMatrixV3::Matrix4f ReversedTranslation;
    ReversedTranslation.InitTranslationTransform(m_position.Negate());
    return ReversedTranslation;
}


MyMatrixV3::Matrix4f WorldTrans::GetReversedRotationMatrix() 
{

	MyMatrixV3::Quaternion InverseQuaternion = m_quaternion.Inverse();
	MyMatrixV3::Matrix4f ReversedRotation = InverseQuaternion.ToMatrix4();
   
    return ReversedRotation;
}


MyVectorV3::Vector3f WorldTrans::WorldPosToLocalPos(const MyVectorV3::Vector3f& WorldPos) 
{
  
    MyVectorV3::Vector4f WorldPos4f = MyVectorV3::Vector4f(WorldPos, 1.0f);
    MyMatrixV3::Matrix4f inverseMatrix = this->GetMatrix();
    inverseMatrix=inverseMatrix.Inverse();
    MyVectorV3:: Vector4f LocalPos4f = inverseMatrix * WorldPos4f;
    MyVectorV3::Vector3f LocalPos3f(LocalPos4f);
    return LocalPos3f;
}


MyVectorV3::Vector3f WorldTrans::WorldDirToLocalDir(const MyVectorV3::Vector3f& WorldDirection) 
{
   // MyMatrixV3::Matrix3f inverseMatrix = MyMatrixV3::Matrix3f(this->GetMatrix().Inverse());
   
    
   
    
	MyMatrixV3::Quaternion InverseQuaternion = m_quaternion.Inverse();
	MyVectorV3::Vector3f LocalDirection = InverseQuaternion.Rotate(WorldDirection);

    LocalDirection = LocalDirection.Normalize();
    
  
    return LocalDirection;
}










}