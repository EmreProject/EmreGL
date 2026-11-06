#ifndef ogl_matrix_h
#define ogl_matrix_h

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
#include <assimp/vector3.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <iostream>
#include "ogl_Vector.h"
#include "ogl_Projection.h"

using namespace std;

namespace MyMatrixV3 {

    class Matrix3f;
    class Matrix4f;
	class Quaternion;


	//ROTATION QUATERNION VECTOR MUST BE NORMALIZED
	//MAGNITUDE OF ROTATION QUATERNION IS ALWAYS 1
    struct Quaternion
    {
        float x, y, z, w;

        Quaternion() { this->SetFromAxisAngle(0, MyVectorV3::Vector3f(0, 1, 0)); }

		//Angle in degree
        Quaternion(float Angle, const MyVectorV3::Vector3f& V);

        Quaternion(float _x, float _y, float _z, float _w);

        void Normalize();

        Quaternion Conjugate() ;

        Quaternion Inverse();

		float Magnitude() ;

        MyVectorV3::Vector3f ToEulerAngles(string order = "XYZ");

        Matrix3f ToMatrix3();
		
        Matrix4f ToMatrix4();

		void SetFromRotationMatrix(const Matrix4f& M);

		// Angle in degree
		void SetFromAxisAngle(float Angle,const MyVectorV3::Vector3f& Axis);

		//Angle in degree
		void SetFromEuler(float x,float y, float z,string order="XYZ");
        
        // Note: the order of multiplication is important
		// this * right
        Quaternion Multiply(const Quaternion& r);

		// Note: the order of multiplication is important
        // left * this
		Quaternion Premultiply(const Quaternion& l);

		// Rotate vector v by this quaternion
		MyVectorV3::Vector3f Rotate(MyVectorV3::Vector3f v);


    };

	// Note: the order of multiplication is important
	// left * right   
	// first apply right rotation then left rotation
    Quaternion operator*(const Quaternion& l, const Quaternion& r);

    Quaternion operator*(const Quaternion& q, const MyVectorV3::Vector3f& v);



    class Matrix4f
    {
    public:
        float m[4][4];

        Matrix4f() {}

        Matrix4f(float* poiMatrix);

        Matrix4f(float a00, float a01, float a02, float a03,
            float a10, float a11, float a12, float a13,
            float a20, float a21, float a22, float a23,
            float a30, float a31, float a32, float a33)
        {
            m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03;
            m[1][0] = a10; m[1][1] = a11; m[1][2] = a12; m[1][3] = a13;
            m[2][0] = a20; m[2][1] = a21; m[2][2] = a22; m[2][3] = a23;
            m[3][0] = a30; m[3][1] = a31; m[3][2] = a32; m[3][3] = a33;
        }

        // constructor from Assimp matrix
        Matrix4f(const aiMatrix4x4& AssimpMatrix)
        {
            m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = AssimpMatrix.a4;
            m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = AssimpMatrix.b4;
            m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = AssimpMatrix.c4;
            m[3][0] = AssimpMatrix.d1; m[3][1] = AssimpMatrix.d2; m[3][2] = AssimpMatrix.d3; m[3][3] = AssimpMatrix.d4;
        }

        Matrix4f(const aiMatrix3x3& AssimpMatrix)
        {
            m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = 0.0f;
            m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = 0.0f;
            m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = 0.0f;
            m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
        }

        void SetZero()
        {
            float* poi = &this->m[0][0];
            for (int i = 0; i < (sizeof(m) / sizeof(m[0][0])); i++) {

                *(poi + i) = 0;

            }

        }

        Matrix4f Transpose() const
        {
            Matrix4f n;

            for (unsigned int i = 0; i < 4; i++) {
                for (unsigned int j = 0; j < 4; j++) {
                    n.m[i][j] = m[j][i];
                }
            }

            return n;
        }


        inline void InitIdentity()
        {
            m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
            m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
            m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
            m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
        }

        inline Matrix4f operator*(const Matrix4f& Right) const
        {
            Matrix4f Ret;

            for (unsigned int i = 0; i < 4; i++) {
                for (unsigned int j = 0; j < 4; j++) {
                    Ret.m[i][j] = m[i][0] * Right.m[0][j] +
                        m[i][1] * Right.m[1][j] +
                        m[i][2] * Right.m[2][j] +
                        m[i][3] * Right.m[3][j];
                }
            }

            return Ret;
        }

        MyVectorV3::Vector4f operator*(const MyVectorV3::Vector4f& v) const
        {
            MyVectorV3::Vector4f r;

            r.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
            r.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
            r.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
            r.w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;

            return r;
        }

        operator const float* () const
        {
            return &(m[0][0]);
        }

        void Print() const
        {
            for (int i = 0; i < 4; i++) {
                printf("%f %f %f %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
            }
        }


        float Determinant3x3() const;
        float Determinant() const;
		
        Matrix4f Inverse() const;

        void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
        void InitScaleTransform(float Scale);
        void InitScaleTransform(const MyVectorV3::Vector3f& Scale);

        void InitRotateTransform(float RotateX, float RotateY, float RotateZ);
        void InitRotateTransformZYX(float RotateX, float RotateY, float RotateZ);
        void InitRotateTransform(const MyVectorV3::Vector3f& Rotate);
        void InitRotateTransform(const Quaternion& quat);

        void InitTranslationTransform(float x, float y, float z);
        void InitTranslationTransform(const MyVectorV3::Vector3f& Pos);

        void InitCameraTransform(const MyVectorV3::Vector3f& Target, const MyVectorV3::Vector3f& Up);
        
        void InitCameraTransform(const MyVectorV3::Vector3f& Pos, const MyVectorV3::Vector3f& Target, const MyVectorV3::Vector3f& Up);

        void InitPersProjTransform(const MyProjection::PersProjInfo& p);

        void InitOrthoProjTransform(const MyProjection::OrthoProjInfo& p);

        void CalcClipPlanes(MyVectorV3::Vector4f& l, MyVectorV3::Vector4f& r, MyVectorV3::Vector4f& b, MyVectorV3::Vector4f& t, MyVectorV3::Vector4f& n, MyVectorV3::Vector4f& f) const;


        void Compose(const MyVectorV3::Vector3f& Position, const Quaternion& Rotate,
            const MyVectorV3::Vector3f& Scale);

		void Decompose(MyVectorV3::Vector3f& Position, Quaternion& Rotate, 
			MyVectorV3::Vector3f& Scale);

    private:
        void InitRotationX(float RotateX);
        void InitRotationY(float RotateY);
        void InitRotationZ(float RotateZ);
    };


    class Matrix3f
    {
    public:
        float m[3][3];

        Matrix3f() {}

        Matrix3f(float* poiMatrix);

        // Initialize the matrix from the top left corner of the 4-by-4 matrix
        Matrix3f(float a00, float a01, float a02,
            float a10, float a11, float a12,
            float a20, float a21, float a22);
        
        Matrix3f(const Matrix4f& a)
        {
            m[0][0] = a.m[0][0]; m[0][1] = a.m[0][1]; m[0][2] = a.m[0][2];
            m[1][0] = a.m[1][0]; m[1][1] = a.m[1][1]; m[1][2] = a.m[1][2];
            m[2][0] = a.m[2][0]; m[2][1] = a.m[2][1]; m[2][2] = a.m[2][2];
        }

        MyVectorV3::Vector3f operator*(const MyVectorV3::Vector3f& v) const
        {
            MyVectorV3::Vector3f r;

            r.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
            r.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
            r.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;

            return r;
        }

        inline Matrix3f operator*(const Matrix3f& Right) const
        {
            Matrix3f Ret;

            for (unsigned int i = 0; i < 3; i++) {
                for (unsigned int j = 0; j < 3; j++) {
                    Ret.m[i][j] = m[i][0] * Right.m[0][j] +
                        m[i][1] * Right.m[1][j] +
                        m[i][2] * Right.m[2][j];
                }
            }

            return Ret;
        }


        Matrix3f Inverse() const;

        Matrix3f Transpose() const
        {
            Matrix3f n;

            for (unsigned int i = 0; i < 3; i++) {
                for (unsigned int j = 0; j < 3; j++) {
                    n.m[i][j] = m[j][i];
                }
            }

            return n;
        }

        void InitRotateTransform(float RotateX, float RotateY, float RotateZ);
		void InitRotateTransform(const MyVectorV3::Vector3f& Rotate);
		void InitRotateTransform(const Quaternion& quat);

        void Print() const
        {
            for (int i = 0; i < 3; i++) {
                printf("%f %f %f\n", m[i][0], m[i][1], m[i][2]);
            }
        }

    private:
        void InitRotationX(float RotateX);
        void InitRotationY(float RotateY);
        void InitRotationZ(float RotateZ);
    };

   

    

}





#endif
