#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "mat4.h"

namespace math
{
    struct Camera
    {
        Vec3 position;
        Vec3 target;
        Vec3 up;

        mat4 GetViewMatrix() const {
            mat4 viewMat = mat4().Identity();

            Vec3 forward = (target - position).Normalized();
            Vec3 right = Vec3().Cross(forward, up).Normalized();
            Vec3 trueUp = Vec3().Cross(forward, right);

            viewMat.m[0][0] = right.x;
            viewMat.m[1][0] = right.y;
            viewMat.m[2][0] = right.z;
            viewMat.m[3][0] = -Vec3::Dot(right, position);

            viewMat.m[0][1] = trueUp.x;
            viewMat.m[1][1] = trueUp.y;
            viewMat.m[2][1] = trueUp.z;
            viewMat.m[3][1] = -Vec3::Dot(trueUp, position);

            viewMat.m[0][2] = -forward.x;
            viewMat.m[1][2] = -forward.y;
            viewMat.m[2][2] = -forward.z;
            viewMat.m[3][2] = Vec3::Dot(forward, position);

            return viewMat;
        }
    };
} // namespace math


#endif // CAMERA_H