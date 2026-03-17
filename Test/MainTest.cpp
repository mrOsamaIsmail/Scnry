#include <Scnry.hpp>
#include <ProjectSerializer.h>
#include <optional>

#define PRINT(x) std::cout << x << std::endl;
#include <iostream>
#include <string>


bool MeshLoader(Scnry::Node& node) 
{
    std::cout << "Loading a mesh" << "\t";
    std::cout << node.Name << "\n";
    return true;
}

bool CameraLoader(Scnry::Node& node)
{
    std::cout << "Loading a Camera" << "\t";
    std::cout << node.Name << "\n";
    return true;
}


int main()
{
    Scnry::Scnry::Init();
    Scnry::Scnry::NodeLoaders[Scnry::NodeType::STATIC_MESH] = MeshLoader;
    Scnry::Scnry::NodeLoaders[Scnry::NodeType::CAMERA] = CameraLoader;

    Scnry::ProjectData data = Scnry::ProjectData();
	data.Name = "MyProject";
    data.Version = Scnry::Version(1, 0);
	data.Assets.push_back({ 12345, Scnry::NodeType::STATIC_MESH, "Assets/Tree.fbx", "Tree" });
	Scnry::ProjectSerializer::Save(data, "MyProject.project");
    //return 0;
    // ── 1. Load project ───────────────────────────────────────
	std::string path = ("MyProject.project");
    auto projResult = Scnry::ProjectSerializer::Load(path);
    if (!projResult) return 1;

    const Scnry::ProjectData& project = *projResult;
    std::cout << "Project: " << project.Name
        << "  v" << project.Version.maj << "." << project.Version.min << "\n\n";

    // ── 2. Print shared asset registry ───────────────────────
    std::cout << "Shared Assets (" << project.Assets.size() << "):\n";
    for (const auto& a : project.Assets)
        std::cout  
        << a.Name << "  uuid=" << a.UUID
        << "  path=" << a.Path << "\n";

    // ── 3. Boot: load startup scene ──────────────────────────
    const Scnry::ProjectSceneEntry* startup = project.GetStartupScene();
    if (!startup) {
        std::cerr << "No startup scene defined!\n";
        return 1;
    }

    std::cout << "\nLoading startup scene: " << startup->Name
        << " (" << startup->Path << ")\n";

   auto sceneResult = Scnry::Scnry::LoadScene(startup->Path.c_str());
    //if (!sceneResult) return 1;

    const auto& scene = Scnry::Scnry::CurrentLoaded;
    std::cout << "  Nodes: " << scene.SceneNodes.size() << "\n";

    // ── 4. Cross-reference: resolve scene AssetIDs → project assets
    std::cout << "\nAsset resolution:\n";
    for (const auto& node : scene.SceneNodes) {
        if (node.AssetID == 0) continue;
        const Scnry::ProjectAssetEntry* asset = project.FindAsset(node.AssetID);
        if (asset)
            std::cout << "  Node '" << node.Name << "' → "
            << asset->Name << " (" << asset->Path << ")\n";
        else
            std::cout << "  Node '" << node.Name
            << "' → UNRESOLVED uuid=" << node.AssetID << "\n";
    }
}


   /*
    Scnry::Scnry::Init();
    Scnry::Scnry::NodeLoaders[Scnry::NodeType::STATIC_MESH] = MeshLoader;
    Scnry::Scnry::NodeLoaders[Scnry::NodeType::CAMERA] = CameraLoader;
    std::string path = ("tempScene.scene");
    Scnry::Scnry::LoadScene("./tempScene.scene");
    //Scnry::Scnry::AddItemToScene();
    Scnry::Node a,b;
    a.Name = ("Shush");
    b.Name = ("DDDD");
    Scnry::Scnry::AddNode(std::forward<Scnry::Node>(b));
    Scnry::Scnry::RemoveNode(a);
    PRINT(Scnry::Scnry::CurrentLoaded.Name);
    PRINT(Scnry::Scnry::CurrentLoaded.SceneNodes.size());
    Scnry::Scnry::SaveScene();
    */
  


