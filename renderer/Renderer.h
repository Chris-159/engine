#ifndef RENDERER_H
#define RENDERER_H

#include<stdint.h>
#include<algorithm>
#include "../math/camera.h"
#include "../core/Model.h"
#include "../misc/light.h"
#include "../misc/texture.h"

namespace renderer
{
    class Renderer
    {
    private:
        std::vector<float> depthBuffer;    

        uint32_t* frameBuffer;
        int32_t width, height;
        
        uint32_t color;
        
        math::mat4 viewMatrix;
        math::mat4 projectionMatrix;
        math::mat4 modelMatrix;

        core::Model* currentModel;
        math::Camera* camera;

        misc::Light mainLight;

    public:
        Renderer(uint32_t* framebuffer_, int32_t width_, int32_t height_);

        bool DrawModel(core::Model& model_);
        void DrawLine(uint32_t* framebuffer_, int width_, int height_,
            int x0, int y0, int x1, int y1, uint32_t color_);
        
        
        void DrawTriangleWireframe(uint32_t* framebuffer_, int width_, int height_,
            const math::Vec3& s0, const math::Vec3& s1, const math::Vec3& s2, uint32_t color_);
        
        // void DrawTriangleFilled(
        //     uint32_t* framebuffer_, int width_, int height_,
        //     const math::Vec3& s0, const math::Vec3& s1, const math::Vec3& s2, 
        //     uint32_t color_,
        //     const misc::Texture* texture = nullptr,
        //     const math::Vec3* uv0 = nullptr,
        //     const math::Vec3* uv1 = nullptr,
        //     const math::Vec3* uv2 = nullptr,
        //     const float* recipW0 = nullptr,
        //     const float* recipW1 = nullptr,
        //     const float* recipW2 = nullptr,
        //     bool perspectiveCorrect = false
        // );

        // void DrawTriangleFilled(
        //     uint32_t* framebuffer_, int width_, int height_,
        //     const math::Vec3& s0_, const math::Vec3& s1_, const math::Vec3& s2_,
        //     const misc::Material* material_,
        //     const misc::Texture* texture_,
        //     const math::Vec3* uv0_ = nullptr,
        //     const math::Vec3* uv1_ = nullptr,
        //     const math::Vec3* uv2_ = nullptr,
        //     const float* recipW0_ = nullptr,
        //     const float* recipW1_ = nullptr,
        //     const float* recipW2_ = nullptr,
        //     const math::Vec3* n0_ = nullptr,
        //     const math::Vec3* n1_ = nullptr,
        //     const math::Vec3* n2_ = nullptr,
        //     const math::Vec3* viewPos0_ = nullptr,
        //     const math::Vec3* viewPos1_ = nullptr,
        //     const math::Vec3* viewPos2_ = nullptr,
        //     bool perspectiveCorrect_ = false
        // );

        void DrawTriangleFilled(
            uint32_t* framebuffer_, int width_, int height_,
            const math::Vec3& s0_, const math::Vec3& s1_, const math::Vec3& s2_,
            uint32_t color_,
            const misc::Texture* texture_,
            const misc::Material* material_ = nullptr,
            const math::Vec3* uv0_ = nullptr,
            const math::Vec3* uv1_ = nullptr,
            const math::Vec3* uv2_ = nullptr,
            const float* recipW0_ = nullptr,
            const float* recipW1_ = nullptr,
            const float* recipW2_ = nullptr,
            const math::Vec3* n0_ = nullptr,
            const math::Vec3* n1_ = nullptr,
            const math::Vec3* n2_ = nullptr,
            const math::Vec3* viewPos0_ = nullptr,
            const math::Vec3* viewPos1_ = nullptr,
            const math::Vec3* viewPos2_ = nullptr,
            bool perspectiveCorrect_ = false
        );

        uint32_t ApplyLight(const misc::Material& material, const misc::Light& light, const math::Vec3& normal, const math::Vec3& viewDir);
        
        void Clear(uint32_t clear_color);
        


        static inline math::Vec3 Reflect(const math::Vec3& incident_, const math::Vec3& normal_) {
            // R = I - 2 * (I · N) * N
            return incident_ - normal_ * (2.0f * math::Vec3::Dot(incident_, normal_));
        }

        inline void ComputeBarycentric(float px_, float py_, 
            const math::Vec3& a_, 
            const math::Vec3& b_, 
            const math::Vec3& c_,
            float& u_, float& v_, float& w_)
        {
            math::Vec3 v0 = b_ - a_;
            math::Vec3 v1 = c_ - a_;
            math::Vec3 v2(px_ - a_.x, py_ - a_.y, 0.0f);

            float d00 = math::Vec3::Dot(v0, v0);
            float d01 = math::Vec3::Dot(v0, v1);
            float d11 = math::Vec3::Dot(v1, v1);
            float d20 = math::Vec3::Dot(v2, v0);
            float d21 = math::Vec3::Dot(v2, v1);

            float denom = d00 * d11 - d01 * d01;
            // degenerate triangle
            if (fabs(denom) < 1e-8f) {
                u_ = v_ = w_ = -1.0f;
                return;
            }

            v_ = (d11 * d20 - d01 * d21) / denom;
            w_ = (d00 * d21 - d01 * d20) / denom;
            u_ = 1.0f - v_ - w_;
        }

        inline void SetCamera(const math::mat4& viewMatrix_, const math::mat4& projectionMatrix_) {
            viewMatrix = viewMatrix_;
            projectionMatrix = projectionMatrix_;
        }

        inline void SetActiveCamera(math::Camera* camera_)         { camera = camera_; }
        inline void SetModelMatrix(const math::mat4& modelMatrix_) { modelMatrix = modelMatrix_; }
        inline void SetLight(const misc::Light& light_)            { mainLight = light_; }
    };
    
    
} // namespace renderer


#endif // RENDERER_H