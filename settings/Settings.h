#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

#include<cstdint>

namespace settings
{
    enum class ProjectionMode {
        FastNDC,       // affine (fast)
        AccurateClipW  // fast projection (slow)
    };

    class Settings
    {
    public:
        ProjectionMode projectionMode = ProjectionMode::AccurateClipW;

        static Settings& Get() {
            static Settings instance_;
            return instance_;
        }

        private:
            Settings() = default;
    };
} // namespace settings


#endif // SETTINGS_H