#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include<string>
#include "../misc/texture.h"

namespace loader
{
    class ImageLoader
    {
    public:
        static misc::Texture Load(const std::string& path_);
    
    private:
        static misc::Texture LoadBMP(const std::string& path_);
        static misc::Texture LoadTGA(const std::string& path_);
        static misc::Texture LoadDDS(const std::string& path_);
        
    };
} // namespace loader


#endif // IMAGE_LOADER_H