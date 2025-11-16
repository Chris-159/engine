#ifndef MATERIAL_H
#define MATERIAL_H

#include "../math/vec3.h"
#include "../misc/texture.h"

namespace misc
{
    struct Material
    {
        math::Vec3 color;           // default color
        
        math::Vec3 ambient;         // Ka - Ambient color
        math::Vec3 diffuse;         // Kd - Diffuse color
        math::Vec3 specular;        // Ks - Specular color
        math::Vec3 emissive;        // Ke - Emissive color

        misc::Texture diffuseMap;
        
        float shininess = 32.0f;    // if not 0, then shiny outline
        std::string texturePath;    // map_Kd

        Material()
            : ambient(0.1f, 0.1f, 0.1f),
              diffuse(1.0f, 1.0f, 1.0f),
              specular(0.0f, 0.0f, 0.0f),
              emissive(0.0f, 0.0f, 0.0f),
              shininess(32.0f) {}
    };
} // namespace misc


#endif // MATERIAL_H