#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <string>
#include "../core/Model.h"

namespace loader {
    class ModelLoader {
        public:
            core::Model LoadObj(const std::string& path);
            // ModelS LoadSTL(const std::string& path);
            
        private:
            bool ParseVertex(const std::string& line, core::Model& model);
            bool ParseFace(const std::string& line, core::Model& model);
    };
    
} // namespace loader

#endif // MODEL_LOADER_H