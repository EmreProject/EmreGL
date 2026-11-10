#include "ogl_Matrix.h"


namespace MyMatrixV3 {
    
    //Matrix3f


    Matrix3f Matrix3f::Inverse() const{

        // For:
// [ a b c ]
// [ d e f ]
// [ g h i ]

        const float a = m[0][0], b = m[0][1], c = m[0][2];
        const float d = m[1][0], e = m[1][1], f = m[1][2];
        const float g = m[2][0], h = m[2][1], i = m[2][2];

        const float det =
            a * (e * i - f * h)
            - b * (d * i - f * g)
            + c * (d * h - e * g);

        // Handle singular matrix
        // (You can also return Identity instead, but I'm matching your Matrix4f behavior.)
        if (fabs(det) < 1e-8f) {
            return *this;
        }

        const float invDet = 1.0f / det;

        Matrix3f res;

        // Adjugate (transpose of cofactor matrix) * (1/det)
        res.m[0][0] = (e * i - f * h) * invDet;
        res.m[0][1] = (c * h - b * i) * invDet;
        res.m[0][2] = (b * f - c * e) * invDet;

        res.m[1][0] = (f * g - d * i) * invDet;
        res.m[1][1] = (a * i - c * g) * invDet;
        res.m[1][2] = (c * d - a * f) * invDet;

        res.m[2][0] = (d * h - e * g) * invDet;
        res.m[2][1] = (b * g - a * h) * invDet;
        res.m[2][2] = (a * e - b * d) * invDet;

        return res;


    }
    
    
    
    //Matrix4f
   

    Matrix4f::Matrix4f(float* poiMatrix) {

        float* poi = (float*)(&m[0][0]);

        for (int i = 0; i < 16; i++) {

            *(poi + i) = *(poiMatrix + i);
          }

    }

    void Matrix4f::InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ)
    {
        m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
        m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
        m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
        m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;
    }


    void Matrix4f::InitScaleTransform(float Scale)
    {
        InitScaleTransform(Scale, Scale, Scale);
    }


    void Matrix4f::InitScaleTransform(const MyVectorV3::Vector3f& Scale)
    {
        InitScaleTransform(Scale.x, Scale.y, Scale.z);
    }


    void Matrix4f::InitRotateTransform(float RotateX, float RotateY, float RotateZ)
    {
        Matrix4f rx, ry, rz;

        float x = ToRadian(RotateX);
        float y = ToRadian(RotateY);
        float z = ToRadian(RotateZ);

        rx.InitRotationX(x);
        ry.InitRotationY(y);
        rz.InitRotationZ(z);

        *this = rz * ry * rx;
    }


    void Matrix4f::InitRotateTransformZYX(float RotateX, float RotateY, float RotateZ)
    {
        Matrix4f rx, ry, rz;

        float x = ToRadian(RotateX);
        float y = ToRadian(RotateY);
        float z = ToRadian(RotateZ);

        rx.InitRotationX(x);
        ry.InitRotationY(y);
        rz.InitRotationZ(z);

        *this = rx * ry * rz;
    }


    void Matrix4f::InitRotateTransform(const MyVectorV3::Vector3f& Rotate)
    {
        InitRotateTransform(Rotate.x, Rotate.y, Rotate.z);
    }


    //
    // The following rotation matrices are for a left handed coordinate system.
    // https://butterflyofdream.wordpress.com/2016/07/05/converting-rotation-matrices-of-left-handed-coordinate-system/
    //

    void Matrix4f::InitRotationX(float x)
    {
        m[0][0] = 1.0f; m[0][1] = 0.0f;  m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = cosf(x);  m[1][2] = sinf(x);  m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = -sinf(x); m[2][2] = cosf(x); m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f;  m[3][2] = 0.0f; m[3][3] = 1.0f;
    }


    void Matrix4f::InitRotationY(float y)
    {
        m[0][0] = cosf(y); m[0][1] = 0.0f; m[0][2] = -sinf(y); m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = sinf(y); m[2][1] = 0.0f; m[2][2] = cosf(y); m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    }


    void Matrix4f::InitRotationZ(float z)
    {
        m[0][0] = cosf(z);  m[0][1] = sinf(z);  m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = -sinf(z); m[1][1] = cosf(z); m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f;  m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
        m[3][0] = 0.0f;  m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    }

    void Matrix4f::InitRotateTransform(const Quaternion& quat)
    {
        float yy2 = 2.0f * quat.y * quat.y;
        float xy2 = 2.0f * quat.x * quat.y;
        float xz2 = 2.0f * quat.x * quat.z;
        float yz2 = 2.0f * quat.y * quat.z;
        float zz2 = 2.0f * quat.z * quat.z;
        float wz2 = 2.0f * quat.w * quat.z;
        float wy2 = 2.0f * quat.w * quat.y;
        float wx2 = 2.0f * quat.w * quat.x;
        float xx2 = 2.0f * quat.x * quat.x;
        m[0][0] = -yy2 - zz2 + 1.0f;
        m[0][1] = xy2 - wz2;
        m[0][2] = xz2 + wy2;
        m[0][3] = 0;
        m[1][0] = xy2 + wz2;
        m[1][1] = -xx2 - zz2 + 1.0f;
        m[1][2] = yz2 - wx2;
        m[1][3] = 0;
        m[2][0] = xz2 - wy2;
        m[2][1] = yz2 + wx2;
        m[2][2] = -xx2 - yy2 + 1.0f;
        m[2][3] = 0.0f;
        m[3][0] = m[3][1] = m[3][2] = 0;
        m[3][3] = 1.0f;
    }


    void Matrix4f::InitTranslationTransform(float x, float y, float z)
    {
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    }


    void Matrix4f::InitTranslationTransform(const MyVectorV3::Vector3f& Pos)
    {
        InitTranslationTransform(Pos.x, Pos.y, Pos.z);
    }


    void Matrix4f::InitCameraTransform(const MyVectorV3::Vector3f& Target, const MyVectorV3::Vector3f& Up)
    {
        MyVectorV3::Vector3f N = Target;
        N.Normalize();

        MyVectorV3::Vector3f UpNorm = Up;
        UpNorm.Normalize();

        MyVectorV3::Vector3f U;
        U = UpNorm.Cross(N);
        U.Normalize();

        MyVectorV3::Vector3f V = N.Cross(U);

        m[0][0] = U.x;   m[0][1] = U.y;   m[0][2] = U.z;   m[0][3] = 0.0f;
        m[1][0] = V.x;   m[1][1] = V.y;   m[1][2] = V.z;   m[1][3] = 0.0f;
        m[2][0] = N.x;   m[2][1] = N.y;   m[2][2] = N.z;   m[2][3] = 0.0f;
        m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;
    }


    void Matrix4f::InitCameraTransform(const MyVectorV3::Vector3f& Pos, const MyVectorV3::Vector3f& Target, const MyVectorV3::Vector3f& Up)
    {
        Matrix4f CameraTranslation;
        CameraTranslation.InitTranslationTransform(-Pos.x, -Pos.y, -Pos.z);

        Matrix4f CameraRotateTrans;
        CameraRotateTrans.InitCameraTransform(Target, Up);

        *this = CameraRotateTrans * CameraTranslation;
    }

    void Matrix4f::InitPersProjTransform(const MyProjection::PersProjInfo& p)
    {
        float ar = p.Height / p.Width;
        float zRange = p.zNear - p.zFar;
        float tanHalfFOV = tanf(ToRadian(p.FOV / 2.0f));

        m[0][0] = 1 / tanHalfFOV; m[0][1] = 0.0f;                 m[0][2] = 0.0f;                        m[0][3] = 0.0;
        m[1][0] = 0.0f;         m[1][1] = 1.0f / (tanHalfFOV * ar); m[1][2] = 0.0f;                        m[1][3] = 0.0;
        m[2][0] = 0.0f;         m[2][1] = 0.0f;                 m[2][2] = (-p.zNear - p.zFar) / zRange; m[2][3] = 2.0f * p.zFar * p.zNear / zRange;
        m[3][0] = 0.0f;         m[3][1] = 0.0f;                 m[3][2] = 1.0f;                        m[3][3] = 0.0;

#ifdef USE_GLM
        glm::mat4 Projection = glm::perspectiveFovLH(glm::radians(p.FOV), p.Width, p.Height, p.zNear, p.zFar);

        m[0][0] = Projection[0][0]; m[0][1] = Projection[1][0]; m[0][2] = Projection[2][0]; m[0][3] = Projection[3][0];
        m[1][0] = Projection[0][1]; m[1][1] = Projection[1][1]; m[1][2] = Projection[2][1]; m[1][3] = Projection[3][1];
        m[2][0] = Projection[0][2]; m[2][1] = Projection[1][2]; m[2][2] = Projection[2][2]; m[2][3] = Projection[3][2];
        m[3][0] = Projection[0][3]; m[3][1] = Projection[1][3]; m[3][2] = Projection[2][3]; m[3][3] = Projection[3][3];
#endif

        //    std::cout << glm::to_string(Projection) << std::endl;
    }


    void Matrix4f::InitOrthoProjTransform(const MyProjection::OrthoProjInfo& p)
    {
        float l = p.l;
        float r = p.r;
        float b = p.b;
        float t = p.t;
        float n = p.n;
        float f = p.f;

        m[0][0] = 2.0f / (r - l); m[0][1] = 0.0f;         m[0][2] = 0.0f;         m[0][3] = -(r + l) / (r - l);
        m[1][0] = 0.0f;         m[1][1] = 2.0f / (t - b); m[1][2] = 0.0f;         m[1][3] = -(t + b) / (t - b);
        m[2][0] = 0.0f;         m[2][1] = 0.0f;         m[2][2] = 2.0f / (f - n); m[2][3] = -(f + n) / (f - n);
        m[3][0] = 0.0f;         m[3][1] = 0.0f;         m[3][2] = 0.0f;         m[3][3] = 1.0;
    }
    
    float Matrix4f::Determinant3x3() const {


        const float a = m[0][0], b = m[0][1], c = m[0][2];
        const float d = m[1][0], e = m[1][1], f = m[1][2];
        const float g = m[2][0], h = m[2][1], i = m[2][2];

        return a * (e * i - f * h)
            - b * (d * i - f * g)
            + c * (d * h - e * g);
    }
   
    float Matrix4f::Determinant() const
    {
        return m[0][0] * m[1][1] * m[2][2] * m[3][3] - m[0][0] * m[1][1] * m[2][3] * m[3][2] + m[0][0] * m[1][2] * m[2][3] * m[3][1] - m[0][0] * m[1][2] * m[2][1] * m[3][3]
            + m[0][0] * m[1][3] * m[2][1] * m[3][2] - m[0][0] * m[1][3] * m[2][2] * m[3][1] - m[0][1] * m[1][2] * m[2][3] * m[3][0] + m[0][1] * m[1][2] * m[2][0] * m[3][3]
            - m[0][1] * m[1][3] * m[2][0] * m[3][2] + m[0][1] * m[1][3] * m[2][2] * m[3][0] - m[0][1] * m[1][0] * m[2][2] * m[3][3] + m[0][1] * m[1][0] * m[2][3] * m[3][2]
            + m[0][2] * m[1][3] * m[2][0] * m[3][1] - m[0][2] * m[1][3] * m[2][1] * m[3][0] + m[0][2] * m[1][0] * m[2][1] * m[3][3] - m[0][2] * m[1][0] * m[2][3] * m[3][1]
            + m[0][2] * m[1][1] * m[2][3] * m[3][0] - m[0][2] * m[1][1] * m[2][0] * m[3][3] - m[0][3] * m[1][0] * m[2][1] * m[3][2] + m[0][3] * m[1][0] * m[2][2] * m[3][1]
            - m[0][3] * m[1][1] * m[2][2] * m[3][0] + m[0][3] * m[1][1] * m[2][0] * m[3][2] - m[0][3] * m[1][2] * m[2][0] * m[3][1] + m[0][3] * m[1][2] * m[2][1] * m[3][0];
    }


    Matrix4f Matrix4f::Inverse() const
    {
        // Compute the reciprocal determinant
        float det = Determinant();

        if (det == 0.0f)
        {
           
            return *this;
        }

        float invdet = 1.0f / det;

        Matrix4f res;
        res.m[0][0] = invdet * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[1][2] *
            (m[2][3] * m[3][1] - m[2][1] * m[3][3]) + m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
        res.m[0][1] = -invdet * (m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[0][2] *
            (m[2][3] * m[3][1] - m[2][1] * m[3][3]) + m[0][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
        res.m[0][2] = invdet * (m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) + m[0][2] *
            (m[1][3] * m[3][1] - m[1][1] * m[3][3]) + m[0][3] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]));
        res.m[0][3] = -invdet * (m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) + m[0][2] *
            (m[1][3] * m[2][1] - m[1][1] * m[2][3]) + m[0][3] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]));
        res.m[1][0] = -invdet * (m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[1][2] *
            (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
        res.m[1][1] = invdet * (m[0][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[0][2] *
            (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[0][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
        res.m[1][2] = -invdet * (m[0][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) + m[0][2] *
            (m[1][3] * m[3][0] - m[1][0] * m[3][3]) + m[0][3] * (m[1][0] * m[3][2] - m[1][2] * m[3][0]));
        res.m[1][3] = invdet * (m[0][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) + m[0][2] *
            (m[1][3] * m[2][0] - m[1][0] * m[2][3]) + m[0][3] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]));
        res.m[2][0] = invdet * (m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) + m[1][1] *
            (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
        res.m[2][1] = -invdet * (m[0][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) + m[0][1] *
            (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[0][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
        res.m[2][2] = invdet * (m[0][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) + m[0][1] *
            (m[1][3] * m[3][0] - m[1][0] * m[3][3]) + m[0][3] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
        res.m[2][3] = -invdet * (m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) + m[0][1] *
            (m[1][3] * m[2][0] - m[1][0] * m[2][3]) + m[0][3] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
        res.m[3][0] = -invdet * (m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) + m[1][1] *
            (m[2][2] * m[3][0] - m[2][0] * m[3][2]) + m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
        res.m[3][1] = invdet * (m[0][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) + m[0][1] *
            (m[2][2] * m[3][0] - m[2][0] * m[3][2]) + m[0][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
        res.m[3][2] = -invdet * (m[0][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) + m[0][1] *
            (m[1][2] * m[3][0] - m[1][0] * m[3][2]) + m[0][2] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
        res.m[3][3] = invdet * (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) + m[0][1] *
            (m[1][2] * m[2][0] - m[1][0] * m[2][2]) + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
        return res;
    }


    void Matrix4f::CalcClipPlanes(MyVectorV3::Vector4f& l, MyVectorV3::Vector4f& r, MyVectorV3::Vector4f& b, MyVectorV3::Vector4f& t, MyVectorV3::Vector4f& n, MyVectorV3::Vector4f& f) const
    {
        MyVectorV3::Vector4f Row1(m[0][0], m[0][1], m[0][2], m[0][3]);
        MyVectorV3::Vector4f Row2(m[1][0], m[1][1], m[1][2], m[1][3]);
        MyVectorV3::Vector4f Row3(m[2][0], m[2][1], m[2][2], m[2][3]);
        MyVectorV3::Vector4f Row4(m[3][0], m[3][1], m[3][2], m[3][3]);

        l = Row1 + Row4;
        r = Row1 - Row4;
        b = Row2 + Row4;
        t = Row2 - Row4;
        n = Row3 + Row4;
        f = Row3 - Row4;
    }

    void Matrix4f::Compose(const MyVectorV3::Vector3f& Position, const Quaternion& Rotate,
        const MyVectorV3::Vector3f& Scale) {

		MyMatrixV3::Matrix4f pos, rot, scale;

		pos.InitTranslationTransform(Position);
		rot.InitRotateTransform(Rotate);
		scale.InitScaleTransform(Scale);
      
		*this = pos * rot * scale;

    }

    void Matrix4f::Decompose(MyVectorV3::Vector3f& Position, Quaternion& Rotate,
        MyVectorV3::Vector3f& Scale) {

		float sx = MyVectorV3::Vector3f(m[0][0], m[1][0], m[2][0]).Length();
        float sy = MyVectorV3::Vector3f(m[0][1], m[1][1], m[2][1]).Length();
        float sz = MyVectorV3::Vector3f(m[0][2], m[1][2], m[2][2]).Length();

        float det = this->Determinant();
		if (det < 0) sx = -sx;

		Position.x = m[0][3];
		Position.y = m[1][3];
		Position.z = m[2][3];
    

		MyMatrixV3::Matrix4f _m1 = *this;

		float invSX = 1.0f / sx;
		float invSY = 1.0f / sy;
		float invSZ = 1.0f / sz;

		_m1.m[0][0] *= invSX;
		_m1.m[1][0] *= invSX;
		_m1.m[2][0] *= invSX;

		_m1.m[0][1] *= invSY;
		_m1.m[1][1] *= invSY;
		_m1.m[2][1] *= invSY;

		_m1.m[0][2] *= invSZ;
		_m1.m[1][2] *= invSZ;
		_m1.m[2][2] *= invSZ;

		Rotate.SetFromRotationMatrix(_m1);

		Scale.x = sx;
		Scale.y = sy;
		Scale.z = sz;
    
    }




    Matrix3f::Matrix3f(float a00, float a01, float a02,
                       float a10, float a11, float a12,
                       float a20, float a21, float a22)
    {
        m[0][0] = a00; m[0][1] = a01; m[0][2] = a02;
        m[1][0] = a10; m[1][1] = a11; m[1][2] = a12;
        m[2][0] = a20; m[2][1] = a21; m[2][2] = a22;
    }


    Matrix3f::Matrix3f(float* poiMatrix) {

        float* poi = (float*)(&m[0][0]);

        for (int i = 0; i < 9; i++) {

            *(poi + i) = *(poiMatrix + i);
        }

    }


    void Matrix3f::InitRotateTransform(float RotateX, float RotateY, float RotateZ)
    {
        Matrix3f rx, ry, rz;

        float x = ToRadian(RotateX);
        float y = ToRadian(RotateY);
        float z = ToRadian(RotateZ);

        rx.InitRotationX(x);
        ry.InitRotationY(y);
        rz.InitRotationZ(z);

        *this = rz * ry * rx;
    }

    void Matrix3f::InitRotateTransform(const MyVectorV3::Vector3f& Rotate) {

        InitRotateTransform(Rotate.x, Rotate.y, Rotate.z);
    }
    void Matrix3f::InitRotateTransform(const Quaternion& quat) {

        float yy2 = 2.0f * quat.y * quat.y;
        float xy2 = 2.0f * quat.x * quat.y;
        float xz2 = 2.0f * quat.x * quat.z;
        float yz2 = 2.0f * quat.y * quat.z;
        float zz2 = 2.0f * quat.z * quat.z;
        float wz2 = 2.0f * quat.w * quat.z;
        float wy2 = 2.0f * quat.w * quat.y;
        float wx2 = 2.0f * quat.w * quat.x;
        float xx2 = 2.0f * quat.x * quat.x;

        m[0][0] = -yy2 - zz2 + 1.0f;
        m[0][1] = xy2 - wz2;
        m[0][2] = xz2 + wy2;
      
        m[1][0] = xy2 + wz2;
        m[1][1] = -xx2 - zz2 + 1.0f;
        m[1][2] = yz2 - wx2;
      
        m[2][0] = xz2 - wy2;
        m[2][1] = yz2 + wx2;
        m[2][2] = -xx2 - yy2 + 1.0f;
    
    

    }



    void Matrix3f::InitRotationX(float x)
    {
        m[0][0] = 1.0f; m[0][1] = 0.0f;     m[0][2] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = cosf(x);  m[1][2] = sinf(x);
        m[2][0] = 0.0f; m[2][1] = -sinf(x); m[2][2] = cosf(x);
    }


    void Matrix3f::InitRotationY(float y)
    {
        m[0][0] = cosf(y); m[0][1] = 0.0f; m[0][2] = -sinf(y);
        m[1][0] = 0.0f;    m[1][1] = 1.0f; m[1][2] = 0.0f;
        m[2][0] = sinf(y); m[2][1] = 0.0f; m[2][2] = cosf(y);
    }


    void Matrix3f::InitRotationZ(float z)
    {
        m[0][0] = cosf(z);  m[0][1] = sinf(z); m[0][2] = 0.0f;
        m[1][0] = -sinf(z); m[1][1] = cosf(z); m[1][2] = 0.0f;
        m[2][0] = 0.0f;     m[2][1] = 0.0f;    m[2][2] = 1.0f;
    }

    Quaternion::Quaternion(float Angle, const MyVectorV3::Vector3f& V)
    {
		this->SetFromAxisAngle(Angle, V);
    }

    Quaternion::Quaternion(float _x, float _y, float _z, float _w)
    {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }

    void Quaternion::Normalize()
    {
        float Length = sqrtf(x * x + y * y + z * z + w * w);

        x /= Length;
        y /= Length;
        z /= Length;
        w /= Length;
    }

    float Quaternion::Magnitude() {

		float sqLen = x * x + y * y + z * z + w * w;
		return sqrtf(sqLen);
    }

    Quaternion Quaternion::Conjugate() 
    {
        Quaternion ret(-x, -y, -z, w);
        return ret;
    }

    Quaternion Quaternion::Inverse() {

		float sqLen = x * x + y * y + z * z + w * w;
		Quaternion ret = Conjugate();
		
        ret.x /= sqLen;
        ret.y /= sqLen;
        ret.z /= sqLen;
        ret.w /= sqLen;


        return ret;
    }


    Quaternion Quaternion::Multiply(const Quaternion& r) {


		return (*this) * r;
    }

    
    Quaternion Quaternion::Premultiply(const Quaternion& l) {


		return l * (*this);
    }


    MyVectorV3::Vector3f Quaternion::Rotate(MyVectorV3::Vector3f v) {

		Quaternion qv = (*this) * v * this->Inverse();
		return MyVectorV3::Vector3f(qv.x, qv.y, qv.z);
    }


    Quaternion operator*(const Quaternion& q, const MyVectorV3::Vector3f& v)
    {
        float w = -(q.x * v.x) - (q.y * v.y) - (q.z * v.z);
        float x = (q.w * v.x) + (q.y * v.z) - (q.z * v.y);
        float y = (q.w * v.y) + (q.z * v.x) - (q.x * v.z);
        float z = (q.w * v.z) + (q.x * v.y) - (q.y * v.x);

        Quaternion ret(x, y, z, w);

        return ret;
    }


    Quaternion operator*(const Quaternion& l, const Quaternion& r)
    {
        float w = (l.w * r.w) - (l.x * r.x) - (l.y * r.y) - (l.z * r.z);
        float x = (l.x * r.w) + (l.w * r.x) + (l.y * r.z) - (l.z * r.y);
        float y = (l.y * r.w) + (l.w * r.y) + (l.z * r.x) - (l.x * r.z);
        float z = (l.z * r.w) + (l.w * r.z) + (l.x * r.y) - (l.y * r.x);

        Quaternion ret(x, y, z, w);

        return ret;
    }



    MyVectorV3::Vector3f Quaternion::ToEulerAngles(string order)
    {
     
        MyMatrixV3::Matrix4f M = this->ToMatrix4();
		float m11 = M.m[0][0], m12 = M.m[0][1], m13 = M.m[0][2];
		float m21 = M.m[1][0], m22 = M.m[1][1], m23 = M.m[1][2];
		float m31 = M.m[2][0], m32 = M.m[2][1], m33 = M.m[2][2];
        float x=0, y=0, z=0;

        float(*Clamp)(float, float, float) = [](float v, float min, float max) {
            return (v < min) ? min : ((v > max) ? max : v);
			};

        transform(order.begin(), order.end(), order.begin(), ::toupper);

        if(order=="XYZ")
        {
            y = asinf(Clamp(m13, -1.0f, 1.0f));
            if (fabs(m13) < 0.99999f) {
                x = atan2f(-m23, m33);
                z = atan2f(-m12, m11);
            }
            else {
				x = atan2f(m32, m22);
                z = 0;
            }
        }
        else if(order=="XZY")
        {
            z = asinf(-Clamp(m12, -1.0f, 1.0f));
            if (fabs(m12) < 0.99999f) {
                x = atan2f(m32, m22);
                y = atan2f(m13, m11);
            }
            else {
				x = atan2f(-m23, m33);
                y = 0;
            }
        }
        else if(order=="YXZ")
        {
            x = asinf(-Clamp(m23, -1.0f, 1.0f));
            if (fabs(m23) < 0.99999f) {
                y = atan2f(m13, m33);
                z = atan2f(m21, m22);
            }
            else {
                y = atan2f(-m31,m11);
                z = 0;
            }
        }
        else if(order=="YZX")
        {
            z = asinf(Clamp(m21, -1.0f, 1.0f));
            if (fabs(m21) < 0.99999f) {
                y = atan2f(-m31, m11);
                x = atan2f(-m23, m22);
            }
            else {
				y = atan2f(m13, m33);
                x = 0;
            }
        }
        else if(order=="ZXY")
        {
            x = asinf(Clamp(m32, -1.0f, 1.0f));
            if (fabs(m32) < 0.99999f) {
                z = atan2f(-m12, m22);
                y = atan2f(-m31, m33);
            }
            else {
                z = atan2f(m21,m11);
				y = 0;
            }
        }
        else if(order=="ZYX")
        {
            y = asinf(-Clamp(m31, -1.0f, 1.0f));
            if (fabs(m31) < 0.99999f) {
                z = atan2f(m21, m11);
                x = atan2f(m32, m33);
            }
            else {
				z = atan2f(-m12, m22);
                x = 0;
            }
        }


		return MyVectorV3::Vector3f(ToDegree(x), ToDegree(y), ToDegree(z));

     
    }

    void Quaternion::SetFromRotationMatrix(const Matrix4f& M) {

		float m11 = M.m[0][0], m12 = M.m[0][1], m13 = M.m[0][2];
		float m21 = M.m[1][0], m22 = M.m[1][1], m23 = M.m[1][2];
		float m31 = M.m[2][0], m32 = M.m[2][1], m33 = M.m[2][2];

		float trace = m11 + m22 + m33;

        if (trace > 0) {

			float s = 0.5f / sqrtf(trace + 1.0f);
            
			w = 0.25f / s;
			x = (m32 - m23) * s;
			y = (m13 - m31) * s;
            z = (m21 - m12) * s;
        }
		else if (m11 > m22 && m11 > m33) {

			float s = 2.0f * sqrtf(1.0f + m11 - m22 - m33);

			w = (m32 - m23) / s;
			x = 0.25f * s;
			y = (m12 + m21) / s;
			z = (m13 + m31) / s;

		}
        else if (m22 > m33) {

			float s = 2.0f * sqrtf(1.0f + m22 - m11 - m33);

			w = (m13 - m31) / s;
			x = (m12 + m21) / s;
			y = 0.25f * s;
			z = (m23 + m32) / s;

        }
        else {

			float s = 2.0f * sqrtf(1.0f + m33 - m11 - m22);

			w = (m21 - m12) / s;
			x = (m13 + m31) / s;
			y = (m23 + m32) / s;
			z = 0.25f * s;

        }


    }

    void Quaternion::SetFromAxisAngle(float Angle, const MyVectorV3::Vector3f& V) {

        float HalfAngleInRadians = ToRadian(Angle / 2);

        float SineHalfAngle = sinf(HalfAngleInRadians);
        float CosHalfAngle = cosf(HalfAngleInRadians);

        x = V.x * SineHalfAngle;
        y = V.y * SineHalfAngle;
        z = V.z * SineHalfAngle;
        w = CosHalfAngle;

    }


    void Quaternion::SetFromEuler(float x, float y, float z, string order) {

		Quaternion qx(x, MyVectorV3::Vector3f(1, 0, 0));
		Quaternion qy(y, MyVectorV3::Vector3f(0, 1, 0));
		Quaternion qz(z, MyVectorV3::Vector3f(0, 0, 1));

        transform(order.begin(), order.end(), order.begin(), ::toupper);

        if(order=="XYZ")
            *this = qz * qy * qx;
        else if(order=="XZY")
            *this = qy * qz * qx;
        else if(order=="YXZ")
            *this = qz * qx * qy;
        else if(order=="YZX")
            *this = qx * qz * qy;
        else if(order=="ZXY")
            *this = qy * qx * qz;
		else if (order == "ZYX")
			*this = qx * qy * qz;


    }

    Matrix3f Quaternion::ToMatrix3() {

		MyMatrixV3::Matrix3f M;

        M.m[0][0] = 1 - 2 * y * y - 2 * z * z;
        M.m[0][1] = 2 * x * y - 2 * z * w;
        M.m[0][2] = 2 * x * z + 2 * y * w;

        M.m[1][0] = 2 * x * y + 2 * z * w;
        M.m[1][1] = 1 - 2 * x * x - 2 * z * z;
        M.m[1][2] = 2 * y * z - 2 * x * w;

        M.m[2][0] = 2 * x * z - 2 * y * w;
        M.m[2][1] = 2 * y * z + 2 * x * w;
        M.m[2][2] = 1 - 2 * x * x - 2 * y * y;

        return M;


    }

    Matrix4f Quaternion::ToMatrix4() {

        MyMatrixV3::Matrix4f M;

        M.m[0][0] = 1 - 2 * y * y - 2 * z * z;
        M.m[0][1] = 2 * x * y - 2 * z * w;
        M.m[0][2] = 2 * x * z + 2 * y * w;
        M.m[0][3] = 0;

        M.m[1][0] = 2 * x * y + 2 * z * w;
        M.m[1][1] = 1 - 2 * x * x - 2 * z * z;
        M.m[1][2] = 2 * y * z - 2 * x * w;
        M.m[1][3] = 0;

        M.m[2][0] = 2 * x * z - 2 * y * w;
        M.m[2][1] = 2 * y * z + 2 * x * w;
        M.m[2][2] = 1 - 2 * x * x - 2 * y * y;
        M.m[2][3] = 0;

        M.m[3][0] = 0;
        M.m[3][1] = 0;
        M.m[3][2] = 0;
        M.m[3][3] = 1;
       
        return M;

    }
   


}