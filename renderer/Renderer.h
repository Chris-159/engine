#ifndef RENDERER_H
#define RENDERER_H

#include<stdint.h>
#include<algorithm>
#include "../math/camera.h"
#include "../core/Model.h"

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

    public:
        Renderer(uint32_t* framebuffer_, int32_t width_, int32_t height_);

        bool DrawModel(core::Model& model_);
        void DrawLine(uint32_t* framebuffer_, int width_, int height_,
            int x0, int y0, int x1, int y1, uint32_t color_);
        
        
        void DrawTriangleWireframe(uint32_t* framebuffer_, int width_, int height_,
            const math::Vec3& s0, const math::Vec3& s1, const math::Vec3& s2, uint32_t color_);
        void DrawTriangleFilled(uint32_t* framebuffer_, int width_, int height_,
            const math::Vec3& s0, const math::Vec3& s1, const math::Vec3& s2, uint32_t color_);
        

        void SetCamera(const math::mat4& viewMatrix_, const math::mat4& projectionMatrix_);
        void SetActiveCamera(math::Camera* camera_);
        void SetModelMatrix(const math::mat4& modelMatrix_);
        
        void Clear(uint32_t clear_color);
    };
    
    
} // namespace renderer


#endif // RENDERER_H