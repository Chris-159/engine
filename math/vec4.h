#ifndef VEC4_H
#define VEC4_H

#include "vec3.h"
#include<math.h>

namespace math {
    struct Vec4
    {
        float x, y, z, w;
        
        Vec4() : x(0), y(0), z(0), w(0) {}
        Vec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
        Vec4(const Vec3& v_, float w_) : x(v_.x), y(v_.y), z(v_.z), w(w_) {}

        // Basic operators
        Vec4 operator+(const Vec4& vector_ ) const { return Vec4(x + vector_.x, y + vector_.y, z + vector_.z, w + vector_.w); };
        Vec4 operator-(const Vec4& vector_ ) const { return Vec4(x - vector_.x, y - vector_.y, z - vector_.z, w - vector_.w); };
        Vec4 operator*( float scalar_ ) const { return Vec4(x * scalar_, y * scalar_, z * scalar_, w * scalar_); };
        Vec4 operator/( float scalar_ ) const { return Vec4(x / scalar_, y / scalar_, z / scalar_, w / scalar_); };


        float Length() const {
            return sqrtf((x * x) + (y * y) + (z * z) + (w * w));
        }

        Vec4 Normalized() const {
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

        
        static float Dot(const Vec4& v1_, const Vec4& v2_) {
           return ((v1_.x * v2_.x) + (v1_.y * v2_.y) + (v1_.z * v2_.z) + (v1_.w * v2_.w)); 
        }

        static Vec4 Cross(const Vec4& v1_, const Vec4& v2_) {
            return Vec4(v1_.y * v2_.z - v1_.z * v2_.y,
                        v1_.z * v2_.x - v1_.x * v2_.z,
                        v1_.x * v2_.y - v1_.y * v2_.x,
                        1.0f);
        }
    };
}


#endif // VEC4_H