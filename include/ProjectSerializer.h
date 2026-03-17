#pragma once
#include <ProjectTypes.h>
#include <string>
#include <optional>

namespace Scnry {
    class ProjectSerializer {
    public:
        /// Load a .project file → ProjectData
        static std::optional<ProjectData> Load(const std::string& filepath);

        /// Save ProjectData → .project file
        static bool Save(const ProjectData& project, const std::string& filepath);
    };
}