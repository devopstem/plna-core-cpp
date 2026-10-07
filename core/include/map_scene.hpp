#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace plna::map {

enum class EquipType : int32_t {
    Rack = 1,
    Cooler = 2,
    Stand = 3
};

enum class MarkerType : int32_t {
    Beacon = 1,
    Cash = 2,
    Entrance = 3,
    Exit = 4,
    Other = 0
};

enum class DrawOp : int32_t {
    Floor = 1,     // x,y,w,h, r,g,b,a
    Box2D = 2,     // x,y,w,h, r,g,b,a
    Line = 3,      // x1,y1,x2,y2, r,g,b,a, stroke
    Circle = 4,    // x,y,radius, r,g,b,a
    Box3D = 5,     // x,y,w,d,h, r,g,b,a  (world meters; projected in Kotlin or here as 2D faces)
    Path = 6       // count, x0,y0,... then r,g,b,a,stroke
};

struct Equipment {
    std::string id;
    EquipType type = EquipType::Rack;
    float x = 0.f;
    float y = 0.f;
    float w = 1.f;
    float d = 0.7f;
    float h = 1.6f;
    float rotationDeg = 0.f;
    int shelves = 4;
    float r = 0.4f, g = 0.5f, b = 0.9f, a = 1.f;
    bool visible = true;
};

struct Wall {
    float x1 = 0.f, y1 = 0.f, x2 = 0.f, y2 = 0.f;
    bool isDoor = false;
};

struct Marker {
    std::string id;
    MarkerType type = MarkerType::Other;
    float x = 0.f, y = 0.f;
    float w = 0.4f, d = 0.4f;
    float r = 0.7f, g = 0.3f, b = 0.9f, a = 1.f;
    bool visible = true;
};

class MapScene {
public:
    static MapScene& instance();

    void clear();
    void setFloor(float widthMeters, float heightMeters);
    void addEquipment(const Equipment& e);
    void addWall(const Wall& w);
    void addMarker(const Marker& m);
    void setLayerVisible(int layer, bool visible); // 1 racks 2 coolers 3 stands 4 beacons
    void setView3D(bool enabled);
    void setRoute(const std::vector<float>& xyPairs);
    void clearRoute();

    /** Packed draw list for Canvas / GLES. */
    std::vector<float> buildDrawList() const;

    float floorWidth() const { return floorW_; }
    float floorHeight() const { return floorH_; }
    bool is3D() const { return view3d_; }

    std::vector<float> routeAround(float fromX, float fromY, float toX, float toY) const;

private:
    MapScene() = default;

    float floorW_ = 40.f;
    float floorH_ = 40.f;
    bool view3d_ = true;
    bool showRacks_ = true;
    bool showCoolers_ = true;
    bool showStands_ = true;
    bool showBeacons_ = true;

    std::vector<Equipment> equipment_;
    std::vector<Wall> walls_;
    std::vector<Marker> markers_;
    std::vector<float> route_;
};

} // namespace plna::map
