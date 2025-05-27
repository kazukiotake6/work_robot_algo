#include "robot.h"
#include <cmath>    // For std::atan2 and M_PI
#include <iostream> // For std::cout for printing status messages

// Define M_PI if it's not already defined by <cmath> (common in some C++ standards/compilers, e.g., MSVC)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Constructor: Initializes the robot's starting position and orientation.
// It also prints an initial status message to the console.
Robot::Robot(Point start_pos, double start_orientation_deg)
    : current_position_(start_pos), current_orientation_deg_(start_orientation_deg) {
    std::cout << "Robot initialized at (" << current_position_.x << ", " << current_position_.y
              << ") facing " << current_orientation_deg_ << " degrees." << std::endl;
}

// Simulates moving the robot to a specified target point.
// This function updates the robot's current_position_ to the target_point.
// It also calculates and updates the robot's current_orientation_deg_ to face
// the target_point from its previous position.
// A status message detailing the movement is printed to std::cout.
void Robot::moveTo(const Point& target_point) {
    Point old_position = current_position_; // Store current position for the status message.

    // Calculate the change in x and y coordinates.
    double delta_x = target_point.x - current_position_.x;
    double delta_y = target_point.y - current_position_.y;
    
    // Update orientation only if the robot is actually changing its position.
    // If delta_x and delta_y are both zero, the robot is "moving" to its current spot,
    // so its orientation should ideally remain unchanged unless it's the first move.
    // atan2(0,0) is often 0, which might unintentionally reset orientation.
    if (delta_x != 0.0 || delta_y != 0.0) {
        // Calculate the angle in radians using atan2(y, x).
        // atan2 handles quadrants correctly and provides an angle in the range [-PI, PI].
        double angle_rad = std::atan2(delta_y, delta_x);
        // Convert radians to degrees.
        current_orientation_deg_ = angle_rad * (180.0 / M_PI);
    }
    // It might be desirable to have a default orientation update policy if delta_x and delta_y are 0,
    // e.g., if the robot is at (0,0) and told to move to (0,0) but its initial orientation was 90 deg.
    // Current logic: orientation remains as it was if not moving to a new point.

    // Update the robot's position to the target point.
    current_position_ = target_point;

    // Print a status message about the movement.
    std::cout << "Robot moving from (" << old_position.x << ", " << old_position.y
              << ") to (" << current_position_.x << ", " << current_position_.y
              << "). New orientation: " << current_orientation_deg_ << " degrees." << std::endl;
}

// Getter for the robot's current position.
// Returns a Point object representing the (x, y) coordinates.
Point Robot::getPosition() const {
    return current_position_;
}

// Getter for the robot's current orientation.
// Returns the orientation in degrees.
double Robot::getOrientation() const {
    return current_orientation_deg_;
}
