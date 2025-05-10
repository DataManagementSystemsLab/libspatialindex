#include <spatialindex/SpatialIndex.h>
#include <iostream>

using namespace SpatialIndex;
using namespace std;

class MyVisitor : public IVisitor {
public:
    void visitNode(const INode& n) override {}

    void visitData(const IData& d) override {
        IShape* shape;
        d.getShape(&shape);
        Region* r = dynamic_cast<Region*>(shape);
        if (r != nullptr) {
            cout << "Found rectangle: [(" 
                 << r->m_pLow[0] << ", " << r->m_pLow[1] << "), (" 
                 << r->m_pHigh[0] << ", " << r->m_pHigh[1] << ")]" << endl;
        }
        delete shape;
    }

    void visitData(std::vector<const IData*>& v) override {}
};

int main() {
    // Create a memory-based storage manager
    IStorageManager* memoryStorage = StorageManager::createNewMemoryStorageManager();

    // Create the R-tree index
    id_type indexIdentifier;
    ISpatialIndex* tree = RTree::createNewRTree(*memoryStorage, 0.7, 100, 100, 2,
                                                 RTree::RV_RSTAR, indexIdentifier);

    // Insert 5 rectangular regions
    for (int i = 0; i < 5; i++) {
        double low[2] = { double(i), double(i) };
        double high[2] = { double(i + 1), double(i + 1) };
        Region r(low, high, 2);
        tree->insertData(0, nullptr, r, i);  // ID = i
    }

    // Query the R-tree with a region that intersects some of the above
    double queryLow[2] = { 1.0, 1.0 };
    double queryHigh[2] = { 3.0, 3.0 };
    Region queryRegion(queryLow, queryHigh, 2);

    MyVisitor visitor;
    tree->intersectsWithQuery(queryRegion, visitor);

    delete tree;
    delete memoryStorage;

    return 0;
}
