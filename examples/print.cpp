#include <spatialindex/SpatialIndex.h>
#include <iostream>
#include <string>
#include <spatialindex/rtree/RTree.h>
using namespace SpatialIndex;
using namespace std;

class MyVisitor : public IVisitor {
public:
    void visitNode(const INode& n) override {
        cout << "Node: " << n.getIdentifier()  <<" " << n.isIndex() <<" "<< n.isLeaf() ;
        
        
        IShape* shape;
        n.getShape(&shape);
        Region* r = dynamic_cast<Region*>(shape);
        if (r != nullptr) {
            cout << "Node Rectangle: [(" 
                 << r->m_pLow[0] << ", " << r->m_pLow[1] << "), (" 
                 << r->m_pHigh[0] << ", " << r->m_pHigh[1] << ")]" << endl;
        }
        cout << "Children: " << n.getChildrenCount() << endl;
        for (uint32_t i = 0; i < n.getChildrenCount(); ++i) {
            id_type childId = n.getChildIdentifier(i);
            cout << "Child ID: " << childId << endl;
        }
        delete shape;
    }

    void visitData(const IData& d) override {
        cout << "Data Node: " << d.getIdentifier() ;
        IShape* shape;
        d.getShape(&shape);
        Region* r = dynamic_cast<Region*>(shape);
        if (r != nullptr) {
            cout << "Data Rectangle: [(" 
                 << r->m_pLow[0] << ", " << r->m_pLow[1] << "), (" 
                 << r->m_pHigh[0] << ", " << r->m_pHigh[1] << ")]" << endl;
        }
        delete shape;
    }

    void visitData(std::vector<const IData*>& v) override {}
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <rtree_file>" << endl;
        return 1;
    }

    // Open the R-tree file
    string rtreeFile = argv[1];
    IStorageManager* diskStorage = StorageManager::loadDiskStorageManager(rtreeFile);

    // Load the R-tree index
    id_type indexIdentifier = 1; // Assuming the index identifier is 1
    ISpatialIndex* tree = RTree::loadRTree(*diskStorage, indexIdentifier);


    //tree->getStatistics(cout);
    cout << (*tree) << endl;
    // Query the entire R-tree
    double queryLow[2] = { std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest() };
    double queryHigh[2] = { std::numeric_limits<double>::max(), std::numeric_limits<double>::max() };
    Region queryRegion(queryLow, queryHigh, 2);

    MyVisitor visitor;
    tree->intersectsWithQuery(queryRegion, visitor);


    cout << (*tree) << endl;
    // Clean up
    delete tree;
    delete diskStorage;

    return 0;
}