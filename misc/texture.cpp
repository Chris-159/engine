#include "Texture.h"
#include <cmath>

namespace misc
{
    uint32_t Texture::Sample(float u, float v) const
    {
        if (data.empty()) return 0xFFFFFFFF;
        
        u = std::fmod(std::fmod(u, 1.0f) + 1.0f, 1.0f);
        v = std::fmod(std::fmod(v, 1.0f) + 1.0f, 1.0f);

        int x = int(u * (width  - 1));
        int y = int((1.0f - v) * (height - 1)); 

        int index = (y * width + x) * channels;

        uint8_t r = data[index + 0];
        uint8_t g = data[index + 1];
        uint8_t b = data[index + 2];
        uint8_t a = data[index + 3];

        return (a << 24) | (r << 16) | (g << 8) | b;
    }
}