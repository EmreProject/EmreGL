#ifndef ogl_world_h
#define ogl_world_h


#include "ogl_Vector.h"
#include "ogl_Matrix.h"
#include "ogl_Projection.h"
#include "ogl_Camera.h"


namespace MyOglWorldTransform {



    class WorldTrans {
    public:
        WorldTrans() {}

        void SetScale(float scale);
        void SetScale(float scaleX,float scaleY,float scaleZ);
        void SetScale(const MyVectorV3::Vector3f&);
        void SetRotation(float x, float y, float z,string order="XYZ");
        void SetRotation(const MyVectorV3::Vector3f&, string order="XYZ");
        void SetRotation(const MyMatrixV3::Quaternion& q);
        void SetPosition(float x, float y, float z);
        void SetPosition(const MyVectorV3::Vector3f& WorldPos);

        void Rotate(float x, float y, float z, string order = "XYZ");

        MyMatrixV3::Matrix4f GetMatrix() ;

        MyVectorV3::Vector3f WorldPosToLocalPos(const MyVectorV3::Vector3f& WorldPos) ;
        MyVectorV3::Vector3f WorldDirToLocalDir(const MyVectorV3::Vector3f& WorldDir) ;

        MyMatrixV3::Matrix4f GetReversedTranslationMatrix() const;
        MyMatrixV3::Matrix4f GetReversedRotationMatrix() ;

        MyVectorV3::Vector3f GetScale() const { return m_scale; }
        MyVectorV3::Vector3f GetPos() const { return m_position; }
        MyMatrixV3::Quaternion GetRotation() const { return m_quaternion; }

        MyVectorV3::Vector3f* GetScalePointer() { return &m_scale; }
        MyVectorV3::Vector3f* GetTranslatePointer()  { return &m_position; }
        MyMatrixV3::Quaternion* GetRotationPointer()  { return &m_quaternion; }


    private:
        MyVectorV3::Vector3f m_scale = 1.0f;
        MyMatrixV3::Quaternion m_quaternion=MyMatrixV3::Quaternion(0,MyVectorV3::Vector3f(1,0,0));
        MyVectorV3::Vector3f m_position = MyVectorV3::Vector3f(0.0f, 0.0f, 0.0f);
    };




}

#endif