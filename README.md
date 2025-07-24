# SpinningCube

**SpinningCube** is a 3D ASCII animation project written in C. 
It draws rotating cubes directly in the terminal using perspective projection, character shading, and z-buffering. 
This project was built in **CLion** and runs using **Ubuntu via WSL**. 

---

## About the Project

In `spinCube.c`, it features:

- Performs 3D rotation using Euler angles
- Projects 3D coordinates to a 2D terminal space using a simplified perspective projection
- Renders ASCII characters (`@`, `$`, `#`, `+`, etc.) based on cube faces
- Uses a **z-buffer** to simulate depth and occlusion
- Animates multiple cubes spinning independently on screen

The rendering is done entirely with characters in the terminal window, refreshing each frame using ANSI escape sequences for smooth output.

---

## EXAMPLE With 3 Cubes:

<img width="1362" height="775" alt="image" src="https://github.com/user-attachments/assets/235e697b-f016-47ab-8b51-49294e57f949" />


---

## CMakeLists.txt Overview

The provided `CMakeLists.txt` and what it does is:

- Sets the C standard to C23
- Links the math library (`-lm`) automatically on UNIX-like systems
- Creates an executable called `SpinningCube`

```cmake
cmake_minimum_required(VERSION 3.31)
project(SpinningCube C)
set(CMAKE_C_STANDARD 23)

add_executable(SpinningCube spinCube.c)

if(UNIX)
    target_link_libraries(SpinningCube m)
endif()


