#ifndef MODEL_H
#define MODEL_H

#include<vector>
#include<string>
#include "Vertex.h"
#include "Face.h"

namespace core
{
    struct Model
    {
        std::vector<Vertex> vertices;
        std::vector<Face> faces;

        void Clear() {
            vertices.clear();
            faces.clear();
        }

        [[nodiscard]] size_t VertexCount() const { return vertices.size(); }
        [[nodiscard]] size_t FaceCount() const { return faces.size(); }
    };
} // namespace core

#endif // MODEL_H