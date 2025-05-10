#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <vector>

struct BoundingBox {
    double min_lon = std::numeric_limits<double>::max();
    double max_lon = std::numeric_limits<double>::lowest();
    double min_lat = std::numeric_limits<double>::max();
    double max_lat = std::numeric_limits<double>::lowest();
};

BoundingBox computeBoundingBox(const std::string& linestring) {
    BoundingBox box;

    size_t start = linestring.find("(");
    size_t end = linestring.find(")");
    if (start == std::string::npos || end == std::string::npos) {
        std::cerr << "Invalid LINESTRING format.\n";
        return box;
    }

    std::string coords_str = linestring.substr(start + 1, end - start - 1);
    std::stringstream ss(coords_str);
    std::string coord;

    while (std::getline(ss, coord, ',')) {
        std::stringstream cs(coord);
        double lon, lat;
        cs >> lon >> lat;

        if (lon < box.min_lon) box.min_lon = lon;
        if (lon > box.max_lon) box.max_lon = lon;
        if (lat < box.min_lat) box.min_lat = lat;
        if (lat > box.max_lat) box.max_lat = lat;
    }

    return box;
}

int main(int argc, char *argv[]) {
    std::ifstream infile(argv[1]);
    if (!infile) {
        std::cerr << "Error opening file.\n";
        return 1;
    }

    std::string line;
    int line_num = 0;
    while (std::getline(infile, line)) {
        ++line_num;

        BoundingBox bbox = computeBoundingBox(line);

        std::cout << "Line " << line_num << " Bounding Box:\n";
        std::cout << "  Min Longitude: " << bbox.min_lon << "\n";
        std::cout << "  Max Longitude: " << bbox.max_lon << "\n";
        std::cout << "  Min Latitude:  " << bbox.min_lat << "\n";
        std::cout << "  Max Latitude:  " << bbox.max_lat << "\n\n";
    }

    return 0;
}
