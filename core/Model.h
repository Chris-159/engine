#ifndef MODEL_H
#define MODEL_H

#include<vector>
#include<string>
#include<unordered_map>

#include "Vertex.h"
#include "Face.h"

#include "../misc/material.h"

namespace core
{
    struct Model
    {
        std::vector<Vertex> vertices;
        std::vector<Face> faces;

        struct TexCoord { float u, v; };
        std::vector<TexCoord> texcoords;

        misc::Material material;

        std::vector<math::Vec3> normals;

        std::unordered_map<std::string, misc::Material> materials;
        std::vector<std::string> faceMaterial;

        void Clear() {
            vertices.clear();
            faces.clear();
            texcoords.clear();
            materials.clear();
        }

        [[nodiscard]] size_t VertexCount() const { return vertices.size(); }
        [[nodiscard]] size_t FaceCount() const { return faces.size(); }
    };
} // namespace core

#endif // MODEL_H