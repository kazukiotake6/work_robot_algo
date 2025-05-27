#include <iostream> // For std::cout, std::endl
#include <vector>   // For std::vector
#include "../geometry.h"       // For Point and Polygon structures
#include "../sweep_planner.h"  // For SweepPathPlanner class
#include "../robot.h"          // For Robot class

int main() {
    // --- 1. Define the Lawn Area ---
    // Create a list of Point objects to define the vertices of the lawn.
    // The order of vertices should define the polygon's boundary.
    // For this example, a simple rectangular lawn is defined.
    std::vector<Point> lawn_vertices;
    lawn_vertices.push_back(Point(0.0, 0.0));   // Bottom-left corner
    lawn_vertices.push_back(Point(0.0, 10.0));  // Top-left corner
    lawn_vertices.push_back(Point(20.0, 10.0)); // Top-right corner
    lawn_vertices.push_back(Point(20.0, 0.0));  // Bottom-right corner

    Polygon lawn_area(lawn_vertices); // Create a Polygon object from the vertices.
    double cutting_width = 1.0;       // Define the mower's cutting width (e.g., in meters).

    // Print the defined lawn parameters to the console.
    std::cout << "Lawn defined. Area vertices: ";
    for(size_t i = 0; i < lawn_vertices.size(); ++i) {
        std::cout << "(" << lawn_vertices[i].x << ", " << lawn_vertices[i].y << ") ";
    }
    std::cout << "Cutting width: " << cutting_width << std::endl;

    // --- 2. Initialize the Path Planner and Generate Waypoints ---
    // Create a SweepPathPlanner object with the defined lawn area and cutting width.
    SweepPathPlanner planner(lawn_area, cutting_width);
    // Call generatePath() to calculate the sequence of waypoints for the sweep pattern.
    std::vector<Point> waypoints = planner.generatePath();
    std::cout << "Path generated with " << waypoints.size() << " waypoints." << std::endl;

    // --- 3. Setup the Robot ---
    // Check if any waypoints were generated. If not, the simulation cannot proceed.
    if (waypoints.empty()) {
        std::cout << "No waypoints generated, exiting." << std::endl;
        // Optional: Add diagnostic information if waypoints are unexpectedly empty.
        // This could involve checking the lawn dimensions against the cutting width.
        if (lawn_area.vertices.size() >= 3 && cutting_width > 0) {
            double min_y_coord = lawn_vertices[0].y, max_y_coord = lawn_vertices[0].y;
            for(const auto& v : lawn_vertices) {
                if (v.y < min_y_coord) min_y_coord = v.y;
                if (v.y > max_y_coord) max_y_coord = v.y;
            }
            if ( (max_y_coord - min_y_coord) < cutting_width && (max_y_coord > min_y_coord) ) {
                 std::cout << "Reason: Lawn height (" << (max_y_coord - min_y_coord) << ") is less than cutting width (" << cutting_width << ")." << std::endl;
                 std::cout << "The planner's current logic for handling areas smaller than the cutting width should create a single pass. If not, this might indicate an issue in SweepPathPlanner." << std::endl;
            } else if (max_y_coord <= min_y_coord) {
                 std::cout << "Reason: Invalid lawn dimensions (e.g., max_y <= min_y)." << std::endl;
            }
        }
        return 0; // Exit if no path.
    }

    // Initialize the Robot at the first waypoint of the generated path.
    // The robot's initial orientation is set to 0.0 degrees (typically along the positive X-axis).
    // The Robot constructor will print its initial state.
    Robot robot(waypoints[0], 0.0); 

    // --- 4. Run the Simulation ---
    // Print a header message for the simulation part.
    std::cout << "\n--- Starting Simulation ---" << std::endl;
    
    // Iterate through each waypoint in the generated path.
    // The robot is already at waypoints[0] due to initialization.
    // The moveTo command for waypoints[0] will primarily adjust its orientation correctly
    // towards the direction of the first segment if needed (or maintain 0.0 if already aligned).
    for (size_t i = 0; i < waypoints.size(); ++i) {
        // Print a message indicating the current target waypoint.
        std::cout << "\nMoving to waypoint " << i + 1 << "/" << waypoints.size() 
                  << ": Target (" << waypoints[i].x << ", " << waypoints[i].y << ")" << std::endl;
        // Command the robot to move to the current waypoint.
        // The Robot's moveTo() method handles updating its position and orientation,
        // and also prints a status message about the movement.
        robot.moveTo(waypoints[i]);
    }
    
    // --- 5. Simulation Complete ---
    // Print a footer message indicating the end of the simulation.
    std::cout << "\n--- Simulation Complete ---" << std::endl;
    // Print the robot's final position and orientation.
    std::cout << "Final robot position: (" << robot.getPosition().x << ", " << robot.getPosition().y 
              << ") facing " << robot.getOrientation() << " degrees." << std::endl;

    return 0; // Indicate successful execution.
}