#include "ogl_Vector.h"


namespace MyVectorV3 {


    Vector4f& Vector4f::Normalize()
    {
        float len = Length();

       

        x /= len;
        y /= len;
        z /= len;
        w /= len;

        return *this;
    }


    float RandomFloat()
    {
        float Max = RAND_MAX;
        return ((float)rand() / Max);
    }

    float RandomFloatRange(float Start, float End)
    {
        // return Start;
        if (End == Start) {
            return Start;
        }

        if (End < Start) {
            printf("Invalid random range: (%f, %f)\n", Start, End);
            exit(0);
        }

        float Delta = End - Start;

        float RandomValue = RandomFloat() * Delta + Start;

        return RandomValue;
    }
    
    void Vector3f::InitRandom(const Vector3f& MinVal, const Vector3f& MaxVal)
    {
        x = RandomFloatRange(MinVal.x, MaxVal.x);
        y = RandomFloatRange(MinVal.y, MaxVal.y);
        z = RandomFloatRange(MinVal.z, MaxVal.z);
    }


    Vector3f Vector3f::Cross(const Vector3f& v) const
    {
        const float _x = y * v.z - z * v.y;
        const float _y = z * v.x - x * v.z;
        const float _z = x * v.y - y * v.x;

        return Vector3f(_x, _y, _z);
    }

    Vector3f& Vector3f::Normalize()
    {
        float len = Length();


        x /= len;
        y /= len;
        z /= len;

        return *this;
    }

    void Vector3f::Rotate(float Angle, const Vector3f& V)
    {


        Quaternion RotationQ(Angle, V);

        Quaternion ConjugateQ = RotationQ.Conjugate();

        Quaternion W = RotationQ * (*this) * ConjugateQ;

        x = W.x;
        y = W.y;
        z = W.z;
    }


    Vector3f Vector3f::Negate() const
    {
        Vector3f ret(-x, -y, -z);
        return ret;
    }


    Quaternion::Quaternion(float Angle, const Vector3f& V)
    {
        float HalfAngleInRadians = ToRadian(Angle / 2);

        float SineHalfAngle = sinf(HalfAngleInRadians);
        float CosHalfAngle = cosf(HalfAngleInRadians);

        x = V.x * SineHalfAngle;
        y = V.y * SineHalfAngle;
        z = V.z * SineHalfAngle;
        w = CosHalfAngle;
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


    Quaternion Quaternion::Conjugate() const
    {
        Quaternion ret(-x, -y, -z, w);
        return ret;
    }


    Quaternion operator*(const Quaternion& q, const Vector3f& v)
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



    Vector3f Quaternion::ToDegrees()
    {
        float f[3];

        f[0] = atan2(x * z + y * w, x * w - y * z);
        f[1] = acos(-x * x - y * y - z * z - w * w);
        f[2] = atan2(x * z - y * w, x * w + y * z);

        f[0] = ToDegree(f[0]);
        f[1] = ToDegree(f[1]);
        f[2] = ToDegree(f[2]);

        return Vector3f(f);
    }





}