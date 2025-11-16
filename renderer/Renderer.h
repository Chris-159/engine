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
        // void DrawTriangleFilled(uint32_t* framebuffer_, int width_, int height_,
        //     const math::Vec3& s0, const math::Vec3& s1, const math::Vec3& s2, uint32_t color_);
        
        void DrawTriangleFilled(
            uint32_t* framebuffer_, int width_, int height_,
            const math::Vec3& s0, const math::Vec3& s1, const math::Vec3& s2, 
            uint32_t color_,
            const misc::Texture* texture = nullptr,
            const math::Vec3* uv0 = nullptr,
            const math::Vec3* uv1 = nullptr,
            const math::Vec3* uv2 = nullptr,
            const float* recipW0 = nullptr,
            const float* recipW1 = nullptr,
            const float* recipW2 = nullptr,
            bool perspectiveCorrect = false
        );

        void ComputeBarycentric(float px_, float py_, 
            const math::Vec3& a_, 
            const math::Vec3& b_, 
            const math::Vec3& c_,
            float& u_, float& v_, float& w_);

        void SetCamera(const math::mat4& viewMatrix_, const math::mat4& projectionMatrix_);
        void SetActiveCamera(math::Camera* camera_);
        void SetModelMatrix(const math::mat4& modelMatrix_);
        void SetLight(const misc::Light& light_);

        uint32_t ApplyLight(const misc::Material& material, const misc::Light& light, const math::Vec3& normal, const math::Vec3& viewDir);
        
        void Clear(uint32_t clear_color);

        static math::Vec3 Reflect(const math::Vec3& incident_, const math::Vec3& normal_) {
            // R = I - 2 * (I · N) * N
            return incident_ - normal_ * (2.0f * math::Vec3::Dot(incident_, normal_));
        }
    };
    
    
} // namespace renderer


#endif // RENDERER_H