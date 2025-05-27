#include <iostream>     // For std::cout, std::endl
#include <vector>       // For std::vector
#include <cassert>      // For assert()
#include <cmath>        // For std::fabs (used in are_doubles_equal)
#include "geometry.h"   // For Point, Polygon, distance(), isInside()
#include "sweep_planner.h" // For SweepPathPlanner

// Helper function to compare two double values within a small tolerance (epsilon).
// This is useful for floating-point comparisons where exact equality is unlikely.
// @param a The first double value.
// @param b The second double value.
// @param epsilon The tolerance for comparison. Defaults to 1e-5.
// @return True if the absolute difference between a and b is less than epsilon, false otherwise.
bool are_doubles_equal(double a, double b, double epsilon = 1e-5) {
    return std::fabs(a - b) < epsilon;
}

// Test function for the distance() function in geometry.h.
// It checks various scenarios:
// - Distance between two distinct points (e.g., (0,0) to (3,4) should be 5).
// - Distance from a point to itself (should be 0).
// - Commutative property (distance(p1,p2) == distance(p2,p1)).
// - Distances along axes.
void testDistance() {
    std::cout << "Running testDistance..." << std::endl;
    Point p1(0.0, 0.0);
    Point p2(3.0, 4.0); // Forms a 3-4-5 triangle with p1
    Point p3(0.0, 0.0); // Same as p1
    Point p4(-1.0, 0.0); // 1 unit along negative x-axis from p1
    Point p5(0.0, -1.0); // 1 unit along negative y-axis from p1

    assert(are_doubles_equal(distance(p1, p2), 5.0)); // Basic 3-4-5 triangle
    assert(are_doubles_equal(distance(p1, p3), 0.0)); // Distance to self should be zero
    assert(are_doubles_equal(distance(p2, p1), 5.0)); // Distance is commutative
    assert(are_doubles_equal(distance(p1, p4), 1.0)); // Distance along x-axis
    assert(are_doubles_equal(distance(p1, p5), 1.0)); // Distance along y-axis
    assert(are_doubles_equal(distance(Point(1.0, 1.0), Point(1.0, 5.0)), 4.0)); // Purely vertical distance
    assert(are_doubles_equal(distance(Point(1.0, 1.0), Point(5.0, 1.0)), 4.0)); // Purely horizontal distance

    std::cout << "testDistance PASSED" << std::endl;
}

// Test function for the isInside() function in geometry.h.
// It checks if points are correctly identified as inside, outside, or on the boundary
// for a square polygon and a more complex L-shaped polygon.
// The current implementation of isInside considers points on the boundary as inside.
void testIsInside() {
    std::cout << "Running testIsInside..." << std::endl;
    
    // Define a square polygon: (0,0) -> (0,4) -> (4,4) -> (4,0)
    std::vector<Point> vertices;
    vertices.push_back(Point(0.0, 0.0));
    vertices.push_back(Point(0.0, 4.0));
    vertices.push_back(Point(4.0, 4.0));
    vertices.push_back(Point(4.0, 0.0));
    Polygon square(vertices);

    // Test points clearly inside the square
    assert(isInside(Point(2.0, 2.0), square) == true); // Center
    assert(isInside(Point(1.0, 3.0), square) == true); // Another internal point

    // Test points clearly outside the square
    assert(isInside(Point(5.0, 2.0), square) == false); // Right
    assert(isInside(Point(2.0, 5.0), square) == false); // Top
    assert(isInside(Point(-1.0, 2.0), square) == false); // Left
    assert(isInside(Point(2.0, -1.0), square) == false); // Bottom

    // Test points on the boundary of the square
    // According to the current `isInside` logic, points on the boundary are considered inside.
    assert(isInside(Point(0.0, 2.0), square) == true); // On left edge
    assert(isInside(Point(2.0, 0.0), square) == true); // On bottom edge
    assert(isInside(Point(4.0, 2.0), square) == true); // On right edge
    assert(isInside(Point(2.0, 4.0), square) == true); // On top edge
    assert(isInside(Point(0.0, 0.0), square) == true); // Vertex (bottom-left)
    assert(isInside(Point(4.0, 4.0), square) == true); // Vertex (top-right)

    // Test with a more complex L-shaped polygon:
    // (0,0)->(0,3)->(1,3)->(1,1)->(3,1)->(3,0)
    std::vector<Point> l_shape_vertices;
    l_shape_vertices.push_back(Point(0,0));
    l_shape_vertices.push_back(Point(0,3));
    l_shape_vertices.push_back(Point(1,3));
    l_shape_vertices.push_back(Point(1,1)); // Inner corner
    l_shape_vertices.push_back(Point(3,1));
    l_shape_vertices.push_back(Point(3,0));
    Polygon l_shape(l_shape_vertices);

    assert(isInside(Point(0.5, 2.0), l_shape) == true);  // Inside the upper vertical part of L
    assert(isInside(Point(2.0, 0.5), l_shape) == true);  // Inside the lower horizontal part of L
    assert(isInside(Point(0.5, 0.5), l_shape) == true);  // Inside the corner "square" part of L
    assert(isInside(Point(1.5, 1.5), l_shape) == false); // Point in the "empty" area of the L-shape's bounding box
    assert(isInside(Point(1.0, 2.0), l_shape) == true);  // On the inner vertical edge of L-shape
    assert(isInside(Point(2.0, 1.0), l_shape) == true);  // On the inner horizontal edge of L-shape

    // Test a point far outside any defined polygon
    assert(isInside(Point(100.0, 100.0), square) == false);

    std::cout << "testIsInside PASSED" << std::endl;
}

// Test function for the SweepPathPlanner's generatePath() method,
// specifically for axis-aligned rectangular areas.
// It checks:
// - Path generation for a rectangle where height is an exact multiple of cutting width.
// - Correct number of waypoints and their y-coordinates.
// - Alternating sweep direction (left-to-right, right-to-left).
// - Path generation for a rectangle where height is NOT an exact multiple of cutting width.
// - Path generation for a rectangle where height is LESS than cutting width (should be one sweep).
void testSweepPathPlannerRectangle() {
    std::cout << "Running testSweepPathPlannerRectangle..." << std::endl;
    
    // Test Case 1: Rectangle height is an exact multiple of cutting_width.
    // Defines a 5x10 rectangle with a cutting width of 2.0.
    // Expected: 5 sweeps, 10 waypoints.
    std::vector<Point> rect_vertices;
    rect_vertices.push_back(Point(0.0, 0.0));   // min_x=0, min_y=0
    rect_vertices.push_back(Point(0.0, 10.0));  // max_y=10
    rect_vertices.push_back(Point(5.0, 10.0));  // max_x=5
    rect_vertices.push_back(Point(5.0, 0.0));
    Polygon rectangle(rect_vertices);
    double cutting_width = 2.0;

    SweepPathPlanner planner(rectangle, cutting_width);
    std::vector<Point> path = planner.generatePath();

    assert(!path.empty());

    // Expected number of sweeps:
    // Height = 10.0. Cutting width = 2.0.
    // First sweep at y = 0 + 2/2 = 1.0
    // Second sweep at y = 1.0 + 2.0 = 3.0
    // Third sweep at y = 3.0 + 2.0 = 5.0
    // Fourth sweep at y = 5.0 + 2.0 = 7.0
    // Fifth sweep at y = 7.0 + 2.0 = 9.0
    // Total 5 sweeps. Each sweep has 2 points. So 10 waypoints.
    assert(path.size() == 10);

    // Verify y-coordinates and alternating x-direction for each sweep.
    double expected_y = cutting_width / 2.0; // Initial y = 1.0
    bool expect_left_to_right = true;        // First sweep is from min_x to max_x.

    for (size_t i = 0; i < path.size(); i += 2) { // Iterate over pairs of waypoints (each sweep)
        assert(are_doubles_equal(path[i].y, expected_y));     // y-coord of start point of sweep
        assert(are_doubles_equal(path[i+1].y, expected_y)); // y-coord of end point of sweep

        if (expect_left_to_right) {
            assert(are_doubles_equal(path[i].x, 0.0));   // Start at min_x
            assert(are_doubles_equal(path[i+1].x, 5.0)); // End at max_x
        } else {
            assert(are_doubles_equal(path[i].x, 5.0));   // Start at max_x
            assert(are_doubles_equal(path[i+1].x, 0.0)); // End at min_x
        }
        expected_y += cutting_width; // Move to the next expected y-coordinate.
        expect_left_to_right = !expect_left_to_right; // Alternate direction.
    }

    // Test Case 2: Rectangle height is NOT an exact multiple of cutting_width.
    // Defines a 5x9 rectangle with a cutting width of 2.0.
    // Expected: 4 sweeps (at y=1,3,5,7), 8 waypoints.
    std::vector<Point> rect_vertices2;
    rect_vertices2.push_back(Point(0.0, 0.0));
    rect_vertices2.push_back(Point(0.0, 9.0)); // Height 9
    rect_vertices2.push_back(Point(5.0, 9.0));
    rect_vertices2.push_back(Point(5.0, 0.0));
    Polygon rectangle2(rect_vertices2);
    SweepPathPlanner planner2(rectangle2, cutting_width); // cutting_width = 2.0
    std::vector<Point> path2 = planner2.generatePath();

    // Expected sweeps for height 9, width 2:
    // y = 1.0, 3.0, 5.0, 7.0. (max_y - cw/2 = 9-1=8). Last sweep is at y=7.0
    // 4 sweeps, so 8 waypoints.
    // The last sweep is at y=7. Next would be y=9.
    // The condition is `current_y <= max_y - cutting_width_ / 2.0`
    // 1 <= 9-1=8 (true) -> p1(0,1), p2(5,1)
    // 3 <= 8 (true) -> p3(5,3), p4(0,3)
    // 5 <= 8 (true) -> p5(0,5), p6(5,5)
    // 7 <= 8 (true) -> p7(5,7), p8(0,7)
    // 9 <= 8 (false) -> loop ends
    assert(path2.size() == 8);
    if(path2.size() == 8) { // Further checks if size is as expected.
        assert(are_doubles_equal(path2[0].y, 1.0)); // First sweep y
        assert(are_doubles_equal(path2[2].y, 3.0)); // Second sweep y
        assert(are_doubles_equal(path2[4].y, 5.0)); // Third sweep y
        assert(are_doubles_equal(path2[6].y, 7.0)); // Fourth sweep y
    }

    // Test Case 3: Rectangle height is LESS than cutting_width.
    // Defines a 5x1 rectangle with a cutting width of 2.0.
    // Expected: 1 sweep in the middle (at y=0.5), 2 waypoints.
    std::vector<Point> rect_vertices3;
    rect_vertices3.push_back(Point(0.0, 0.0));
    rect_vertices3.push_back(Point(0.0, 1.0)); // Height 1
    rect_vertices3.push_back(Point(5.0, 1.0));
    rect_vertices3.push_back(Point(5.0, 0.0));
    Polygon rectangle3(rect_vertices3); // cutting_width = 2.0
    SweepPathPlanner planner3(rectangle3, cutting_width);
    std::vector<Point> path3 = planner3.generatePath();
    // Expected: one sweep line at y = (0+1)/2 = 0.5
    assert(path3.size() == 2); 
    if (path3.size() == 2) {
        assert(are_doubles_equal(path3[0].y, 0.5));
        assert(are_doubles_equal(path3[1].y, 0.5));
        assert(are_doubles_equal(path3[0].x, 0.0));
        assert(are_doubles_equal(path3[1].x, 5.0));
    }


    std::cout << "testSweepPathPlannerRectangle PASSED" << std::endl;
}

int main() {
    std::cout << "--- Starting All Tests ---" << std::endl;

    testDistance(); // Test geometric distance calculation.
    std::cout << std::endl; 
    testIsInside(); // Test point-in-polygon detection.
    std::cout << std::endl;
    testSweepPathPlannerRectangle(); // Test sweep path generation for rectangles.
    std::cout << std::endl;

    std::cout << "--- All tests passed! ---" << std::endl;
    return 0; // Indicate successful execution of tests.
}
