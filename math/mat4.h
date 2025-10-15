#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"

namespace math
{
    struct mat4
    {
        float m[4][4] = { 0 };


        Vec3 operator*(const Vec3& v_) const { 
            float x_ = m[0][0]*v_.x + m[0][1]*v_.y + m[0][2]*v_.z + m[0][3]*1;
            float y_ = m[1][0]*v_.x + m[1][1]*v_.y + m[1][2]*v_.z + m[1][3]*1;
            float z_ = m[2][0]*v_.x + m[2][1]*v_.y + m[2][2]*v_.z + m[2][3]*1;
            float w_ = m[3][0]*v_.x + m[3][1]*v_.y + m[3][2]*v_.z + m[3][3]*1; 
            
            if (w_ != 0.0f) {
                x_ /= w_;
                y_ /= w_;
                z_ /= w_;
            }

            return Vec3(x_, y_, z_); 
        };
        mat4 operator*(const mat4& other_) const {
            mat4 out_ = mat4();
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++) {
                    out_.m[i][j] = 0.0f;
                    for (int k = 0; k < 4; k++)
                        out_.m[i][j] += m[i][k] * other_.m[k][j];
                }
            
            return out_;
        };


        static mat4 Identity() {
            mat4 mat_ = mat4();
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    if (i == j)
                        mat_.m[i][j] = 1;
                    else
                        mat_.m[i][j] = 0;

            return mat_;
        }

        static mat4 Translation(const Vec3& t_) {
            mat4 mattr_ = Identity();

            mattr_.m[0][3] = t_.x;
            mattr_.m[1][3] = t_.y;
            mattr_.m[2][3] = t_.z;

            return mattr_;
        }

        void Translate(const Vec3& t_) {
            m[0][3] += t_.x;
            m[1][3] += t_.y;
            m[2][3] += t_.z;
        }

        static mat4 Scale(const Vec3& s_) {
            mat4 scaled_ = mat4();

            scaled_.m[0][0] = s_.x;
            scaled_.m[1][1] = s_.y;
            scaled_.m[2][2] = s_.z;
            scaled_.m[3][3] = 1.0f;

            return scaled_;
        }

        static mat4 RotationX(float angle) {
            mat4 rotated_ = mat4().Identity();

            rotated_.m[1][1] = cosf(angle);
            rotated_.m[1][2] = -sinf(angle);
            rotated_.m[2][1] = sinf(angle);
            rotated_.m[2][2] = cosf(angle);

            return rotated_;
        }        
        static mat4 RotationY(float angle) {
            mat4 rotated_ = mat4().Identity();

            rotated_.m[0][0] = cosf(angle);
            rotated_.m[0][2] = sinf(angle);
            rotated_.m[2][0] = -sinf(angle);
            rotated_.m[2][2] = cosf(angle);

            return rotated_;
        }
        static mat4 RotationZ(float angle) {
            mat4 rotated_ = mat4().Identity();

            rotated_.m[0][0] = cosf(angle);
            rotated_.m[0][1] = -sinf(angle);
            rotated_.m[1][0] = sinf(angle);
            rotated_.m[1][1] = cosf(angle);

            return rotated_;
        }

        // Create the ModelMatrix (you can apply this to any point in the 3D space)
        static mat4 TRS(const Vec3& position_, const Vec3& rotation_, const Vec3& scale_) {
            mat4 scale_mat = mat4().Scale(scale_);
            mat4 trans_mat = mat4().Translation(position_);

            mat4 rotX = mat4().RotationX(rotation_.x);
            mat4 rotY = mat4().RotationY(rotation_.y);
            mat4 rotZ = mat4().RotationZ(rotation_.z);
            mat4 rot_mat = rotZ * rotY * rotX;

            // S → R → T = scale, rotate, translation (and flip this, because.... math)
            mat4 trs_ = trans_mat * rot_mat * scale_mat;

            return trs_;
        }
    };

} // namespace math


#endif // MAT4_H