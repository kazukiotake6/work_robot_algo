#ifndef ROBOT_H
#define ROBOT_H

#include "geometry.h" // For Point struct
#include <iostream>    // For std::cout, used for printing status messages

// Represents a simple robot capable of moving to specified points and tracking its orientation.
class Robot {
private:
    Point current_position_;        // The robot's current 2D position (x, y).
    double current_orientation_deg_; // The robot's current orientation in degrees.
                                     // 0 degrees typically means facing along the positive X-axis.
                                     // 90 degrees along positive Y-axis, etc.
public:
    // Constructor for the Robot.
    // @param start_pos The initial position of the robot.
    // @param start_orientation_deg The initial orientation of the robot in degrees.
    Robot(Point start_pos, double start_orientation_deg);

    // Moves the robot to a specified target point.
    // Upon arrival, the robot's orientation is updated to face the direction from which it arrived.
    // More accurately, it updates orientation to face the `target_point` *from* its previous `current_position_`.
    // If the target_point is the same as current_position_, orientation might not change meaningfully
    // unless the robot was already at target_point but facing a different direction.
    // The function prints the movement to std::cout.
    // @param target_point The destination Point for the robot.
    void moveTo(const Point& target_point);

    // Gets the current position of the robot.
    // @return The current Point (x, y) of the robot.
    Point getPosition() const;

    // Gets the current orientation of the robot.
    // @return The current orientation in degrees.
    double getOrientation() const;
};

#endif // ROBOT_H
