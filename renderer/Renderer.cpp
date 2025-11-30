#include<iostream>

#include "Renderer.h"
#include "../settings/Settings.h"

using namespace renderer;
using namespace math;
using namespace settings;

//  CONSTRUCTOR
Renderer::Renderer(uint32_t* framebuffer_, int32_t width_, int32_t height_) :
        frameBuffer(framebuffer_), width(width_), height(height_),
        color(0xFFFFFFFF), camera(nullptr), currentModel(nullptr),
        modelMatrix(math::mat4::Identity()) 
        {
            depthBuffer.resize(width * height, 1.0f);
        }
// -----------------------------


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



// void Renderer::DrawTriangleFilled(
//     uint32_t* framebuffer_, int width_, int height_,
//     const math::Vec3& s0, const math::Vec3& s1, const math::Vec3& s2,
//     uint32_t color_,
//     const misc::Texture* texture,
//     const math::Vec3* uv0, const math::Vec3* uv1, const math::Vec3* uv2,
//     const float* recipW0, const float* recipW1, const float* recipW2,
//     bool perspectiveCorrect
// ) {
//     float minX = std::min({ s0.x, s1.x, s2.x });
//     float maxX = std::max({ s0.x, s1.x, s2.x });
//     float minY = std::min({ s0.y, s1.y, s2.y });
//     float maxY = std::max({ s0.y, s1.y, s2.y });

//     minX = std::max(minX, 0.0f);
//     minY = std::max(minY, 0.0f);
//     maxX = std::min(maxX, float(width_ - 1));
//     maxY = std::min(maxY, float(height_ - 1));

//     for (int y = int(minY); y <= int(maxY); ++y) {
//         for (int x = int(minX); x <= int(maxX); ++x) {

//             float u, v, w;
//             ComputeBarycentric(float(x) + 0.5f, float(y) + 0.5f, s0, s1, s2, u, v, w);

//             const float eps = -0.0001f;
//             if (u < eps || v < eps || w < eps) continue;

//             float z = u * s0.z + v * s1.z + w * s2.z;
//             int pixelIndex = y * width_ + x;

//             if (z < depthBuffer[pixelIndex]) {

//                 uint32_t outColor = color_;

//                 if (texture && uv0 && uv1 && uv2) {
//                     if (!perspectiveCorrect || !(recipW0 && recipW1 && recipW2))
//                     {
//                         // AFFINE
//                         float U = u * uv0->x + v * uv1->x + w * uv2->x;
//                         float V = u * uv0->y + v * uv1->y + w * uv2->y;
//                         outColor = texture->Sample(U, V);
//                     }
//                     else
//                     {
//                         // perspective
//                         float oneOverW =
//                             u * (*recipW0) + v * (*recipW1) + w * (*recipW2);

//                         float U_over_W =
//                             u * (uv0->x * (*recipW0)) +
//                             v * (uv1->x * (*recipW1)) +
//                             w * (uv2->x * (*recipW2));

//                         float V_over_W =
//                             u * (uv0->y * (*recipW0)) +
//                             v * (uv1->y * (*recipW1)) +
//                             w * (uv2->y * (*recipW2));

//                         float U = U_over_W / oneOverW;
//                         float V = V_over_W / oneOverW;

//                         outColor = texture->Sample(U, V);
//                     }
//                 }

//                 depthBuffer[pixelIndex] = z;
//                 framebuffer_[pixelIndex] = outColor;
//             }
//         }
//     }
// }

// void Renderer::DrawTriangleFilled(
//             uint32_t* framebuffer_, int width_, int height_,
//             const math::Vec3& s0_, const math::Vec3& s1_, const math::Vec3& s2_,
//             uint32_t color_,
//             const misc::Texture* texture_,
//             const math::Vec3* uv0_,
//             const math::Vec3* uv1_,
//             const math::Vec3* uv2_,
//             const float* recipW0_,
//             const float* recipW1_,
//             const float* recipW2_,
//             const math::Vec3* n0_,
//             const math::Vec3* n1_,
//             const math::Vec3* n2_,
//             const math::Vec3* viewPos0_,
//             const math::Vec3* viewPos1_,
//             const math::Vec3* viewPos2_,
//             bool perspectiveCorrect_
// ) {
//     float minX = std::min({ s0_.x, s1_.x, s2_.x });
//     float maxX = std::max({ s0_.x, s1_.x, s2_.x });
//     float minY = std::min({ s0_.y, s1_.y, s2_.y });
//     float maxY = std::max({ s0_.y, s1_.y, s2_.y });

//     minX = std::max(minX, 0.0f);
//     minY = std::max(minY, 0.0f);
//     maxX = std::min(maxX, float(width_ - 1));
//     maxY = std::min(maxY, float(height_ - 1));

//     // whether we have normals/view positions for per-pixel lighting
//     bool hasVertexNormals = (n0_ && n1_ && n2_);
//     bool hasViewPos = (viewPos0_ && viewPos1_ && viewPos2_);

//     for (int y = int(minY); y <= int(maxY); ++y) {
//         for (int x = int(minX); x <= int(maxX); ++x) {

//             // sample at pixel center
//             float u, v, w;
//             ComputeBarycentric(float(x) + 0.5f, float(y) + 0.5f, s0_, s1_, s2_, u, v, w);

//             const float eps = -0.0001f;
//             if (u < eps || v < eps || w < eps) continue;

//             // interpolated z (screen space) for depth test
//             float z = u * s0_.z + v * s1_.z + w * s2_.z;
//             int pixelIndex = y * width_ + x;

//             if (z >= depthBuffer[pixelIndex]) continue;

//             uint32_t outColor = color_;

//             // TEXTURE sampling (same as before) - perspective correct if requested
//             if (texture_ && uv0_ && uv1_ && uv2_) {
//                 if (!perspectiveCorrect_ || !(recipW0_ && recipW1_ && recipW2_)) {
//                     // affine interpolate UVs
//                     float U = u * uv0_->x + v * uv1_->x + w * uv2_->x;
//                     float V = u * uv0_->y + v * uv1_->y + w * uv2_->y;
//                     outColor = texture_->Sample(U, V);
//                 } else {
//                     float oneOverW =
//                         u * (*recipW0_) + v * (*recipW1_) + w * (*recipW2_);

//                     float U_over_W =
//                         u * (uv0_->x * (*recipW0_)) +
//                         v * (uv1_->x * (*recipW1_)) +
//                         w * (uv2_->x * (*recipW2_));

//                     float V_over_W =
//                         u * (uv0_->y * (*recipW0_)) +
//                         v * (uv1_->y * (*recipW1_)) +
//                         w * (uv2_->y * (*recipW2_));

//                     float U = U_over_W / oneOverW;
//                     float V = V_over_W / oneOverW;

//                     outColor = texture_->Sample(U, V);
//                 }
//             }

//             // If we have per-vertex normals and/or view positions, interpolate them and compute lighting per-pixel.
//             if (hasVertexNormals || hasViewPos) {
//                 math::Vec3 interpNormal(0,0,0);
//                 math::Vec3 interpViewPos(0,0,0);

//                 if (!perspectiveCorrect_ || !(recipW0_ && recipW1_ && recipW2_)) {
//                     // AFFINE interpolation (no /w)
//                     if (hasVertexNormals) {
//                         interpNormal = (*n0_) * u + (*n1_) * v + (*n2_) * w;
//                         interpNormal = interpNormal.Normalized();
//                     }
//                     if (hasViewPos) {
//                         interpViewPos = (*viewPos0_) * u + (*viewPos1_) * v + (*viewPos2_) * w;
//                     }
//                 } else {
//                     // PERSPECTIVE-CORRECT interpolation for vectors/positions:
//                     float oneOverW =
//                         u * (*recipW0_) + v * (*recipW1_) + w * (*recipW2_);

//                     if (hasVertexNormals) {
//                         float nx_over_w =
//                             u * (n0_->x * (*recipW0_)) +
//                             v * (n1_->x * (*recipW1_)) +
//                             w * (n2_->x * (*recipW2_));
//                         float ny_over_w =
//                             u * (n0_->y * (*recipW0_)) +
//                             v * (n1_->y * (*recipW1_)) +
//                             w * (n2_->y * (*recipW2_));
//                         float nz_over_w =
//                             u * (n0_->z * (*recipW0_)) +
//                             v * (n1_->z * (*recipW1_)) +
//                             w * (n2_->z * (*recipW2_));

//                         interpNormal = math::Vec3(nx_over_w / oneOverW,
//                                                   ny_over_w / oneOverW,
//                                                   nz_over_w / oneOverW).Normalized();
//                     }

//                     if (hasViewPos) {
//                         float vx_over_w =
//                             u * (viewPos0_->x * (*recipW0_)) +
//                             v * (viewPos1_->x * (*recipW1_)) +
//                             w * (viewPos2_->x * (*recipW2_));
//                         float vy_over_w =
//                             u * (viewPos0_->y * (*recipW0_)) +
//                             v * (viewPos1_->y * (*recipW1_)) +
//                             w * (viewPos2_->y * (*recipW2_));
//                         float vz_over_w =
//                             u * (viewPos0_->z * (*recipW0_)) +
//                             v * (viewPos1_->z * (*recipW1_)) +
//                             w * (viewPos2_->z * (*recipW2_));

//                         interpViewPos = math::Vec3(vx_over_w / oneOverW,
//                                                    vy_over_w / oneOverW,
//                                                    vz_over_w / oneOverW);
//                     }
//                 }

//                 // If we have interpolated a normal and a view position (or at least assume camera at origin in view space),
//                 // compute per-pixel lighting:
//                 if (hasVertexNormals) {
//                     // view direction: from fragment toward camera (camera is at origin in view-space)
//                     math::Vec3 viewDir;
//                     if (hasViewPos) {
//                         viewDir = (-interpViewPos).Normalized(); // camera at (0,0,0)
//                     } else {
//                         // fallback: approximate using screen-space - use triangle average direction (not ideal but safe)
//                         viewDir = math::Vec3(0,0,1);
//                     }

//                     // Apply per-pixel lighting using interpolated normal and viewDir.
//                     outColor = ApplyLight(currentModel ? currentModel->material : misc::Material(), mainLight, interpNormal, viewDir);

//                     // If we also had a texture sample, we can modulate texture by lighting:
//                     if (texture_ && uv0_ && uv1_ && uv2_) {
//                         // combine shaded lighting (outColor) with sampled texture color (outColorTex)
//                         // outColor currently has lighting from material color; but the previous texture sample wrote into outColor.
//                         // To keep consistent: compute shadedColor from material (as above) and multiply with texture sample.
//                         // We'll extract the shaded color and sampled color, then modulate.
//                         // Extract shadedColor components:
//                         uint32_t sampled = texture_->Sample(0.0f, 0.0f); // placeholder, but we already have texture sampled earlier in outColor
//                         // The code path above overwrote outColor with texture sample in either case.
//                         // To correctly combine: do the lighting computation into a local variable and combine with texture sample.
//                         // (So reorganize slightly:)
//                     }
//                 }
//             }

//             // Better approach: compute lighting into shadedColor, then if texture present multiply.
//             // To keep code clearer, redo per-pixel shading here with proper separation:

//             // ---- recompute shadedColor properly ----
//             uint32_t finalColor = outColor; // fallback

//             // If we have normals, compute shadedColor from material using interpNormal/viewDir
//             if (hasVertexNormals) {
//                 // compute interpNormal and interpViewPos again (we can refactor to avoid double work; left verbose for clarity)
//                 math::Vec3 interpNormal2(0,0,0);
//                 math::Vec3 interpViewPos2(0,0,0);

//                 if (!perspectiveCorrect_ || !(recipW0_ && recipW1_ && recipW2_)) {
//                     interpNormal2 = (*n0_) * u + (*n1_) * v + (*n2_) * w;
//                     interpNormal2 = interpNormal2.Normalized();
//                     if (hasViewPos) interpViewPos2 = (*viewPos0_) * u + (*viewPos1_) * v + (*viewPos2_) * w;
//                 } else {
//                     float oneOverW =
//                         u * (*recipW0_) + v * (*recipW1_) + w * (*recipW2_);

//                     interpNormal2 = math::Vec3(
//                         (u * (n0_->x * (*recipW0_)) + v * (n1_->x * (*recipW1_)) + w * (n2_->x * (*recipW2_))) / oneOverW,
//                         (u * (n0_->y * (*recipW0_)) + v * (n1_->y * (*recipW1_)) + w * (n2_->y * (*recipW2_))) / oneOverW,
//                         (u * (n0_->z * (*recipW0_)) + v * (n1_->z * (*recipW1_)) + w * (n2_->z * (*recipW2_))) / oneOverW
//                     ).Normalized();

//                     if (hasViewPos) {
//                         interpViewPos2 = math::Vec3(
//                             (u * (viewPos0_->x * (*recipW0_)) + v * (viewPos1_->x * (*recipW1_)) + w * (viewPos2_->x * (*recipW2_))) / oneOverW,
//                             (u * (viewPos0_->y * (*recipW0_)) + v * (viewPos1_->y * (*recipW1_)) + w * (viewPos2_->y * (*recipW2_))) / oneOverW,
//                             (u * (viewPos0_->z * (*recipW0_)) + v * (viewPos1_->z * (*recipW1_)) + w * (viewPos2_->z * (*recipW2_))) / oneOverW
//                         );
//                     }
//                 }

//                 math::Vec3 viewDir2 = hasViewPos ? (-interpViewPos2).Normalized() : math::Vec3(0,0,1);

//                 // shaded color based on material of currentModel (fallback if null)
//                 const misc::Material& matRef = currentModel ? ( (currentModel->faces.size()>0) ? *(&currentModel->material) : currentModel->material ) : misc::Material();
//                 uint32_t shaded = ApplyLight(matRef, mainLight, interpNormal2, viewDir2);

//                 if (texture_ && uv0_ && uv1_ && uv2_) {
//                     // we already computed a sampled texture color earlier into 'outColor' (maybe overwritten)
//                     // recompute sampled texture color properly:
//                     uint32_t sampledColor;
//                     if (!perspectiveCorrect_ || !(recipW0_ && recipW1_ && recipW2_)) {
//                         float U = u * uv0_->x + v * uv1_->x + w * uv2_->x;
//                         float V = u * uv0_->y + v * uv1_->y + w * uv2_->y;
//                         sampledColor = texture_->Sample(U, V);
//                     } else {
//                         float oneOverW =
//                             u * (*recipW0_) + v * (*recipW1_) + w * (*recipW2_);

//                         float U_over_W =
//                             u * (uv0_->x * (*recipW0_)) +
//                             v * (uv1_->x * (*recipW1_)) +
//                             w * (uv2_->x * (*recipW2_));

//                         float V_over_W =
//                             u * (uv0_->y * (*recipW0_)) +
//                             v * (uv1_->y * (*recipW1_)) +
//                             w * (uv2_->y * (*recipW2_));

//                         float U = U_over_W / oneOverW;
//                         float V = V_over_W / oneOverW;
//                         sampledColor = texture_->Sample(U, V);
//                     }

//                     // multiply sampledColor (texture) with shaded (lighting). We'll do component-wise multiply (modulate).
//                     uint8_t sr = (sampledColor >> 16) & 0xFF;
//                     uint8_t sg = (sampledColor >> 8) & 0xFF;
//                     uint8_t sb = (sampledColor) & 0xFF;

//                     uint8_t lr = (shaded >> 16) & 0xFF;
//                     uint8_t lg = (shaded >> 8) & 0xFF;
//                     uint8_t lb = (shaded) & 0xFF;

//                     uint8_t fr = uint8_t( (int(sr) * int(lr)) / 255 );
//                     uint8_t fg = uint8_t( (int(sg) * int(lg)) / 255 );
//                     uint8_t fb = uint8_t( (int(sb) * int(lb)) / 255 );

//                     finalColor = (0xFFu << 24) | (fr << 16) | (fg << 8) | fb;
//                 } else {
//                     // no texture, just shaded color
//                     finalColor = shaded;
//                 }
//             } else {
//                 // no vertex normals: keep previous behavior (flat shading as passed via color_ or earlier outColor)
//                 finalColor = outColor;
//             }

//             // write depth + color
//             depthBuffer[pixelIndex] = z;
//             framebuffer_[pixelIndex] = finalColor;
//         }
//     }    
// }

void Renderer::DrawTriangleFilled(
            uint32_t* framebuffer_, int width_, int height_,
            const math::Vec3& s0_, const math::Vec3& s1_, const math::Vec3& s2_,
            uint32_t color_,
            const misc::Texture* texture_,
            const misc::Material* material_,
            const math::Vec3* uv0_,
            const math::Vec3* uv1_,
            const math::Vec3* uv2_,
            const float* recipW0_,
            const float* recipW1_,
            const float* recipW2_,
            const math::Vec3* n0_,
            const math::Vec3* n1_,
            const math::Vec3* n2_,
            const math::Vec3* viewPos0_,
            const math::Vec3* viewPos1_,
            const math::Vec3* viewPos2_,
            bool perspectiveCorrect_
) {
    float minX = std::min({ s0_.x, s1_.x, s2_.x });
    float maxX = std::max({ s0_.x, s1_.x, s2_.x });
    float minY = std::min({ s0_.y, s1_.y, s2_.y });
    float maxY = std::max({ s0_.y, s1_.y, s2_.y });

    minX = std::max(minX, 0.0f);
    minY = std::max(minY, 0.0f);
    maxX = std::min(maxX, float(width_ - 1));
    maxY = std::min(maxY, float(height_ - 1));

    // whether we have normals/view positions for per-pixel lighting
    bool hasVertexNormals = (n0_ && n1_ && n2_);
    bool hasViewPos = (viewPos0_ && viewPos1_ && viewPos2_);

    for (int y = int(minY); y <= int(maxY); ++y) {
        for (int x = int(minX); x <= int(maxX); ++x) {

            // sample at pixel center
            float u, v, w;
            ComputeBarycentric(float(x) + 0.5f, float(y) + 0.5f, s0_, s1_, s2_, u, v, w);

            const float eps = -0.0001f;
            if (u < eps || v < eps || w < eps) continue;

            // interpolated z (screen space) for depth test
            float z = u * s0_.z + v * s1_.z + w * s2_.z;
            int pixelIndex = y * width_ + x;

            if (z >= depthBuffer[pixelIndex]) continue;

            // uint32_t outColor = color_;

            // ---- recompute shadedColor properly ----
            // 1. interpolate normal
            math::Vec3 interpNormal(0,0,0);
            math::Vec3 interpViewPos(0,0,0);
            uint32_t finalColor = color_; // outColor

            if (hasVertexNormals) {
                if (!perspectiveCorrect_ || !(recipW0_ && recipW1_ && recipW2_)) {
                    // AFFINE
                    interpNormal = (*n0_) * u + (*n1_) * v + (*n2_) * w;
                    interpNormal = interpNormal.Normalized();
                    
                    if (hasViewPos)
                        interpViewPos = (*viewPos0_) * u +
                                        (*viewPos1_) * v +
                                        (*viewPos2_) * w;
                } else {
                    // PERSPECTIVE CORRECT
                    float oneOverW =
                        u * (*recipW0_) + v * (*recipW1_) + w * (*recipW2_);

                    interpNormal = math::Vec3(
                        (u * (n0_->x * (*recipW0_)) +
                         v * (n1_->x * (*recipW1_)) +
                         w * (n2_->x * (*recipW2_))) / oneOverW,
                        
                        (u * (n0_->y * (*recipW0_)) +
                         v * (n1_->y * (*recipW1_)) +
                         w * (n2_->y * (*recipW2_))) / oneOverW,
                        
                        (u * (n0_->z * (*recipW0_)) +
                         v * (n1_->z * (*recipW1_)) +
                         w * (n2_->z * (*recipW2_))) / oneOverW
                    ).Normalized();

                    if (hasViewPos) {
                        interpViewPos = math::Vec3(
                            (u * (viewPos0_->x * (*recipW0_)) +
                             v * (viewPos1_->x * (*recipW1_)) +
                             w * (viewPos2_->x * (*recipW2_))) / oneOverW,
                            
                            (u * (viewPos0_->y * (*recipW0_)) +
                             v * (viewPos1_->y * (*recipW1_)) +
                             w * (viewPos2_->y * (*recipW2_))) / oneOverW,
                            
                             (u * (viewPos0_->z * (*recipW0_)) +
                              v * (viewPos1_->z * (*recipW1_)) +
                              w * (viewPos2_->z * (*recipW2_))) / oneOverW
                        );
                    }
                }
                // --------------------------------------

                // 2. Compute view direction
                math::Vec3 viewDir = hasViewPos ? (-interpViewPos).Normalized() : math::Vec3(0,0,1);
                // -------------------------
                //const misc::Material& matRef = currentModel ? ((currentModel->faces.size()>0) ? *(&currentModel->material) : currentModel->material) : misc::Material();
                //uint32_t shaded = ApplyLight(matRef, mainLight, interpNormal2, viewDir2);

                // 3. Apply material-based lighting
                uint32_t litColor = 0xFFFFFFFF;
                if (material_ && hasVertexNormals) {
                    litColor = ApplyLight(*material_, mainLight, interpNormal, viewDir);
                } else {
                    litColor = color_;   // fallback
                }
                // ----------------------

                // 4. Sample texture (if exists)
                finalColor = litColor;
                if (texture_ && uv0_ && uv1_ && uv2_) {
                    //uint32_t sampledColor;
                    float U, V;

                    if (!perspectiveCorrect_ || !(recipW0_ && recipW1_ && recipW2_)) {
                        U = u * uv0_->x + v * uv1_->x + w * uv2_->x;
                        V = u * uv0_->y + v * uv1_->y + w * uv2_->y;
                        //sampledColor = texture_->Sample(U, V);
                    } else {
                        float oneOverW =
                            u * (*recipW0_) + v * (*recipW1_) + w * (*recipW2_);

                        float U_over_W =
                            u * (uv0_->x * (*recipW0_)) +
                            v * (uv1_->x * (*recipW1_)) +
                            w * (uv2_->x * (*recipW2_));

                        float V_over_W =
                            u * (uv0_->y * (*recipW0_)) +
                            v * (uv1_->y * (*recipW1_)) +
                            w * (uv2_->y * (*recipW2_));

                        U = U_over_W / oneOverW;
                        V = V_over_W / oneOverW;
                    }

                    uint32_t texColor = texture_->Sample(U, V);

                    uint8_t sr = (texColor >> 16) & 0xFF;
                    uint8_t sg = (texColor >> 8)  & 0xFF;
                    uint8_t sb = (texColor)       & 0xFF;

                    uint8_t lr = (litColor >> 16) & 0xFF;
                    uint8_t lg = (litColor >> 8)  & 0xFF;
                    uint8_t lb = (litColor)       & 0xFF;

                    uint8_t fr = uint8_t((int(sr) * int(lr)) / 255);
                    uint8_t fg = uint8_t((int(sg) * int(lg)) / 255);
                    uint8_t fb = uint8_t((int(sb) * int(lb)) / 255);

                    finalColor = (0xFFu << 24) | (fr << 16) | (fg << 8) | fb;
                } 
            } 
            // ----------------------

            // write depth + color
            depthBuffer[pixelIndex] = z;
            framebuffer_[pixelIndex] = finalColor;
        }
    }
}    

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
            float clipW0, clipW1, clipW2; 

            Vec3 proj0 = camera->ProjectPoint(v0, viewMatrix, projectionMatrix, clipW0);
            Vec3 proj1 = camera->ProjectPoint(v1, viewMatrix, projectionMatrix, clipW1);
            Vec3 proj2 = camera->ProjectPoint(v2, viewMatrix, projectionMatrix, clipW2);

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
            
            Vec3 vn0_world(0, 0, 0), vn1_world(0, 0, 0), vn2_world(0, 0, 0);
            bool haveNormals = false;
            if (!face.normalIndices.empty() &&
                face.normalIndices.size() >= 3 &&
                !currentModel->normals.empty())
            {
                int ni0 = face.normalIndices[0];
                int ni1 = face.normalIndices[i];
                int ni2 = face.normalIndices[i + 1];

                if (ni0 >= 0 && ni1 >= 0 && ni2 >= 0 &&
                    ni0 < currentModel->normals.size() &&
                    ni1 < currentModel->normals.size() &&
                    ni2 < currentModel->normals.size())
                {
                    Vec4 t0 = modelMatrix * Vec4(currentModel->normals[ni0], 0.0f);
                    Vec4 t1 = modelMatrix * Vec4(currentModel->normals[ni1], 0.0f);
                    Vec4 t2 = modelMatrix * Vec4(currentModel->normals[ni2], 0.0f);

                    vn0_world = Vec3(t0.x, t0.y, t0.z).Normalized();
                    vn1_world = Vec3(t1.x, t1.y, t1.z).Normalized();
                    vn2_world = Vec3(t2.x, t2.y, t2.z).Normalized();

                    haveNormals = true;
                }
            }

            // Fallback: if no pre-vertex normals, use face normals (flat)
            if (!haveNormals) {
                vn0_world = vn1_world = vn2_world = normal;
                haveNormals = true;
            }
            
            Vec3 viewPos0 = v0v;
            Vec3 viewPos1 = v1v;
            Vec3 viewPos2 = v2v;

            // 8. Perspective correct (recipW)
            bool usePerspective = (Settings::Get().projectionMode == ProjectionMode::AccurateClipW);

            float recipW0 = 0.0f, recipW1 = 0.0f, recipW2 = 0.0f;
            float* pRecipW0 = nullptr;
            float* pRecipW1 = nullptr;
            float* pRecipW2 = nullptr;

            if (usePerspective) {
                if (fabs(clipW0) > 1e-8f) recipW0 = 1.0f / clipW0;   
                if (fabs(clipW1) > 1e-8f) recipW1 = 1.0f / clipW1;   
                if (fabs(clipW2) > 1e-8f) recipW2 = 1.0f / clipW2;
                
                pRecipW0 = &recipW0;
                pRecipW1 = &recipW1;
                pRecipW2 = &recipW2;
            }
            // ------------

            const misc::Material* matPtr = mat;

            // 9. Draw the triangle (textured version)
            DrawTriangleFilled(frameBuffer, 
                width, 
                height, 
                s0, s1, s2, 
                0xFFFFFFFF,
                texture,
                matPtr, 
                uv0, uv1, uv2,
                pRecipW0, pRecipW1, pRecipW2,
                &vn0_world, &vn1_world, &vn2_world,
                &viewPos0, &viewPos1, &viewPos2,
                usePerspective);

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
