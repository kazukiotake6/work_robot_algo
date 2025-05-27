#ifndef SWEEP_PLANNER_H
#define SWEEP_PLANNER_H

#include "geometry.h" // For Polygon and Point structures
#include <vector>     // For std::vector

// Class responsible for generating a sweep (boustrophedon) path for a given polygonal area.
// Currently optimized for axis-aligned rectangular areas.
class SweepPathPlanner {
private:
    Polygon area_to_mow_;        // The polygonal area that needs to be covered.
    double cutting_width_;       // The effective width of the mower's cutting tool.
    std::vector<Point> waypoints_; // Stores the generated sequence of points defining the path.

public:
    // Constructor for SweepPathPlanner.
    // @param area The Polygon representing the area to be mowed.
    // @param cutting_width The width of each pass of the mower.
    SweepPathPlanner(const Polygon& area, double cutting_width);

    // Generates the sweep path based on the area_to_mow_ and cutting_width_.
    // The path covers the area by making parallel sweeps.
    // For axis-aligned rectangles, it generates horizontal sweeps.
    // @return A vector of Point objects representing the waypoints of the generated path.
    std::vector<Point> generatePath();

    // Retrieves the last generated path.
    // @return A const reference to the vector of waypoints.
    const std::vector<Point>& getWaypoints() const;
};

#endif // SWEEP_PLANNER_H
