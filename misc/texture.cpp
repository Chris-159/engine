#include "Texture.h"
#include <cmath>

namespace misc
{
    uint32_t Texture::Sample(float u, float v) const
    {
        if (data.empty()) return 0xFFFFFFFF;
        
        u = std::fmod(std::fmod(u, 1.0f) + 1.0f, 1.0f);
        v = std::fmod(std::fmod(v, 1.0f) + 1.0f, 1.0f);

        int x = int(u * (width  - 1) + 0.5f);
        int y = int((1.0f - v) * (height - 1) + 0.5f); 

        // clamp (safety)
        if (x < 0) x = 0; if (x >= width) x = width - 1;
        if (y < 0) y = 0; if (y >= height) y = height - 1;

        int base = (y * width + x) * channels;

        uint8_t r = 0, g = 0, b = 0, a = 255;
        if (channels >= 1) r = data[base + 0];
        if (channels >= 2) g = data[base + 1];
        if (channels >= 3) b = data[base + 2];
        if (channels >= 4) a = data[base + 3];

        return (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
    }
}