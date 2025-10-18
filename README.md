# Contour Engine

A real-time, physics-based graphics engine written in C and modern OpenGL that renders 3D models as dynamic, interactive contour lines.

<img src="https://images.rawpixel.com/image_png_800/cHJpdmF0ZS9sci9pbWFnZXMvd2Vic2l0ZS8yMDI0LTExL3Jhd3BpeGVsb2ZmaWNlM19odW1hbl9mYWNlX2xpbmVfaWxsdXNpb25fZWxlbWVudF9taW5pbWFsX3BzeWNoZV9mOGRlMjY0YS1kNjUzLTQwYzctOWY4Yy0yYmNkMTY5MDhhMDJfMi5wbmc.png" alt="Contour Rendered Head" width="300"/>
*(Image represents the target visual style)*
## About The Project

This project explores an alternative to traditional pixel-based rasterization. Instead of rendering triangles, it generates a physics-based representation of a 3D model using a Verlet integration simulation. This particle system is then rendered as a series of flowing, stylized contour lines inspired by topographic maps and engraving art.

The entire physics simulation runs on the GPU using OpenGL Compute Shaders, allowing for hundreds of thousands of interconnected particles to be simulated in real-time. This allows the rendered object to be "physically" interactive—it can be pushed, pulled, and deformed.

The rendering pipeline uses a hybrid approach:
* The contour lines are rasterized using Geometry Shaders for stylized thickness and lighting.
* Shadows and other effects are generated via ray marching against simple Signed Distance Fields (SDFs), creating a unique blend of rendering techniques.

### Built With

* **C**
* **OpenGL 4.3+** (Specifically Compute Shaders & Geometry Shaders)
* **SDL2** for windowing and input
* **GLAD** for loading OpenGL functions
* **cglm** for 3D math
* **CMake** for the build system

## Getting Started

To get a local copy up and running, follow these simple steps.

### Prerequisites

You will need a C compiler (GCC, Clang, or MSVC), CMake, and the development libraries for SDL2.

### Installation

1.  Clone the repo
    ```sh
    git clone https://github.com/f3ltz/WovenWorld.git
    ```
2.  Navigate to the project directory
    ```sh
    cd Contour-Engine
    ```
3.  Configure and build with CMake
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```
4.  Run the executable from the `build` directory.

## Acknowledgements

* The artistic direction is heavily inspired by the stunning "topographic" artwork seen in modern digital art and design.
