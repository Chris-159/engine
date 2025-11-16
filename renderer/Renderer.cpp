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

void Renderer::SetLight(const misc::Light& light_) {
    mainLight = light_;
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

void Renderer::ComputeBarycentric(float px_, float py_, 
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

void Renderer::DrawTriangleFilled(
    uint32_t* framebuffer_, int width_, int height_,
    const math::Vec3& s0, const math::Vec3& s1, const math::Vec3& s2,
    uint32_t color_,
    const misc::Texture* texture,
    const math::Vec3* uv0, const math::Vec3* uv1, const math::Vec3* uv2,
    const float* recipW0, const float* recipW1, const float* recipW2,
    bool perspectiveCorrect
) {
    float minX = std::min({ s0.x, s1.x, s2.x });
    float maxX = std::max({ s0.x, s1.x, s2.x });
    float minY = std::min({ s0.y, s1.y, s2.y });
    float maxY = std::max({ s0.y, s1.y, s2.y });

    minX = std::max(minX, 0.0f);
    minY = std::max(minY, 0.0f);
    maxX = std::min(maxX, float(width_ - 1));
    maxY = std::min(maxY, float(height_ - 1));

    for (int y = int(minY); y <= int(maxY); ++y) {
        for (int x = int(minX); x <= int(maxX); ++x) {

            float u, v, w;
            ComputeBarycentric((float)x, (float)y, s0, s1, s2, u, v, w);

            const float eps = -0.0001f;
            if (u < eps || v < eps || w < eps) continue;

            float z = u * s0.z + v * s1.z + w * s2.z;
            int pixelIndex = y * width_ + x;

            if (z < depthBuffer[pixelIndex]) {

                uint32_t outColor = color_;

                if (texture && uv0 && uv1 && uv2) {
                    if (!perspectiveCorrect || !(recipW0 && recipW1 && recipW2))
                    {
                        // AFFINE
                        float U = u * uv0->x + v * uv1->x + w * uv2->x;
                        float V = u * uv0->y + v * uv1->y + w * uv2->y;
                        outColor = texture->Sample(U, V);
                    }
                    else
                    {
                        // perspective
                        float oneOverW =
                            u * (*recipW0) + v * (*recipW1) + w * (*recipW2);

                        float U_over_W =
                            u * (uv0->x * (*recipW0)) +
                            v * (uv1->x * (*recipW1)) +
                            w * (uv2->x * (*recipW2));

                        float V_over_W =
                            u * (uv0->y * (*recipW0)) +
                            v * (uv1->y * (*recipW1)) +
                            w * (uv2->y * (*recipW2));

                        float U = U_over_W / oneOverW;
                        float V = V_over_W / oneOverW;

                        outColor = texture->Sample(U, V);
                    }
                }

                depthBuffer[pixelIndex] = z - 1e-5f;
                framebuffer_[pixelIndex] = outColor;
            }
        }
    }
}

// void Renderer::DrawTriangleFilled(uint32_t* framebuffer_, int width_, int height_,
//                                   const math::Vec3& s0, const math::Vec3& s1, const math::Vec3& s2, uint32_t color_)
// {
//     float minX = std::min({ s0.x, s1.x, s2.x });
//     float maxX = std::max({ s0.x, s1.x, s2.x });
//     float minY = std::min({ s0.y, s1.y, s2.y });
//     float maxY = std::max({ s0.y, s1.y, s2.y });

//     // bounds checking (clamp)
//     minX = std::max(minX, 0.0f);
//     minY = std::max(minY, 0.0f);
//     maxX = std::min(maxX, float(width_ - 1));
//     maxY = std::min(maxY, float(height_ - 1));

//     // Precompute edges
//     math::Vec3 v0 = s1 - s0;
//     math::Vec3 v1 = s2 - s0;

//     float d00 = math::Vec3::Dot(v0, v0);
//     float d01 = math::Vec3::Dot(v0, v1);
//     float d11 = math::Vec3::Dot(v1, v1);
//     float denom = d00 * d11 - d01 * d01;
//     if (fabs(denom) < 1e-8f) return; // Degenerate triangle

//     // Main rasterization loop
//     for (int y = int(minY); y <= int(maxY); ++y) {
//         for (int x = int(minX); x <= int(maxX); ++x) {
//             math::Vec3 v2((float)x - s0.x, (float)y - s0.y, 0.0f);

//             float d20 = math::Vec3::Dot(v2, v0);
//             float d21 = math::Vec3::Dot(v2, v1);

//             float v = (d11 * d20 - d01 * d21) / denom;
//             float w = (d00 * d21 - d01 * d20) / denom;
//             float u = 1.0f - v - w;

//             const float eps = -0.0001f;
//             if (u < eps || v < eps || w < eps)
//                 continue;

//             float z = u * s0.z + v * s1.z + w * s2.z;
//             //float depth = (z * 0.5f) + 0.5f;

//             int pixelIndex = y * width_ + x;

//             // if (y % 100 == 0 && x % 100 == 0)
//             //     std::cout << "z=" << z << " depth=" << depth
//             //             << " buf=" << depthBuffer[pixelIndex] << std::endl;

//             if (z < depthBuffer[pixelIndex]) {
//                 depthBuffer[pixelIndex] = z - 1e-5f; // depth - 1e-4f
//                 framebuffer_[pixelIndex] = color_;
//             }
//         }
//     }
// }

// uint32_t Renderer::ApplyLight(uint32_t color_, float intensity_) {
//     intensity_ = std::clamp(intensity_, 0.0f, 1.0f);

//     // get each color value separately
//     uint8_t r = (color_ >> 16) & 0xFF;
//     uint8_t g = (color_ >> 8)  & 0xFF;
//     uint8_t b = (color_)       & 0xFF;

//     // apply color inetnsity to every chanel
//     r = uint8_t(r * intensity_);
//     g = uint8_t(g * intensity_);
//     b = uint8_t(b * intensity_);

//     // reassemble the color
//     return (r << 16) | (g << 8) | b;
// }

// uint32_t Renderer::ApplyLight(
//     const misc::Material& material,
//     const misc::Light& light,
//     const math::Vec3& normal,
//     const math::Vec3& viewDir
// ) {
//     // Diffuse (Lambert)
//     float diff = std::max(0.0f, math::Vec3::Dot(normal, -light.direction));

//     // Specular (Phong)
//     math::Vec3 reflectDir = Reflect(light.direction, normal);
//     float spec = 0.0f;
//     if (material.specular > 0.0f) {
//         spec = powf(std::max(math::Vec3::Dot(viewDir, reflectDir), 0.0f), material.shininess);
//     }

//     // ambient + összegzés
//     float total = 
//         material.ambient * light.ambient +
//         material.diffuse * diff * light.intensity +
//         material.specular * spec * light.intensity;

//     total = std::clamp(total, 0.0f, 1.0f);

//     // calculate color
//     math::Vec3 baseColor = material.color * total + material.emissive;
//     baseColor.x = std::min(baseColor.x, 1.0f);
//     baseColor.y = std::min(baseColor.y, 1.0f);
//     baseColor.z = std::min(baseColor.z, 1.0f);

//     uint8_t r = uint8_t(baseColor.x * 255);
//     uint8_t g = uint8_t(baseColor.y * 255);
//     uint8_t b = uint8_t(baseColor.z * 255);

//     return (r << 16) | (g << 8) | b;
// }

uint32_t Renderer::ApplyLight(
    const misc::Material& material,
    const misc::Light& light,
    const math::Vec3& normal,
    const math::Vec3& viewDir
) {
    // Diffuse (Lambert)
    float diff = std::max(0.0f, math::Vec3::Dot(normal, -light.direction));

    // Specular (Phong)
    math::Vec3 reflectDir = Reflect(light.direction, normal);
    float spec = powf(std::max(math::Vec3::Dot(viewDir, reflectDir), 0.0f), material.shininess);

    // Color blending per component
    math::Vec3 color =
        (material.ambient * light.ambient) +
        (material.diffuse * diff * light.intensity) +
        (material.specular * spec * light.intensity) +
        material.emissive;

    color.x = std::clamp(color.x, 0.0f, 1.0f);
    color.y = std::clamp(color.y, 0.0f, 1.0f);
    color.z = std::clamp(color.z, 0.0f, 1.0f);

    uint8_t r = uint8_t(color.x * 255);
    uint8_t g = uint8_t(color.y * 255);
    uint8_t b = uint8_t(color.z * 255);

    return (r << 16) | (g << 8) | b;
}

bool Renderer::DrawModel(core::Model& model_) {
    currentModel = &model_;
    
    if (!currentModel) return false;
    bool drewAny = false;

    for (const auto& face : currentModel->faces) {
        const std::vector<int>& idx = face.vertexIndices;
        size_t n = idx.size();
        if (n < 3) continue;

        for (size_t i = 1; i + 1 < n; ++i) {

            // 0. Vertex index extraction
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
            // ------------

            // 1. Model -> View transformations
            // Model space -> World space
            Vec3 v0m = (modelMatrix * Vec4(v0, 1.0f)).ToVec3();
            Vec3 v1m = (modelMatrix * Vec4(v1, 1.0f)).ToVec3();
            Vec3 v2m = (modelMatrix * Vec4(v2, 1.0f)).ToVec3();
            
            // View space
            Vec3 v0v = (viewMatrix * Vec4(v0m, 1.0f)).ToVec3();
            Vec3 v1v = (viewMatrix * Vec4(v1m, 1.0f)).ToVec3();
            Vec3 v2v = (viewMatrix * Vec4(v2m, 1.0f)).ToVec3();
            // ------------

            // 2. Face normal + backface culling
            Vec3 a = v1m - v0m;
            Vec3 b = v2m - v0m;
            Vec3 normal = Vec3::Cross(a, b).Normalized();
            Vec3 toCam = (-v0v).Normalized();
            float facing = Vec3::Dot(normal, toCam);
            const float BACKFACE_EPSILON = 1e-3f;
            if (facing >= BACKFACE_EPSILON) continue; 
            // ------------

            // 3. Projection
            Vec3 proj0 = camera->ProjectPoint(v0, viewMatrix, projectionMatrix);
            Vec3 proj1 = camera->ProjectPoint(v1, viewMatrix, projectionMatrix);
            Vec3 proj2 = camera->ProjectPoint(v2, viewMatrix, projectionMatrix);

            Vec3 s0 = math::Vec3::ToScreen(proj0, width, height);
            Vec3 s1 = math::Vec3::ToScreen(proj1, width, height);
            Vec3 s2 = math::Vec3::ToScreen(proj2, width, height);
            // ------------

            // 4. Clipping (Screen bounds check)
            if ((s0.x < 0 && s1.x < 0 && s2.x < 0) ||
                (s0.x >= width && s1.x >= width && s2.x >= width) ||
                (s0.y < 0 && s1.y < 0 && s2.y < 0) ||
                (s0.y >= height && s1.y >= height && s2.y >= height)) {
                std::cout << "Out of screen.\n";
                continue;
            }
            // ------------

            // 5. Get material
            const misc::Material* mat = nullptr;

            if (!face.materialName.empty()) {
                auto it = currentModel->materials.find(face.materialName);
                if (it != currentModel->materials.end()) {
                    mat = &it->second;
                }
            }

            if (!mat) {
                mat = &currentModel->material; // fallback default
            }

            uint32_t shadedColor = ApplyLight(*mat, mainLight, normal, (-v0v).Normalized());
            // ------------

            // 6. Texture pointer
            const misc::Texture* texture = nullptr;

            if (mat && mat->diffuseMap.data.size() > 0) {
                texture = &mat->diffuseMap;
            }
            // ------------

            // 7. Get the UV-s
            const Vec3* uv0 = nullptr;
            const Vec3* uv1 = nullptr;
            const Vec3* uv2 = nullptr;

            static Vec3 tmp0, tmp1, tmp2;

            if (!face.texcoordIndices.empty() &&
                face.texcoordIndices.size() >= 3 &&
                !currentModel->texcoords.empty())
            {
                int ti0 = face.texcoordIndices[0];
                int ti1 = face.texcoordIndices[i];
                int ti2 = face.texcoordIndices[i + 1];

                if (ti0 >= 0 && ti1 >= 0 && ti2 >= 0 &&
                    ti0 < currentModel->texcoords.size() &&
                    ti1 < currentModel->texcoords.size() &&
                    ti2 < currentModel->texcoords.size())
                {
                    const auto& t0 = currentModel->texcoords[ti0];
                    const auto& t1 = currentModel->texcoords[ti1];
                    const auto& t2 = currentModel->texcoords[ti2];

                    tmp0 = Vec3(t0.u, t0.v, 0);
                    tmp1 = Vec3(t1.u, t1.v, 0);
                    tmp2 = Vec3(t2.u, t2.v, 0);

                    uv0 = &tmp0;
                    uv1 = &tmp1;
                    uv2 = &tmp2;
                }
            }
            // ------------

            // 8. Perspective correct (recipW)
            float recipW0 = 1.0f / proj0.z;
            float recipW1 = 1.0f / proj1.z;
            float recipW2 = 1.0f / proj2.z;
            // ------------

            // 9. Draw the triangle (textured version)
            DrawTriangleFilled(frameBuffer, 
                width, 
                height, 
                s0, s1, s2, 
                shadedColor, 
                texture, 
                uv0, uv1, uv2,
                &recipW0, &recipW1, &recipW2,
                false);
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
