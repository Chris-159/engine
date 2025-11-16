#include "MaterialLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace loader;

std::unordered_map<std::string, misc::Material> MaterialLoader::LoadMTL(const std::string& path)
{
    std::unordered_map<std::string, misc::Material> materials;
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open MTL file: " << path << std::endl;
        return materials;
    }

    std::string line;
    std::string currentName;
    misc::Material currentMat;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key == "newmtl")
        {
            if (!currentName.empty()) {
                materials[currentName] = currentMat;
            }
            iss >> currentName;
            currentMat = misc::Material(); // reset
        }
        else if (key == "Ka") // Ambient color
            iss >> currentMat.ambient.x >> currentMat.ambient.y >> currentMat.ambient.z;
        else if (key == "Kd") // Diffuse color
            iss >> currentMat.color.x >> currentMat.color.y >> currentMat.color.z;
        else if (key == "Ks") // Specular color
            iss >> currentMat.specular.x >> currentMat.specular.y >> currentMat.specular.z;
        else if (key == "Ke") // Emissive color
            iss >> currentMat.emissive.x >> currentMat.emissive.y >> currentMat.emissive.z;
        else if (key == "Ns") // Shininess
            iss >> currentMat.shininess;
        else if (key == "map_Kd") // Diffuse texture map
            iss >> currentMat.texturePath;
    }

    if (!currentName.empty()) {
        materials[currentName] = currentMat;
    }

    return materials;
}
