#ifndef projection_v2_h
#define projection_v2_h

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




namespace MyProjection {



    struct PersProjInfo
    {
        float FOV;
        float Width;
        float Height;
        float zNear;
        float zFar;
    };


    struct OrthoProjInfo
    {
        float r;        // right
        float l;        // left
        float b;        // bottom
        float t;        // top
        float n;        // z near
        float f;        // z far

        float Width;
        float Height;

        void Print()
        {
            printf("Left %f   Right %f\n", l, r);
            printf("Bottom %f Top %f\n", b, t);
            printf("Near %f   Far %f\n", n, f);
        }
    };





}

#endif