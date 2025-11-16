#ifndef TEXTURE_H
#define TEXTURE_H

#include<vector>
#include<stdint.h>

namespace misc
{
    struct Texture
    {
        int width, height, channels;
        std::vector<uint8_t> data;

        uint32_t Sample(float u, float v) const;
    };
    
} // namespace misc


#endif // TEXTURE_H