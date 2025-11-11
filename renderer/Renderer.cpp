#include "Renderer.h"
#include<iostream>

using namespace renderer;
using namespace math;

//  CONSTRUCTOR
Renderer::Renderer(uint32_t* framebuffer_, int32_t width_, int32_t height_) :
        frameBuffer(framebuffer_), width(width_), height(height_),
        color(0xFFFFFFFF), camera(nullptr), currentModel(nullptr),
        modelMatrix(math::mat4::Identity()) 
        {
            depthBuffer.resize(width * height, 1.0f);
        }
// -----------------------------

void Renderer::SetCamera(const math::mat4& viewMatrix_, const math::mat4& projectionMatrix_) {
    viewMatrix = viewMatrix_;
    projectionMatrix = projectionMatrix_;
}

void Renderer::SetActiveCamera(math::Camera* camera_) {
    camera = camera_;
}

void Renderer::SetModelMatrix(const math::mat4& modelMatrix_) {
    modelMatrix = modelMatrix_;
}

void Renderer::DrawLine(uint32_t* framebuffer_, int width_, int height_,
              int x0, int y0, int x1, int y1, uint32_t color_)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true)
    {
        if (x0 >= 0 && x0 < width_ && y0 >= 0 && y0 < height_)
            framebuffer_[y0 * width_ + x0] = color_;

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void Renderer::DrawTriangleWireframe(uint32_t* framebuffer_, int width_, int height_,
                  const math::Vec3& s0, const math::Vec3& s1, const math::Vec3& s2, uint32_t color_)
{
    int32_t x0 = static_cast<int32_t>(s0.x);
    int32_t y0 = static_cast<int32_t>(s0.y);
    int32_t x1 = static_cast<int32_t>(s1.x);
    int32_t y1 = static_cast<int32_t>(s1.y);
    int32_t x2 = static_cast<int32_t>(s2.x);
    int32_t y2 = static_cast<int32_t>(s2.y);

    Renderer::DrawLine(framebuffer_, width_, height_, x0, y0, x1, y1, color_);
    Renderer::DrawLine(framebuffer_, width_, height_, x1, y1, x2, y2, color_);
    Renderer::DrawLine(framebuffer_, width_, height_, x2, y2, x0, y0, color_);
}

void Renderer::DrawTriangleFilled(uint32_t* framebuffer_, int width_, int height_,
                                  const math::Vec3& s0, const math::Vec3& s1, const math::Vec3& s2, uint32_t color_)
{
    float minX = std::min({ s0.x, s1.x, s2.x });
    float maxX = std::max({ s0.x, s1.x, s2.x });
    float minY = std::min({ s0.y, s1.y, s2.y });
    float maxY = std::max({ s0.y, s1.y, s2.y });

    // bounds checking
    minX = std::max(minX, 0.0f);
    minY = std::max(minY, 0.0f);
    maxX = std::min(maxX, float(width_ - 1));
    maxY = std::min(maxY, float(height_ - 1));

    // Precompute edges
    math::Vec3 v0 = s1 - s0;
    math::Vec3 v1 = s2 - s0;

    float d00 = math::Vec3::Dot(v0, v0);
    float d01 = math::Vec3::Dot(v0, v1);
    float d11 = math::Vec3::Dot(v1, v1);
    float denom = d00 * d11 - d01 * d01;
    if (fabs(denom) < 1e-8f) return; // Degenerate triangle

    // Main rasterization loop
    for (int y = int(minY); y <= int(maxY); ++y) {
        for (int x = int(minX); x <= int(maxX); ++x) {
            math::Vec3 v2((float)x - s0.x, (float)y - s0.y, 0.0f);

            float d20 = math::Vec3::Dot(v2, v0);
            float d21 = math::Vec3::Dot(v2, v1);

            float v = (d11 * d20 - d01 * d21) / denom;
            float w = (d00 * d21 - d01 * d20) / denom;
            float u = 1.0f - v - w;

            const float eps = -0.0001f;
            if (u < eps || v < eps || w < eps)
                continue;

            float z = u * s0.z + v * s1.z + w * s2.z;
            //float depth = (z * 0.5f) + 0.5f;

            int pixelIndex = y * width_ + x;

            // if (y % 100 == 0 && x % 100 == 0)
            //     std::cout << "z=" << z << " depth=" << depth
            //             << " buf=" << depthBuffer[pixelIndex] << std::endl;

            if (z < depthBuffer[pixelIndex]) {
                depthBuffer[pixelIndex] = z - 1e-5f; // depth - 1e-4f
                framebuffer_[pixelIndex] = color_;
            }
        }
    }
}


// void Renderer::DrawTriangleFilled(uint32_t* framebuffer_, int width_, int height_,
//                     const math::Vec3& s0, const math::Vec3& s1, const math::Vec3& s2, uint32_t color_) 
// {    
//     float minX = std::max(0.0f, std::floor(std::min({s0.x, s1.x, s2.x})));
//     float maxX = std::min(float(width_ - 1), std::ceil(std::max({s0.x, s1.x, s2.x})));
//     float minY = std::max(0.0f, std::floor(std::min({s0.y, s1.y, s2.y})));
//     float maxY = std::min(float(height_ - 1), std::ceil(std::max({s0.y, s1.y, s2.y})));

//     float denom = (s1.y - s2.y)*(s0.x - s2.x) + (s2.x - s1.x)*(s0.y - s2.y);
//     if (fabs(denom) < 1e-8f) return;

//     for (int y = int(minY); y <= int(maxY); ++y) {
//         for (int x = int(minX); x <= int(maxX); ++x) {
//             float px = float(x) + 0.5f;
//             float py = float(y) + 0.5f;

//             float w0 = ((s1.y - s2.y)*(px - s2.x) + (s2.x - s1.x)*(py - s2.y)) / denom;
//             float w1 = ((s2.y - s0.y)*(px - s2.x) + (s0.x - s2.x)*(py - s2.y)) / denom;
//             float w2 = 1.0f - w0 - w1;

//             if (w0 < 0 || w1 < 0 || w2 < 0)
//                 continue;

//             // Interpolált z
//             float z = w0 * s0.z + w1 * s1.z + w2 * s2.z;
//             int pixelIndex = y * width_ + x;

//             if (z < depthBuffer[pixelIndex]) {
//                 depthBuffer[pixelIndex] = z;
//                 framebuffer_[pixelIndex] = color_;
//             }
//         }
//     }
// }


// void Renderer::DrawTriangleFilled(uint32_t* framebuffer_, int width_, int height_,
//             const math::Vec3& s0, const math::Vec3& s1, const math::Vec3& s2, uint32_t color_) 
// {
//     float minX = std::min({s0.x, s1.x, s2.x});
//     float maxX = std::max({s0.x, s1.x, s2.x});
//     float minY = std::min({s0.y, s1.y, s2.y});
//     float maxY = std::max({s0.y, s1.y, s2.y});

//     // bounds checking
//     minX = std::max(minX, 0.0f);
//     minY = std::max(minY, 0.0f);
//     maxX = std::min(maxX, float(width_ - 1));
//     maxY = std::min(maxY, float(height_ - 1));

//     float denom = (s1.y - s2.y)*(s0.x - s2.x) + (s2.x - s1.x)*(s0.y - s2.y);
//     if (fabs(denom) < 1e-8f) return; // deformed triangle

//     for (int y = int(minY); y <= int(maxY); ++y) {
//         for (int x = int(minX); x <= int(maxX); ++x) {
//             // Barycentric coordinates
//             float w0 = ((s1.y - s2.y)*(x - s2.x) + (s2.x - s1.x)*(y - s2.y)) / denom;
//             float w1 = ((s2.y - s0.y)*(x - s2.x) + (s0.x - s2.x)*(y - s2.y)) / denom;
//             float w2 = 1.0f - w0 - w1;

//             const float eps = -0.0001f;
//             if (w0 < eps || w1 < eps || w2 < eps) continue;

//             // z-interpolation
//             float z = w0 * s0.z + w1 * s1.z + w2 * s2.z;
            
//             int pixelIndex = y * width + x;

//             //  DEBUG
//             // if (x % 50 == 0 && y % 50 == 0) { // ritkán logoljon
//             //     std::cout << "Pixel (" << x << "," << y << ") z=" << z 
//             //             << " depth=" << depthBuffer[pixelIndex] << std::endl;
//             // }
//             // -----------

//             if (z < depthBuffer[pixelIndex]) {
//                 depthBuffer[pixelIndex] = z;
//                 framebuffer_[pixelIndex] = color_;
//             }
//         }
//     }
// }

bool Renderer::DrawModel(core::Model& model_) {
    currentModel = &model_;
    
    if (!currentModel) return false;
    bool drewAny = false;

    for (const auto& face : currentModel->faces) {
        const std::vector<int>& idx = face.vertexIndices;
        size_t n = idx.size();
        if (n < 3) continue;

        for (size_t i = 1; i + 1 < n; ++i) {
            int32_t index0 = idx[0];
            int32_t index1 = idx[i];
            int32_t index2 = idx[i + 1];

            if ((size_t)index0 >= currentModel->vertices.size() ||
                (size_t)index1 >= currentModel->vertices.size() ||
                (size_t)index2 >= currentModel->vertices.size()) {
                continue;
            }

            const core::Vertex& cv0 = currentModel->vertices[index0];
            const core::Vertex& cv1 = currentModel->vertices[index1];
            const core::Vertex& cv2 = currentModel->vertices[index2];

            Vec3 v0(cv0.x, cv0.y, cv0.z);
            Vec3 v1(cv1.x, cv1.y, cv1.z);
            Vec3 v2(cv2.x, cv2.y, cv2.z);

            // Model space -> World space
            Vec3 v0m = (modelMatrix * Vec4(v0, 1.0f)).ToVec3();
            Vec3 v1m = (modelMatrix * Vec4(v1, 1.0f)).ToVec3();
            Vec3 v2m = (modelMatrix * Vec4(v2, 1.0f)).ToVec3();
            
            // View space
            Vec3 v0v = (viewMatrix * Vec4(v0m, 1.0f)).ToVec3();
            Vec3 v1v = (viewMatrix * Vec4(v1m, 1.0f)).ToVec3();
            Vec3 v2v = (viewMatrix * Vec4(v2m, 1.0f)).ToVec3();
            // Vec3 v0v = (viewMatrix * math::Vec4(v0, 1.0f)).ToVec3();
            // Vec3 v1v = (viewMatrix * math::Vec4(v1, 1.0f)).ToVec3();
            // Vec3 v2v = (viewMatrix * math::Vec4(v2, 1.0f)).ToVec3();
            
            // Face normal
            Vec3 a = v1m - v0m;
            Vec3 b = v2m - v0m;
            Vec3 normal = Vec3::Cross(a, b).Normalized();
            
            // normal.z <= 0
            // if (Vec3::Dot(normal, (-v0v)) < 0)
            //     normal = -normal;
            
            // Back-face culling (negative z culling)
            Vec3 toCam = (-v0v).Normalized();
            //Vec3 toCam = (camera->position - v0).Normalized();
            float facing = Vec3::Dot(normal, toCam);
            const float BACKFACE_EPSILON = 1e-3f;
            if (facing >= BACKFACE_EPSILON) {
                // std::cout << "Normal (culled): " << normal.x << ", " << normal.y << ", " << normal.z << " | Face: " << facing << "\n";
                continue;
            } 
            // else {
            //     std::cout << "NOPENOPE: " << normal.x << ", " << normal.y << ", " << normal.z << " | Face: " << facing << "\n";
            // }

            // Projection
            Vec3 proj0 = camera->ProjectPoint(v0, viewMatrix, projectionMatrix);
            Vec3 proj1 = camera->ProjectPoint(v1, viewMatrix, projectionMatrix);
            Vec3 proj2 = camera->ProjectPoint(v2, viewMatrix, projectionMatrix);

            Vec3 s0 = math::Vec3::ToScreen(proj0, width, height);
            Vec3 s1 = math::Vec3::ToScreen(proj1, width, height);
            Vec3 s2 = math::Vec3::ToScreen(proj2, width, height);

            // float cross = (s1.x - s0.x)*(s2.y - s0.y) - (s1.y - s0.y)*(s2.x - s0.x);
            // std::cout << "Cross: " << cross << "\n";
            // if (cross <= 0) {
            //     continue;
            // } // back-face  

            // Screen bounds check
            if ((s0.x < 0 && s1.x < 0 && s2.x < 0) ||
                (s0.x >= width && s1.x >= width && s2.x >= width) ||
                (s0.y < 0 && s1.y < 0 && s2.y < 0) ||
                (s0.y >= height && s1.y >= height && s2.y >= height)) {
                std::cout << "Out of screen.\n";
                continue;
            }

            // Fill triangle
            DrawTriangleFilled(frameBuffer, width, height, s0, s1, s2, color);
            drewAny = true;
        }
    }

    return drewAny;
}

void Renderer::Clear(uint32_t clear_color) {
    if (frameBuffer == nullptr) return;
    
    if (width <= 0 || height <= 0) return;

    size_t pixels = static_cast<size_t>(width) * static_cast<size_t>(height);

    std::fill_n(frameBuffer, pixels, clear_color);
    std::fill(depthBuffer.begin(), depthBuffer.end(), 1.0f);
}