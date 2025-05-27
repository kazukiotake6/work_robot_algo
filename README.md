# work_robot_algo

This repository contains algorithms and simulations related to robotics.

## Robotic Lawnmower Sweep Path Planning

This feature implements a sweep path (boustrophedon pattern) generation for a robotic lawnmower. It defines a polygonal area and calculates a series of waypoints for the robot to follow to cover the area. Currently, the path generation is optimized for axis-aligned rectangular areas.

### Core Components

*   **`Point`**: A simple structure (`TestGcc/geometry.h`) representing a 2D coordinate (x, y).
*   **`Polygon`**: A structure (`TestGcc/geometry.h`) representing a polygon as a list of `Point` vertices.
*   **`SweepPathPlanner`**: A class (`TestGcc/sweep_planner.h`, `TestGcc/sweep_planner.cpp`) responsible for generating the sweep path waypoints based on a given `Polygon` area and a cutting width.
*   **`Robot`**: A class (`TestGcc/robot.h`, `TestGcc/robot.cpp`) that simulates a robot. It can move to target points, updating its position and orientation, and printing its status.

### Running the Simulation

The main simulation is located in `TestGcc/.vscode/mingw_test.cpp`.
This file:
1.  Defines a sample rectangular lawn area (`Polygon`).
2.  Sets a cutting width for the mower.
3.  Initializes the `SweepPathPlanner` with the lawn and cutting width.
4.  Generates the waypoints using the planner.
5.  Initializes a `Robot` at the start of the path.
6.  Simulates the robot's movement through each waypoint, printing status updates (position, orientation) to the console.

To run the simulation, compile `TestGcc/.vscode/mingw_test.cpp` along with the necessary component files (`geometry.h`, `sweep_planner.cpp`, `robot.cpp`). For example, using g++:
```bash
g++ TestGcc/.vscode/mingw_test.cpp TestGcc/sweep_planner.cpp TestGcc/robot.cpp -o simulation -I TestGcc -std=c++11
./simulation
```
(The exact command might need adjustment based on your C++ environment and compiler.)

### Running the Tests

Unit tests for the geometric functions and the path planner are located in `TestGcc/test_planner.cpp`.
This file uses `cassert` for basic checks and includes tests for:
*   `distance()` function.
*   `isInside()` point-in-polygon function.
*   `SweepPathPlanner` logic for rectangular areas, including different height scenarios relative to the cutting width.

To run the tests, compile `TestGcc/test_planner.cpp` along with the necessary component files (`geometry.h`, `sweep_planner.cpp`). For example, using g++:
```bash
g++ TestGcc/test_planner.cpp TestGcc/sweep_planner.cpp TestGcc/robot.cpp -o run_tests -I TestGcc -std=c++11
./run_tests
```
(The exact command might need adjustment based on your C++ environment and compiler. Note: `robot.cpp` might not be strictly needed for `test_planner` if it only tests geometry and planner, but including it won't harm if `test_planner.cpp` doesn't use `Robot` specific features directly in its main tests).

The test executable will print "All tests passed!" if assertions are met, otherwise, it will terminate on the first failed assertion.