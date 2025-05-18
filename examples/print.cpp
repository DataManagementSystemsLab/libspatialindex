#include <spatialindex/SpatialIndex.h>
#include <iostream>
#include <string>
#include <spatialindex/rtree/RTree.h>
#include "json.hpp"

using json = nlohmann::json;
using namespace SpatialIndex;
using namespace std;

class MyVisitor : public IVisitor {
public:
    json result = json::array();

    void visitNode(const INode& n) override {
        json nodeJson;
        nodeJson["type"] = "node";
        nodeJson["level"]=n.getLevel();
        nodeJson["id"] = n.getIdentifier();
        nodeJson["is_index"] = n.isIndex();
        nodeJson["is_leaf"] = n.isLeaf();

        IShape* shape;
        n.getShape(&shape);
        Region* r = dynamic_cast<Region*>(shape);
        if (r != nullptr) {
            nodeJson["rectangle"] = {
                {"low", {r->m_pLow[0], r->m_pLow[1]}},
                {"high", {r->m_pHigh[0], r->m_pHigh[1]}}
            };
        }

        json children = json::array();
        for (uint32_t i = 0; i < n.getChildrenCount(); ++i) {
            children.push_back(n.getChildIdentifier(i));
        }
        nodeJson["children"] = children;

        result.push_back(nodeJson);
        delete shape;
    }

    void visitData(const IData& d) override {
        json dataJson;
        dataJson["type"] = "data";
        dataJson["id"] = d.getIdentifier();

        IShape* shape;
        d.getShape(&shape);
        Region* r = dynamic_cast<Region*>(shape);
        if (r != nullptr) {
            dataJson["rectangle"] = {
                {"low", {r->m_pLow[0], r->m_pLow[1]}},
                {"high", {r->m_pHigh[0], r->m_pHigh[1]}}
            };
        }

        result.push_back(dataJson);
        delete shape;
    }

    void visitData(std::vector<const IData*>&) override {}
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <rtree_file>" << endl;
        return 1;
    }

    string rtreeFile = argv[1];
    IStorageManager* diskStorage = StorageManager::loadDiskStorageManager(rtreeFile);
    id_type indexIdentifier = 1;
    ISpatialIndex* tree = RTree::loadRTree(*diskStorage, indexIdentifier);

    double queryLow[2] = { std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest() };
    double queryHigh[2] = { std::numeric_limits<double>::max(), std::numeric_limits<double>::max() };
    Region queryRegion(queryLow, queryHigh, 2);

    MyVisitor visitor;
    tree->intersectsWithQuery(queryRegion, visitor);

    // Output the final JSON
    cout << visitor.result.dump(2) << endl;

    delete tree;
    delete diskStorage;

    return 0;
}
