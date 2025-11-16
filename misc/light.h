#ifndef LIGHT_H
#define LIGHT_H

#include "../math/vec3.h"

namespace misc
{
    struct Light
    {
        math::Vec3 direction;
        float intensity;
        float ambient;
    };    
} // namespace misc

#endif // LIGHT_H