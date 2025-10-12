#ifndef FACE_H
#define FACE_H

#include<vector>

namespace core
{
    struct Face
    {
        std::vector<int> vertexIndices;
        
        Face() = default;
        explicit Face(const std::vector<int>& indices)
            : vertexIndices(indices) {}
    };
} // namespace face

#endif // FACE_H