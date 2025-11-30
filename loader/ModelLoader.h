#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <string>
#include "../core/Model.h"

namespace loader {
    class ModelLoader {
        public:
            core::Model LoadObj(const std::string& path_);
            // ModelS LoadSTL(const std::string& path);

            static inline void TrimString(std::string &str_);
        
        private:
            bool ParseVertex(const std::string& line_, core::Model& model_);
            bool ParseFace(const std::string& line_, const std::string& materialName_, core::Model& model_);
            bool ParseTexCoord(const std::string& line_, core::Model& model_);
            bool ParseNormal(const std::string& line_, core::Model& model_);
        };
    
} // namespace loader

#endif // MODEL_LOADER_H