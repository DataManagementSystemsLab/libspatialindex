#include <spatialindex/SpatialIndex.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>

using namespace SpatialIndex;
using namespace std;

struct BoundingBox {
    double min_lon = std::numeric_limits<double>::max();
    double max_lon = std::numeric_limits<double>::lowest();
    double min_lat = std::numeric_limits<double>::max();
    double max_lat = std::numeric_limits<double>::lowest();
};
struct LPoint {
    double lon;
    double lat;
    LPoint(double lon, double lat) {
        this->lon = lon;
        this->lat = lat;
    }
};
vector<BoundingBox> computeBoundingBox(const std::string& linestring, int partition = 1) {
    BoundingBox box;

    size_t start = linestring.find("(");
    size_t end = linestring.find(")");
    if (start == std::string::npos || end == std::string::npos) {
        std::cerr << "Invalid LINESTRING format.\n";
        return vector<BoundingBox>{box};
    }

    std::string coords_str = linestring.substr(start + 1, end - start - 1);
    std::stringstream ss(coords_str);
    std::string coord;
    vector<LPoint> points;
    while (std::getline(ss, coord, ',')) {
        std::stringstream cs(coord);
        double lon, lat;
        cs >> lon >> lat;
        points.push_back(LPoint(lon, lat));
        if (lon < box.min_lon) box.min_lon = lon;
        if (lon > box.max_lon) box.max_lon = lon;
        if (lat < box.min_lat) box.min_lat = lat;
        if (lat > box.max_lat) box.max_lat = lat;
    }
    // Partition points into the specified number of partitions
    size_t num_points = points.size();
    size_t points_per_partition = (num_points + partition - 1) / partition; // Ceiling division
    vector<BoundingBox> partition_boxes;
    for (int i = 0; i < partition; ++i) {
        BoundingBox partition_box;
        size_t start_idx = i * points_per_partition;
        size_t end_idx = std::min(start_idx + points_per_partition, num_points);

        for (size_t j = start_idx; j < end_idx; ++j) {
            const LPoint& p = points[j];
            if (p.lon < partition_box.min_lon) partition_box.min_lon = p.lon;
            if (p.lon > partition_box.max_lon) partition_box.max_lon = p.lon;
            if (p.lat < partition_box.min_lat) partition_box.min_lat = p.lat;
            if (p.lat > partition_box.max_lat) partition_box.max_lat = p.lat;
        }
        partition_boxes.push_back(partition_box);
        // Update the overall bounding box
        cout << "Partition " << i + 1 << ": [(" 
             << partition_box.min_lon << ", " << partition_box.min_lat << "), (" 
             << partition_box.max_lon << ", " << partition_box.max_lat << ")]" << endl;
    }

    return partition_boxes;
}

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

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }

    // Open the input file
    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Error opening file: " << argv[1] << endl;
        return 1;
    }

    // Create a disk-based storage manager
    string outputFile = argv[2];
    IStorageManager* diskStorage = StorageManager::createNewDiskStorageManager(outputFile,4096);

    // Create the R-tree index
    id_type indexIdentifier;
    ISpatialIndex* tree = RTree::createNewRTree(*diskStorage, 0.7, 100, 100, 2,
                                                RTree::RV_RSTAR, indexIdentifier);

    // Read and parse the input file
    string line;
    int line_num = 0;
    while (getline(infile, line)) {
        ++line_num;

        vector<BoundingBox> bboxs = computeBoundingBox(line);
        for(const auto& bbox : bboxs) {
        double low[2] = { bbox.min_lon, bbox.min_lat };
        double high[2] = { bbox.max_lon, bbox.max_lat };
        Region r(low, high, 2);
        tree->insertData(0, nullptr, r, line_num);  // Use line number as ID
    }
    }

    // Query the R-tree with a region that intersects some of the above
    double queryLow[2] = { 1.0, 1.0 };
    double queryHigh[2] = { 3.0, 3.0 };
    Region queryRegion(queryLow, queryHigh, 2);

    MyVisitor visitor;
    tree->intersectsWithQuery(queryRegion, visitor);


    cout << tree;
    // Clean up
    delete tree;
    delete diskStorage;

    cout << "R-tree written to file: " << outputFile << endl;

    

    return 0;
}