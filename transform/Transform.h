#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../core/Model.h"
#include "../math/mat4.h"
#include "../math/vec3.h"

namespace transform
{
    class Transform
    {
    public:
        static std::vector<math::Vec3> Apply(const core::Model& model, const math::mat4& model_matrix) {
            std::vector<math::Vec3> transformed;
            // reserve the vector in memory (no realloc needed)
            transformed.reserve(model.vertices.size());

            for (auto& v : model.vertices) {
                math::Vec3 vec(v.x, v.y, v.z);
                transformed.push_back(model_matrix * vec);
            }

            return transformed;
        }
        
        static std::vector<math::Vec3> ApplyTRS(
            const core::Model& model,
            const math::Vec3& position,
            const math::Vec3& rotation,
            const math::Vec3& scale)
        {

            math::mat4 modelMat = math::mat4().TRS(position, rotation, scale);

            return Apply(model, modelMat);
        }
    };

} // namespace transform


#endif // TRANSFORM_H