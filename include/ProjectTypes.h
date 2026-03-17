#pragma once
#include <Types.h>   // reuses AssetType enum
#include <string>
#include <vector>
#include <cstdint>
#include <Scnry.hpp>  
namespace Scnry {
// ── Per-scene entry in the project ───────────────────────────
struct ProjectSceneEntry {
    std::string  Name;
    std::string  Path;          // relative path to .scene file
    uint64_t     UUID    = 0;
    bool         Startup = false;
};

// ── Shared asset entry (global asset registry) ───────────────
struct ProjectAssetEntry {
    uint64_t     UUID = 0;
    NodeType     Type = NodeType::EMPTY;
    std::string  Path;          // path on disk
    std::string  Name;          // display name
};

// ── Top-level project data ────────────────────────────────────
struct ProjectData {
    std::string                     Name;
    Version                         Version;        // reuse from SceneTypes
    uint64_t                        StartupSceneUUID = 0;
    std::vector<ProjectSceneEntry>  Scenes;
    std::vector<ProjectAssetEntry>  Assets;

    // Convenience lookups
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
};

};