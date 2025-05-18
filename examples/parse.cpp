#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <vector>


#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

constexpr double EARTH_RADIUS_M = 6371000.0;
constexpr double DEG_TO_RAD = M_PI / 180.0;

struct Point {
    double lon;
    double lat;
};

// Approximate area of a lat-lon rectangle in square meters
double rectangleArea(double lat1, double lon1, double lat2, double lon2) {
    // Convert to radians
    lat1 *= DEG_TO_RAD;
    lat2 *= DEG_TO_RAD;
    lon1 *= DEG_TO_RAD;
    lon2 *= DEG_TO_RAD;

    double dLat = std::abs(lat2 - lat1);
    double dLon = std::abs(lon2 - lon1);

    // Approximate area using spherical Earth formula
    double avgLat = (lat1 + lat2) / 2.0;
    double width = EARTH_RADIUS_M * dLon * std::cos(avgLat); // longitudinal width
    double height = EARTH_RADIUS_M * dLat; // latitudinal height

    return width * height;
}

struct BoundingBox {
    double min_lon = std::numeric_limits<double>::max();
    double max_lon = std::numeric_limits<double>::lowest();
    double min_lat = std::numeric_limits<double>::max();
    double max_lat = std::numeric_limits<double>::lowest();
};

struct Rectangle {
    Rectangle() : min_lon(std::numeric_limits<double>::max()),
                  max_lon(std::numeric_limits<double>::lowest()),
                  min_lat(std::numeric_limits<double>::max()),
                  max_lat(std::numeric_limits<double>::lowest()) {}
    Rectangle(double lon, double lat)
        : min_lon(lon), max_lon(lon), min_lat(lat), max_lat(lat) {}
    double min_lon;
    double max_lon;
    double min_lat;
    double max_lat;
    void update(double lon, double lat) {
        if (lon < min_lon) min_lon = lon;
        if (lon > max_lon) max_lon = lon;
        if (lat < min_lat) min_lat = lat;
        if (lat > max_lat) max_lat = lat;
    }
    double area() const {
        return rectangleArea(min_lat, min_lon, max_lat, max_lon);
    }
    void print() const {
        std::cout << "Bounding Box: (" << min_lon << ", " << min_lat << "), ("
                  << max_lon << ", " << max_lat << ")\n";
    }
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
    std::vector<Point> points;

    while (std::getline(ss, coord, ',')) {
        
        std::stringstream cs(coord);
        double lon, lat;
        cs >> lon >> lat;
        points.push_back({lon, lat});

        if (lon < box.min_lon) box.min_lon = lon;
        if (lon > box.max_lon) box.max_lon = lon;
        if (lat < box.min_lat) box.min_lat = lat;
        if (lat > box.max_lat) box.max_lat = lat;
    }

    double bounding_area = rectangleArea(box.min_lat, box.min_lon, box.max_lat, box.max_lon);
    // Individual rectangles between successive points
    double total_area = 0.0;
    Rectangle rect[2];
    for (size_t i = 0; i < points.size()/2; ++i) {
        rect[0].update(points[i].lon, points[i].lat);
    }
    for (size_t i = points.size()/2; i < points.size(); ++i) {
        rect[1].update(points[i].lon, points[i].lat);
    }
    
    total_area = rect[0].area()+  rect[1].area();
    
      /*  if(total_area>bounding_area){
            if (points.size() > 2) {
                
            for (const auto& point : points) {
                std::cout << "Point: (" << point.lon << ", " << point.lat << ") \t";
            }
        }
        }*/
    printf("ratio: %f, points: %d ", total_area/bounding_area,points.size());
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
        
        std::cout << "Line " << line_num << " ,";
        BoundingBox bbox = computeBoundingBox(line);

        
        std::cout << "bb ("<< bbox.min_lon <<"," << bbox.max_lon << "),("<< bbox.min_lat <<"," << bbox.max_lat << ")\n";
    }

    return 0;
}
