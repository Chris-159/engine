#ifndef CAMERA_H
#define CAMERA_H

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
            Vec3 trueUp = Vec3().Cross(right, forward);

            viewMat.m[0][0] = right.x;
            viewMat.m[1][0] = right.y;
            viewMat.m[2][0] = right.z;
            // 0

            viewMat.m[0][1] = trueUp.x;
            viewMat.m[1][1] = trueUp.y;
            viewMat.m[2][1] = trueUp.z;
            // 0

            viewMat.m[0][2] = -forward.x;
            viewMat.m[1][2] = -forward.y;
            viewMat.m[2][2] = -forward.z;
            // 0

            viewMat.m[0][3] = -Vec3::Dot(right, position);
            viewMat.m[1][3] = -Vec3::Dot(trueUp, position);
            viewMat.m[2][3] = Vec3::Dot(forward, position);
            // 1

            return viewMat;
        }

        // 
        mat4 GetProjectionMatrix(float fov_, float aspect_, float near_, float far_) const {
            float f_ = 1.0f / tanf(fov_ * 0.5f);
            mat4 projMat = mat4();

            projMat.m[0][0] = f_ / aspect_;
            projMat.m[1][1] = f_;
            projMat.m[2][2] = (far_ + near_) / (near_ - far_);
            projMat.m[2][3] = (2 * far_ * near_) / (near_ - far_);
            projMat.m[3][2] = -1.0f;
            projMat.m[3][3] = 0.0f;

            return projMat;
        }

        Vec3 ProjectPoint(const Vec3& worldPos_, const mat4& view_, const mat4& projection_) {
            Vec4 p(worldPos_.x, worldPos_.y, worldPos_.z, 1.0f);
            Vec4 viewPos = view_ * p;
            Vec4 clipPos = projection_ * viewPos;

            if (fabs(clipPos.w) > 0.00001f) {
                clipPos.x /= clipPos.w;
                clipPos.y /= clipPos.w;
                clipPos.z /= clipPos.w;
            }

            return Vec3(clipPos.x, clipPos.y, clipPos.z);
        }

        // math::Vec4 Camera::ProjectPoint4(const math::Vec3& worldPos, const math::mat4& view, const math::mat4& proj) const {
        //     // world -> view -> clip
        //     math::Vec4 v(worldPos, 1.0f);
        //     math::Vec4 viewV = view * v;
        //     math::Vec4 clipV = proj * viewV;
        //     return clipV; // (x,y,z,w) clip space; NDC = clipV / clipV.w
        // }
    };
} // namespace math


#endif // CAMERA_H