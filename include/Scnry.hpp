#ifndef SCENARY_H
#define SCENARY_H

#include <Types.h>
#include <memory>

//#include <yaml-cpp/yaml.h>
namespace Scnry {




// Add custom node types here
enum class UIType{
    BTN,
    TEXT,
    ICON
};
enum class NodeType
{
    EMPTY = 0,
    SKELETAL_MESH = 1,
    STATIC_MESH = 2,
    RIGID_BODY = 3,
    CAMERA = 4,
    VOLUME = 5,
    SCRIPT = 6,
    PARTICLE_EFFECT = 7,
    UI = 8,
    NUM_NODE_TYPES = 9,
    UUID = 10
};
enum class LoadState{
    SUCCESS,
    FAIL
};


class Version 
{
public:
    Version();
    Version(int maj,int min);
    int min;
    int maj;
};
class Node;
class Scene{
    public:
    list<Node> SceneNodes; 
    
    Scene(string SceneName = "SCEmpty");
   
    string Name;
    Version Version;
    uint Index;
    float LastEdit;
    
};

class ISerializable {
public:
    virtual string& GetItemPath() const = 0;
    virtual Node& GetItemTransform() const = 0;
};

// Every scene node inherits from this class 
class Node
{
public:
    string Name;
    NodeType Type;
    int Parent;
    Array<float, 16> TransformMatrix;
    unsigned long long AssetID;
public:
    Node();
    Node(const char* name, NodeType&& type, Array<float, 16>&& nodeMatrix_16,unsigned long id);
    Node(string&& name, NodeType&& type, Array<float, 16>&& nodeMatrix_16);
    bool operator ==(const Node& other) const;
    bool operator !=(const Node& other) const;
};
//dictionary (list) for each node type to pass a loader callback
class Impl;
class Scnry
{
public:
        
        //static Scnry instance;
        static list<string> Scenes;
        static Scene CurrentLoaded;
        static string CurrentPath;
        //LoaderFunctions
        static dictionary<NodeType , bool(*)(Node&)> NodeLoaders;
        static bool Init();
        static LoadState RemoveItemFromScene(ISerializable const& Item);
        static LoadState LoadScene(const char* ScenePath);
        static LoadState LoadScene(uint SceneIndex);
        static LoadState AddItemToScene(ISerializable const& Item);
        static LoadState AddNode(Node&& Item,int Patrnt = -1);
        static LoadState RemoveNode(Node Item);
        static LoadState SaveScene(string SaveToPath = "SAME");
public:
        
        static std::shared_ptr<Impl> implementation;
};
};
#endif