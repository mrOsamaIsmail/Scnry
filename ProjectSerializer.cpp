#include <ProjectSerializer.h>
#include <yaml-cpp/yaml.h>
#include <Scnry.hpp>
#include <fstream>
#include <iostream>

namespace Scnry {
    // ─────────────────────────────────────────────────────────────
    //  Load
    // ─────────────────────────────────────────────────────────────
    std::optional<ProjectData> ProjectSerializer::Load(const std::string& filepath) {
        YAML::Node root;
        try {
            root = YAML::LoadFile(filepath);
        }
        catch (const YAML::Exception& e) {
            std::cerr << "[ProjectSerializer] Parse error: " << e.what() << "\n";
            return std::nullopt;
        }

        const YAML::Node& proj = root["Project"];
        if (!proj) {
            std::cerr << "[ProjectSerializer] Missing 'Project' root key.\n";
            return std::nullopt;
        }

        ProjectData data;

        // ── Name & Version ────────────────────────────────────────
        data.Name = proj["Name"].as<std::string>("Unnamed");

        if (const auto& ver = proj["Version"]) {
            data.Version.maj = ver["Maj"].as<uint32_t>(1);
            data.Version.min = ver["Min"].as<uint32_t>(0);
        }

        // ── Startup scene UUID ────────────────────────────────────
        data.StartupSceneUUID = proj["StartupScene"].as<uint64_t>(0);

        // ── Scenes ────────────────────────────────────────────────
        if (const auto& scenes = proj["Scenes"]) {
            for (const auto& s : scenes) {
                ProjectSceneEntry entry;
                entry.Name = s["Name"].as<std::string>("Unnamed");
                entry.Path = s["Path"].as<std::string>("");
                entry.UUID = s["UUID"].as<uint64_t>(0);
                entry.Startup = s["Startup"].as<bool>(false);
                data.Scenes.push_back(entry);
            }
        }

        // ── Assets ────────────────────────────────────────────────
        if (const auto& assets = proj["Assets"]) {
            for (const auto& a : assets) {
                ProjectAssetEntry entry;
                entry.UUID = a["UUID"].as<uint64_t>(0);
                entry.Type = static_cast<NodeType>(a["Type"].as<uint32_t>(0));
                entry.Path = a["Path"].as<std::string>("");
                entry.Name = a["Name"].as<std::string>("");
                data.Assets.push_back(entry);
            }
        }

        return data;
    }

    // ─────────────────────────────────────────────────────────────
    //  Save
    // ─────────────────────────────────────────────────────────────
    bool ProjectSerializer::Save(const ProjectData& project, const std::string& filepath) {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Project" << YAML::Value << YAML::BeginMap;

        // ── Name & Version ────────────────────────────────────────
        out << YAML::Key << "Name" << YAML::Value << project.Name;
        out << YAML::Key << "Version" << YAML::Value << YAML::BeginMap
            << YAML::Key << "Maj" << YAML::Value << project.Version.maj
            << YAML::Key << "Min" << YAML::Value << project.Version.min
            << YAML::EndMap;

        // ── Startup scene UUID ────────────────────────────────────
        out << YAML::Key << "StartupScene" << YAML::Value << project.StartupSceneUUID;

        // ── Scenes ────────────────────────────────────────────────
        out << YAML::Key << "Scenes" << YAML::Value << YAML::BeginSeq;
        for (const auto& s : project.Scenes) {
            out << YAML::BeginMap
                << YAML::Key << "Name" << YAML::Value << s.Name
                << YAML::Key << "Path" << YAML::Value << s.Path
                << YAML::Key << "UUID" << YAML::Value << s.UUID
                << YAML::Key << "Startup" << YAML::Value << s.Startup
                << YAML::EndMap;
        }
        out << YAML::EndSeq;

        // ── Assets ────────────────────────────────────────────────
        out << YAML::Key << "Assets" << YAML::Value << YAML::BeginSeq;
        for (const auto& a : project.Assets) {
            out << YAML::BeginMap
                << YAML::Key << "UUID" << YAML::Value << a.UUID
                << YAML::Key << "Type" << YAML::Value << static_cast<uint32_t>(a.Type)
                << YAML::Key << "Path" << YAML::Value << a.Path
                << YAML::Key << "Name" << YAML::Value << a.Name
                << YAML::EndMap;
        }
        out << YAML::EndSeq;

        out << YAML::EndMap;  // Project
        out << YAML::EndMap;  // root

        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "[ProjectSerializer] Cannot write: " << filepath << "\n";
            return false;
        }
        file << out.c_str();
        return true;
    }
};