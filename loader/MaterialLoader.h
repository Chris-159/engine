#ifndef MATERIAL_LOADER_H
#define MATERIAL_LOADER_H

#include<string>
#include<unordered_map>

#include "../misc/material.h"

namespace loader
{
    class MaterialLoader
    {
    public:
        static std::unordered_map<std::string, misc::Material> LoadMTL(const std::string& path_);
    };
} // namespace loader


#endif // MATERIAL_LOADER_H