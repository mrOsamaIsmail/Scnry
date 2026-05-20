
#include <yaml-cpp/yaml.h>
#include <Scnry.hpp>
#include <fstream>
#include <chrono>


using Scene = Scnry::Scene;
using LoadState = Scnry::LoadState;

class Scnry::Impl
{
public:
    //YAML::Node Node;
    Impl() {}

    /*operator YAML::Node()const
    {
        return this->Node;
    }*/

   /* void SetNode(const YAML::Node& node)
    {
        this->Node = node;
    }*/
    //definitions are in the bottom

};

namespace YAML
{
    template<>
    struct convert<Scnry::Version> 
    {
        static Node encode(const Scnry::Version& input) 
        {
            Node node;
            node["Maj"] = input.maj;
            node["Min"] = input.min;
            return node;
        }
        static bool decode(const Node& node, Scnry::Version& output) 
        {
            output.maj = node["Maj"].as<int>();
            output.min = node["Min"].as<int>();
            return true;
        }
    };
    template <>
    struct convert<Scnry::Node> {
        static Node encode(const Scnry::Node& node)
        {
            Node _node;
            _node["Name"] = node.Name;
            _node["Parent"] = node.Parent;
            _node["NodeType"] = (int)node.Type;
            _node["Transform"] = node.TransformMatrix;
            _node["Transform"].SetStyle(YAML::EmitterStyle::Flow);
            _node["AssetID"] = node.AssetID;
            return _node;
        }
        static bool decode(const Node& node, Scnry::Node& SceneNode)
        {
            SceneNode.Name = node["Name"].as<string>();
            SceneNode.Parent = node["Parent"].as<int>();
            SceneNode.Type = static_cast<Scnry::NodeType>(node["NodeType"].as<int>());
            SceneNode.TransformMatrix = node["Transform"].as<std::array<float, 16>>();
            SceneNode.AssetID = node["AssetID"].as<uint64_t>();
            return true;
        }


    };
    template<>
    struct convert<Scnry::Scene>
    {
        static Node encode(const Scnry::Scene& scene) 
        {
            Node sceneNode;
            sceneNode["SceneName"] = scene.Name;
            sceneNode["Version"] = scene.Version;
            sceneNode["SceneNodes"] = YAML::Node(YAML::NodeType::Sequence);
            
            for (const auto& node : scene.SceneNodes) {
                sceneNode["SceneNodes"].push_back(node);
            }
            return sceneNode;
        }
        static bool decode(const Node& node, Scnry::Scene& output) 
        {
            output.Name = node["SceneName"].as<string>();
            output.Version = node["Version"].as<Scnry::Version>();
            list<Scnry::Node> nodes;
            YAML::Node SCNNodes = node["SceneNodes"];
            Scnry::Node iterNode;
            for (size_t i = 0; i < SCNNodes.size(); i++)
            {
                Node& current = SCNNodes[i];
                //iterNode.Name = current[""]

                iterNode.Name = current["Name"].as<string>();
                iterNode.Type = static_cast<Scnry::NodeType>(current["NodeType"].as<int>());
                iterNode.Parent = current["Parent"].as<int>();
                iterNode.TransformMatrix = current["Transform"].as<Array<float, 16>>();
                iterNode.AssetID = current["AssetID"].as<uint64_t>();
                output.SceneNodes.push_back(iterNode);
            }
            return true;
        }
    };
}




std::shared_ptr<Scnry::Impl> Scnry::Scnry::implementation;
dictionary<Scnry::NodeType, bool(*)(Scnry::Node&)> Scnry::Scnry::NodeLoaders;
Scnry::Scene Scnry::Scnry::CurrentLoaded;
string Scnry::Scnry::CurrentPath;
bool Scnry::Scnry::Init() 
{
    //Scnry::Scnry::NodeLoaders() //= dictionary<Scnry::NodeType, bool(*)(Scnry::Node&, const string&)>();
    Scnry::Scnry::CurrentLoaded = Scene();
    Scnry::implementation = std::make_shared<Impl>();
    return true;
}

Scnry::Version::Version(int _maj, int _min): maj(maj), min(_min){}
Scnry::Version::Version(): maj(0), min(0){}

Scnry::Scene::Scene(string SceneName) :
    Name(SceneName),
    SceneNodes(),
    Index(0),
    LastEdit(0.0f)
{}
Scnry::Node::Node(const char* name, NodeType&& type, Array<float,16>&& nodeMatrix_16,uint64_t ID)
:
    Name(string(name)),
    Type(type),
    TransformMatrix(nodeMatrix_16),
    Parent(-1),
    AssetID(ID)
{}
Scnry::Node::Node(string&& name, NodeType&& type, Array<float,16>&& nodeMatrix_16)
:
    Name(string(name)),
    Type(type),
    TransformMatrix(nodeMatrix_16),
    Parent(-1)
{}
Scnry::Node::Node():
    Name(""),
    Type(NodeType::EMPTY),
    TransformMatrix(),
    Parent(-1)
{}
bool Scnry::Node::operator ==(const Node& other) const
{
    if (other.Name == Name && other.Parent == Parent && other.Type == Type && other.TransformMatrix == TransformMatrix)
        return true;
    else
        return false;
}
bool Scnry::Node::operator !=(const Node& other) const
{
    if (other.Name == Name && other.Parent == Parent && other.Type == Type && other.TransformMatrix == TransformMatrix)
        return false;
    else
        return true;
}

Scnry::LoadState Scnry::Scnry::LoadScene(const char* ScenePath)
{
    string path = string(ScenePath);
    Scnry::CurrentPath = path;

    YAML::Node file = YAML::LoadFile(path);
    Scene currentScene;
    currentScene = file["Scene"].as<Scene>();
    
    for (Node& nodeRef : currentScene.SceneNodes)
    {
        //check to see if there is a loader for said node type
        auto it = Scnry::Scnry::NodeLoaders.find(nodeRef.Type);
        if(it != Scnry::NodeLoaders.end())
            Scnry::Scnry::NodeLoaders[nodeRef.Type]((nodeRef));
    }
    Scnry::CurrentLoaded = currentScene;
    return LoadState::SUCCESS;
}

Scnry::LoadState Scnry::Scnry::LoadScene(uint SceneIndex)
{
    //Scnry::implementation = new Impl();
    return LoadState::SUCCESS;
}
Scnry::LoadState Scnry::Scnry::SaveScene(string SaveToPath)
{
    YAML::Node node;
    node["Scene"] = Scnry::CurrentLoaded;
    YAML::Emitter out;

    out << node;//(YAML::Node)Scnry::Scnry::implementation;

    if (!out.good()) {
#ifdef DEBUG

        std::cerr << "YAML Emission failed: " << out.GetLastError() << "\n";
#endif // DEBUG

        return LoadState::FAIL;
    }
    if (SaveToPath.compare(string("SAME"))==0)
    {

        std::ofstream fileOut(Scnry::CurrentPath);
        fileOut << out.c_str();
        fileOut.close();
    }
    else 
    {
        std::ofstream fileOut(SaveToPath);
        fileOut << out.c_str();
        fileOut.close();
    }
    return LoadState::SUCCESS;
}

Scnry::LoadState Scnry::Scnry::AddItemToScene(ISerializable const& Item)
{
    

    //((YAML::Node)Scnry::Scnry::implementation)["SceneNodes"].push_back(newNode);
    //auto& nodesNode = file["SceneNodes"];
    return LoadState::SUCCESS;
}

Scnry::LoadState Scnry::Scnry::RemoveItemFromScene(ISerializable const& Item)
{

    return LoadState::SUCCESS;
}

Scnry::LoadState Scnry::Scnry::AddNode(Node&& Item, int Patrnt) 
{
    Scnry::CurrentLoaded.SceneNodes.push_back(Item);
    return LoadState::SUCCESS;
}

Scnry::LoadState Scnry::Scnry::RemoveNode(Node Item)
{
    Scnry::CurrentLoaded.SceneNodes.remove(Item);
    return LoadState::SUCCESS;
}
