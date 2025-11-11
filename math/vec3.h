#ifndef VEC3_H
#define VEC3_H

#include<math.h>

namespace math {
    struct Vec3
    {
        float x, y, z;
        
        Vec3() : x(0), y(0), z(0) {}
        Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

        // Basic operators
        Vec3 operator+(const Vec3& vector_ ) const { return Vec3(x + vector_.x, y + vector_.y, z + vector_.z); };
        Vec3 operator-(const Vec3& vector_ ) const { return Vec3(x - vector_.x, y - vector_.y, z - vector_.z); };
        Vec3 operator-() const { return Vec3(-x, -y, -z); };
        Vec3 operator*( float scalar_ ) const { return Vec3(x * scalar_, y * scalar_, z * scalar_); };
        Vec3 operator/( float scalar_ ) const { return Vec3(x / scalar_, y / scalar_, z / scalar_); };


        float Length() const {
            return sqrtf((x * x) + (y * y) + (z * z));
        }

        Vec3 Normalized() const {
            float lngth = Length();

            if (lngth < 0.000001f)
                return *this;

            return *this / lngth;
        }

        void Normalize() {
            float lngth = Length();

            if (lngth < 0.000001f)
                return;

            *this = *this / lngth;
        }

        static float Dot(const Vec3& v1_, const Vec3& v2_) {
           return ((v1_.x * v2_.x) + (v1_.y * v2_.y) + (v1_.z * v2_.z)); 
        }

        static Vec3 Cross(const Vec3& v1_, const Vec3& v2_) {
            return Vec3(v1_.y * v2_.z - v1_.z * v2_.y,
                        v1_.z * v2_.x - v1_.x * v2_.z,
                        v1_.x * v2_.y - v1_.y * v2_.x);
        }

        // Converts NDC to screen coordinates
        static Vec3 ToScreen(const Vec3& ndc_, int32_t width_, int32_t height_) {
            return Vec3(
                (ndc_.x * 0.5f + 0.5f) * width_,
                (1.0f - (ndc_.y * 0.5f + 0.5f)) * height_,
                ndc_.z
            );
        }
    };
}


#endif // VEC3_H