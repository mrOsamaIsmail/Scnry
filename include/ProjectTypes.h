#pragma once
#include <ScnryTypes.h>   // reuses AssetType enum
#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <Scnry.hpp>
namespace Scnry {
struct ProjectSceneEntry {
    std::string  Name;
    std::string  Path;          // relative path to .scene file
    uint64_t     UUID    = 0;
    bool         Startup = false;
};

struct ProjectAssetEntry {
    uint64_t     UUID = 0;
    NodeType     Type = NodeType::EMPTY;
    std::string  Path;          // path on disk
    std::string  Name;          // display name
};

struct ProjectData {
    std::string                     Name;
    Version                         Version;        
    uint64_t                        StartupSceneUUID = 0;
    std::vector<ProjectSceneEntry>  Scenes;
    std::vector<ProjectAssetEntry>  Assets;

    const ProjectSceneEntry* FindScene(uint64_t uuid) const {
        for (const auto& s : Scenes)
            if (s.UUID == uuid) return &s;
        return nullptr;
    }

    const ProjectSceneEntry* GetStartupScene() const {
        return FindScene(StartupSceneUUID);
    }

    const ProjectAssetEntry* FindAsset(uint64_t uuid) const {
        for (const auto& a : Assets)
            if (a.UUID == uuid) return &a;
        return nullptr;
    }

    const ProjectAssetEntry* FindAssetByName(const std::string& name) const {
        for (const auto& a : Assets)
            if (a.Name == name) return &a;
        return nullptr;
    }

    const void AddSceneEntry(ProjectSceneEntry&& entry) 
    {
        Scenes.emplace_back(entry);
    }

    ProjectData& operator=(const ProjectData& other) {
        if (this != &other) {
            Name = other.Name;
            Version = other.Version;
            StartupSceneUUID = other.StartupSceneUUID;
            Scenes = other.Scenes;
            Assets = other.Assets;
        }
        return *this;
    }

    ProjectData& operator=(const std::optional<ProjectData>& optional) {
        if (optional.has_value()) {
            return *this = optional.value();
        }
        return *this;
    }

    ProjectData& assign(const std::optional<ProjectData>& optional) {
        return *this = optional;
    }

    bool operator!() const {
        return Name.empty() && Scenes.empty() && Assets.empty();
    }
};

};