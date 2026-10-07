#include "map_scene.hpp"

#include <algorithm>
#include <cmath>

namespace plna::map {

MapScene& MapScene::instance() {
    static MapScene scene;
    return scene;
}

void MapScene::clear() {
    equipment_.clear();
    walls_.clear();
    markers_.clear();
    route_.clear();
    floorW_ = 40.f;
    floorH_ = 40.f;
}

void MapScene::setFloor(float widthMeters, float heightMeters) {
    floorW_ = std::max(1.f, widthMeters);
    floorH_ = std::max(1.f, heightMeters);
}

void MapScene::addEquipment(const Equipment& e) {
    equipment_.push_back(e);
}

void MapScene::addWall(const Wall& w) {
    walls_.push_back(w);
}

void MapScene::addMarker(const Marker& m) {
    markers_.push_back(m);
}

void MapScene::setLayerVisible(int layer, bool visible) {
    switch (layer) {
        case 1: showRacks_ = visible; break;
        case 2: showCoolers_ = visible; break;
        case 3: showStands_ = visible; break;
        case 4: showBeacons_ = visible; break;
        default: break;
    }
}

void MapScene::setView3D(bool enabled) {
    view3d_ = enabled;
}

void MapScene::setRoute(const std::vector<float>& xyPairs) {
    route_ = xyPairs;
}

void MapScene::clearRoute() {
    route_.clear();
}

namespace {

bool layerOk(EquipType t, bool racks, bool coolers, bool stands) {
    switch (t) {
        case EquipType::Rack: return racks;
        case EquipType::Cooler: return coolers;
        case EquipType::Stand: return stands;
    }
    return true;
}

void pushFloor(std::vector<float>& out, float w, float h) {
    out.push_back(static_cast<float>(DrawOp::Floor));
    out.push_back(0.f);
    out.push_back(0.f);
    out.push_back(w);
    out.push_back(h);
    out.push_back(0.06f);
    out.push_back(0.09f);
    out.push_back(0.16f);
    out.push_back(1.f);
}

void pushBox2d(std::vector<float>& out, float x, float y, float w, float d,
               float r, float g, float b, float a) {
    out.push_back(static_cast<float>(DrawOp::Box2D));
    out.push_back(x);
    out.push_back(y);
    out.push_back(w);
    out.push_back(d);
    out.push_back(r);
    out.push_back(g);
    out.push_back(b);
    out.push_back(a);
}

void pushBox3d(std::vector<float>& out, float x, float y, float w, float d, float h,
               float r, float g, float b, float a) {
    out.push_back(static_cast<float>(DrawOp::Box3D));
    out.push_back(x);
    out.push_back(y);
    out.push_back(w);
    out.push_back(d);
    out.push_back(h);
    out.push_back(r);
    out.push_back(g);
    out.push_back(b);
    out.push_back(a);
}

void pushLine(std::vector<float>& out, float x1, float y1, float x2, float y2,
              float r, float g, float b, float a, float stroke) {
    out.push_back(static_cast<float>(DrawOp::Line));
    out.push_back(x1);
    out.push_back(y1);
    out.push_back(x2);
    out.push_back(y2);
    out.push_back(r);
    out.push_back(g);
    out.push_back(b);
    out.push_back(a);
    out.push_back(stroke);
}

void pushCircle(std::vector<float>& out, float x, float y, float radius,
                float r, float g, float b, float a) {
    out.push_back(static_cast<float>(DrawOp::Circle));
    out.push_back(x);
    out.push_back(y);
    out.push_back(radius);
    out.push_back(r);
    out.push_back(g);
    out.push_back(b);
    out.push_back(a);
}

} // namespace

std::vector<float> MapScene::buildDrawList() const {
    std::vector<float> out;
    out.reserve(2048);

    pushFloor(out, floorW_, floorH_);

    // Grid every 5 m
    const float step = 5.f;
    for (float x = step; x < floorW_; x += step) {
        pushLine(out, x, 0.f, x, floorH_, 0.15f, 0.2f, 0.3f, 0.35f, 1.f);
    }
    for (float y = step; y < floorH_; y += step) {
        pushLine(out, 0.f, y, floorW_, y, 0.15f, 0.2f, 0.3f, 0.35f, 1.f);
    }

    for (const auto& w : walls_) {
        if (w.isDoor) {
            pushLine(out, w.x1, w.y1, w.x2, w.y2, 0.13f, 0.77f, 0.37f, 0.95f, 3.f);
        } else {
            pushLine(out, w.x1, w.y1, w.x2, w.y2, 0.7f, 0.75f, 0.85f, 0.95f, 2.5f);
        }
    }

    for (const auto& e : equipment_) {
        if (!layerOk(e.type, showRacks_, showCoolers_, showStands_)) continue;
        if (view3d_) {
            pushBox3d(out, e.x, e.y, e.w, e.d, e.h, e.r, e.g, e.b, e.a);
        } else {
            pushBox2d(out, e.x, e.y, e.w, e.d, e.r, e.g, e.b, e.a);
        }
    }

    for (const auto& m : markers_) {
        if (m.type == MarkerType::Beacon && !showBeacons_) continue;
        const float rad = std::max(m.w, m.d) * 0.5f;
        if (m.type == MarkerType::Beacon) {
            pushCircle(out, m.x, m.y, rad, m.r, m.g, m.b, m.a);
        } else {
            pushBox2d(out, m.x, m.y, m.w, m.d, m.r, m.g, m.b, m.a);
        }
    }

    if (route_.size() >= 4) {
        out.push_back(static_cast<float>(DrawOp::Path));
        const int count = static_cast<int>(route_.size() / 2);
        out.push_back(static_cast<float>(count));
        out.insert(out.end(), route_.begin(), route_.end());
        out.push_back(0.39f); // indigo
        out.push_back(0.40f);
        out.push_back(0.95f);
        out.push_back(1.f);
        out.push_back(4.f); // stroke
    }

    return out;
}

std::vector<float> MapScene::routeAround(float fromX, float fromY, float toX, float toY) const {
    // L-path with slight corridor offset to avoid overlapping rack centers.
    std::vector<float> path;
    path.reserve(8);
    path.push_back(fromX);
    path.push_back(fromY);

    const float midX = toX;
    const float midY = fromY;
    // If a rack roughly blocks the horizontal leg, go via corridor at y+2
    bool blocked = false;
    for (const auto& e : equipment_) {
        const float minX = e.x;
        const float maxX = e.x + e.w;
        const float minY = e.y;
        const float maxY = e.y + e.d;
        const float segMinX = std::min(fromX, midX);
        const float segMaxX = std::max(fromX, midX);
        if (midY >= minY && midY <= maxY && segMaxX >= minX && segMinX <= maxX) {
            blocked = true;
            break;
        }
    }
    if (blocked) {
        const float detourY = std::clamp(fromY + 3.f, 0.f, floorH_);
        path.push_back(fromX);
        path.push_back(detourY);
        path.push_back(toX);
        path.push_back(detourY);
    } else {
        path.push_back(midX);
        path.push_back(midY);
    }
    path.push_back(toX);
    path.push_back(toY);
    return path;
}

} // namespace plna::map
