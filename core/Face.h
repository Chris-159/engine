#ifndef FACE_H
#define FACE_H

#include<vector>

namespace core
{
    struct Face
    {
        std::vector<int> vertexIndices;
        std::vector<int> texcoordIndices;
        std::vector<int> normalIndices;
        std::string materialName;         // face material
        
        Face() = default;
        explicit Face(const std::vector<int>& indices, const std::string& matName = "")
            : vertexIndices(indices), materialName(matName) {}
    };
} // namespace face

#endif // FACE_H