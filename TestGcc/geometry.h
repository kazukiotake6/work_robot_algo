#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <cmath> // For std::sqrt and std::pow
#include <algorithm> // For std::min and std::max

// Represents a 2D point with x and y coordinates.
struct Point {
    double x; // x-coordinate
    double y; // y-coordinate

    // Constructor to initialize a Point.
    Point(double _x, double _y) : x(_x), y(_y) {}
};

// Represents a polygon defined by a sequence of vertices.
struct Polygon {
    std::vector<Point> vertices; // List of Point objects representing the vertices of the polygon.

    // Constructor to initialize a Polygon with a given list of vertices.
    Polygon(const std::vector<Point>& _vertices) : vertices(_vertices) {}
};

// Calculates the Euclidean distance between two Point objects.
// Formula: sqrt((x2 - x1)^2 + (y2 - y1)^2)
inline double distance(Point p1, Point p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

// Checks if a Point p is inside the given Polygon polygon using the ray casting algorithm.
// The algorithm works by drawing a horizontal ray from the point to the right (positive x-direction)
// and counting how many times it intersects with the edges of the polygon.
// If the number of intersections is odd, the point is inside; if even, it's outside.
inline bool isInside(Point p, const Polygon& polygon) {
    int num_vertices = polygon.vertices.size();
    if (num_vertices < 3) {
        return false; // A polygon must have at least 3 vertices to be valid.
    }

    double x = p.x; // x-coordinate of the point to check
    double y = p.y; // y-coordinate of the point to check
    bool inside = false; // Initialize 'inside' status to false (point is assumed outside)

    Point p1 = polygon.vertices[0]; // Start with the first vertex of the polygon
    // Loop through all edges of the polygon (p1, p2)
    // The loop goes from i=0 to num_vertices, where polygon.vertices[num_vertices % num_vertices] is polygon.vertices[0],
    // effectively closing the polygon loop by connecting the last vertex back to the first.
    for (int i = 0; i <= num_vertices; ++i) {
        Point p2 = polygon.vertices[i % num_vertices]; // The second vertex of the current edge

        // Edge Case: Point on a horizontal segment.
        // If the point's y-coordinate matches the y-coordinates of a horizontal segment,
        // and the point's x-coordinate is within the x-range of that segment, it's on the boundary (considered inside).
        if (p1.y == p2.y && p1.y == y && x >= std::min(p1.x, p2.x) && x <= std::max(p1.x, p2.x)) {
            return true; // Point is on a horizontal boundary segment.
        }

        // Edge Case: Point on a vertical segment.
        // If the point's x-coordinate matches the x-coordinates of a vertical segment,
        // and the point's y-coordinate is within the y-range of that segment, it's on the boundary (considered inside).
        if (p1.x == p2.x && p1.x == x && y >= std::min(p1.y, p2.y) && y <= std::max(p1.y, p2.y)) {
            return true; // Point is on a vertical boundary segment.
        }

        // Check if the horizontal ray (extending to the right from p) intersects with the current edge (p1, p2).
        // This condition checks if the edge crosses the horizontal line at y = p.y.
        // One endpoint (p1) must be below or on the line, and the other (p2) must be above or on the line, or vice versa.
        if ((p1.y < y && p2.y >= y) || (p2.y < y && p1.y >= y)) {
            // Calculate the x-coordinate of the intersection point of the ray and the edge.
            // This is done using the formula for line intersection derived from the line equation:
            // x_intersection = (y_target - y1) * (x2 - x1) / (y2 - y1) + x1
            // This specific formula avoids division by zero if the edge is horizontal (p1.y == p2.y),
            // which is handled by the edge case check above.
            // If the edge is vertical (p1.x == p2.x), (p2.y - p1.y) could be non-zero.
            double x_intersection = (y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;

            // If the intersection point is to the right of the point p (x_intersection > x),
            // it means the ray has crossed this edge. We toggle the 'inside' status.
            if (x_intersection > x) {
                inside = !inside;
            }
        }
        p1 = p2; // Move to the next edge by setting p1 to the current p2.
    }
    return inside; // Return the final 'inside' status.
}

#endif // GEOMETRY_H
