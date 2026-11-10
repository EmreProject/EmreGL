#ifndef ogl_vector_h
#define ogl_vector_h

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


namespace MyVectorV3 {


    // powf wrapper for integer params to avoid crazy casting
#define powi(base,exp) (int)powf((float)(base), (float)(exp))
#define M_PI       3.14159265358979323846   // pi
#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))
#define RAND_MAX 0x7fff

    struct Vector2i
    {
        int x;
        int y;
    };

    struct Vector3i
    {
        union {
            float x;
            float r;
        };

        union {
            float y;
            float g;
        };

        union {
            float z;
            float b;
        };
    };

    struct Vector2f
    {
        union {
            float x;
            float u;
        };

        union {
            float y;
            float v;
        };

        Vector2f()
        {
        }

        Vector2f(float _x, float _y)
        {
            x = _x;
            y = _y;
        }

        void Print(bool endl = true) const
        {
            printf("(%f, %f)", x, y);

            if (endl) {
                printf("\n");
            }
        }

        float Length() const
        {
            float len = sqrtf(x * x + y * y);
            return len;
        }


        void Normalize()
        {
            float len = Length();



            x /= len;
            y /= len;
        }
    };




    struct Vector4f;

    struct Vector3f
    {
        union {
            float x;
            float r;
        };

        union {
            float y;
            float g;
        };

        union {
            float z;
            float b;
        };

        Vector3f() {}

        Vector3f(float _x, float _y, float _z)
        {
            x = _x;
            y = _y;
            z = _z;
        }

        Vector3f(const float* pFloat)
        {
            x = pFloat[0];
            y = pFloat[1];
            z = pFloat[2];
        }

        void InitRandom(const Vector3f& MinVal, const Vector3f& MaxVal);

        void InitBySphericalCoords(float Radius, float Pitch, float Heading)
        {
            x = Radius * cosf(ToRadian(Pitch)) * sinf(ToRadian(Heading));
            y = -Radius * sinf(ToRadian(Pitch));
            z = Radius * cosf(ToRadian(Pitch)) * cosf(ToRadian(Heading));
        }

        Vector3f(float f)
        {
            x = y = z = f;
        }

        Vector3f(const Vector4f& v);

        Vector3f& operator+=(const Vector3f& r)
        {
            x += r.x;
            y += r.y;
            z += r.z;

            return *this;
        }

        Vector3f& operator-=(const Vector3f& r)
        {
            x -= r.x;
            y -= r.y;
            z -= r.z;

            return *this;
        }

        Vector3f& operator*=(float f)
        {
            x *= f;
            y *= f;
            z *= f;

            return *this;
        }

        bool operator==(const Vector3f& r)
        {
            return ((x == r.x) && (y == r.y) && (z == r.z));
        }

        bool operator!=(const Vector3f& r)
        {
            return !(*this == r);
        }

        operator const float* () const
        {
            return &(x);
        }


        Vector3f Cross(const Vector3f& v) const;

        float Dot(const Vector3f& v) const
        {
            float ret = x * v.x + y * v.y + z * v.z;
            return ret;
        }

        float Distance(const Vector3f& v) const
        {
            float delta_x = x - v.x;
            float delta_y = y - v.y;
            float delta_z = z - v.z;
            float distance = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
            return distance;
        }

        float Length() const
        {
            float len = sqrtf(x * x + y * y + z * z);
            return len;
        }

        bool IsZero() const
        {
            return ((x + y + z) == 0.0f);
        }

        Vector3f& Normalize();

        void Rotate(float Angle, const Vector3f& Axis);

        Vector3f Negate() const;

        void Print(bool endl = true) const
        {
            printf("(%f, %f, %f)", x, y, z);

            if (endl) {
                printf("\n");
            }
        }

        float* data()
        {
            return &x;
        }
    };


    struct Vector4f
    {
        float x;
        float y;
        float z;
        float w;

        Vector4f()
        {
        }

        Vector4f(float _x, float _y, float _z, float _w)
        {
            x = _x;
            y = _y;
            z = _z;
            w = _w;
        }

        Vector4f(const Vector3f& v, float _w)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            w = _w;
        }

        void Print(bool endl = true) const
        {
            printf("(%f, %f, %f, %f)", x, y, z, w);

            if (endl) {
                printf("\n");
            }
        }

        Vector3f to3f() const
        {
            Vector3f v(x, y, z);
            return v;
        }

        float Length() const
        {
            float len = sqrtf(x * x + y * y + z * z + w * w);
            return len;
        }

        Vector4f& Normalize();

        float Dot(const Vector4f& v) const
        {
            float ret = x * v.x + y * v.y + z * v.z + w * v.w;
            return ret;
        }

        bool operator==(const Vector4f& r)
        {
            return ((x == r.x) && (y == r.y) && (z == r.z) && (w == r.w));
        }

    };


    inline Vector2f operator+(const Vector2f& l, const Vector2f& r)
    {
        Vector2f Ret(l.x + r.x,
            l.y + r.y);

        return Ret;
    }

    inline Vector2f operator-(const Vector2f& l, const Vector2f& r)
    {
        Vector2f Ret(l.x - r.x,
            l.y - r.y);

        return Ret;
    }

    inline Vector2f operator*(const Vector2f& l, float f)
    {
        Vector2f Ret(l.x * f,
            l.y * f);

        return Ret;
    }


    inline Vector2f operator/(const Vector2f& l, float f)
    {
        Vector2f Ret(l.x / f,
            l.y / f);

        return Ret;
    }

    inline Vector3f operator+(const Vector3f& l, const Vector3f& r)
    {
        Vector3f Ret(l.x + r.x,
            l.y + r.y,
            l.z + r.z);

        return Ret;
    }

    inline Vector3f operator-(const Vector3f& l, const Vector3f& r)
    {
        Vector3f Ret(l.x - r.x,
            l.y - r.y,
            l.z - r.z);

        return Ret;
    }

    inline Vector3f operator*(const Vector3f& l, float f)
    {
        Vector3f Ret(l.x * f,
            l.y * f,
            l.z * f);

        return Ret;
    }


    inline Vector3f operator*(const Vector3f& l, Vector3f f)
    {
        Vector3f Ret(l.x * f.x,
            l.y * f.y,
            l.z * f.z);

        return Ret;
    }


    inline Vector3f operator/(const Vector3f& l, float f)
    {
        Vector3f Ret(l.x / f,
            l.y / f,
            l.z / f);

        return Ret;
    }


    inline Vector3f::Vector3f(const Vector4f& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
    }


    inline Vector4f operator+(const Vector4f& l, const Vector4f& r)
    {
        Vector4f Ret(l.x + r.x,
            l.y + r.y,
            l.z + r.z,
            l.w + r.w);

        return Ret;
    }


    inline Vector4f operator-(const Vector4f& l, const Vector4f& r)
    {
        Vector4f Ret(l.x - r.x,
            l.y - r.y,
            l.z - r.z,
            l.w - r.w);

        return Ret;
    }


    inline Vector4f operator/(const Vector4f& l, float f)
    {
        Vector4f Ret(l.x / f,
            l.y / f,
            l.z / f,
            l.w / f);

        return Ret;
    }


    inline Vector4f operator*(const Vector4f& l, float f)
    {
        Vector4f Ret(l.x * f,
            l.y * f,
            l.z * f,
            l.w * f);

        return Ret;
    }


    inline Vector4f operator*(float f, const Vector4f& l)
    {
        Vector4f Ret(l.x * f,
            l.y * f,
            l.z * f,
            l.w * f);

        return Ret;
    }


    struct Quaternion
    {
        float x, y, z, w;

        Quaternion(float Angle, const Vector3f& V);

        Quaternion(float _x, float _y, float _z, float _w);

        void Normalize();

        Quaternion Conjugate() const;

        Vector3f ToDegrees();
    };

    Quaternion operator*(const Quaternion& l, const Quaternion& r);

    Quaternion operator*(const Quaternion& q, const Vector3f& v);



}

#endif 
