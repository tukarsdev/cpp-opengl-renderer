# C++ OpenGL Renderer

## Overview

### See the world

![Screenshot of world](/screenshot.png)

The actual world certainly isn't beautiful, but it is a showcase of various OpenGL skills, such as mesh instantiation, loading a mesh from a file vs through math, various light forms such as directional, point, and spot lights, dynamic mesh animation, and shadow mapping.

### Navigating the scene:

Press `Esc` to disable mouse locking, and then `Esc` again to re-enable mouse locking.

Use `WASD`, `Ctrl` and `Space` to move the camera:
* `W`: Move forward
* `A`: Move left
* `S`: Move backwards
* `D`: Move right
* `Ctrl`: Move down
* `Space`: Move up

Move the mouse to look around the scene.

#### Using lights in the scene

There are five lights in the scene, you can select these using the numbers `1` through `5`.
* `1` is a yellow spot light
* `2` is a blue spot light
* `3` is a neutral, weaker spotlight with larger radius
* `4` is a natural warm point light
* `5` is a blue point light

To use these lights, go the position in the scene you would like to see the light, and point at where you would like the light to shine. Then click the left mouse button and the light will shine at that location. You can then move to see the shadow effect of the spot light. Note that point lights shine in all directions, so it doesn't matter where you are looking!

You can move items in the scene by pressing a number key `0` through `9`, and using the arrow keys. This will move the object around the scene.

### Known design problems

Currently the edges of shadows are pixelated, but I don't really know exactly how to fix it. It also appears that sometimes shadows won't render on the edges of objects where they should be.

## Building and running the project

**Ensure you have the necessary [dependencies](#dependencies) before building the project.**

To do a release build: `python build.py`

To do a debug build: `python build.py --debug`

To do a clean release: `python build.py --clean`

To do a clean debug: `python build.py --clean --debug`

On linux, you need to use `python3` instead of `python`.

The build script will run the executable immediately.

Tested on windows, ubuntu, fedora (headless), and arch (headless).

On windows you will need to use visual studio to download the necessary SDK and MSVC dependencies.

### Dependencies

To build this project you need `cmake`.

### Windows

Builds can be done with either `Ninja` or `Visual Studio 17 2022`.

### Docker

A docker build is available if you want to avoid downloading the packages directly onto your system.
You will need `docker` installed.

### Linux

The following lists the packages you need for your operating system of choice, as well as the install command.

#### Ubuntu (`apt` based)

* `pkg-config`
* `libgl1-mesa-dev`
* `libwayland-dev`
* `wayland-protocols`
* `libxkbcommon-dev`
* `libx11-dev`
* `libxrandr-dev`
* `libxinerama-dev`
* `libxcursor-dev`
* `libxi-dev`

```bash
sudo apt install pkg-config libgl1-mesa-dev libwayland-dev wayland-protocols libxkbcommon-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

#### Fedora (`dnf` based)

* `pkgconfig`
* `mesa-libGL-devel`
* `wayland-devel`
* `wayland-protocols-devel`
* `libxkbcommon-devel`
* `libX11-devel`
* `libXrandr-devel`
* `libXinerama-devel`
* `libXcursor-devel`
* `libXi-devel`

```bash
sudo dnf install pkgconfig mesa-libGL-devel wayland-devel wayland-protocols-devel libxkbcommon-devel libX11-devel libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel
```

#### Arch (`pacman` based)

* `pkg-config`
* `libglvnd`
* `wayland`
* `wayland-protocols`
* `libxkbcommon`
* `libx11`
* `libxrandr`
* `libxinerama`
* `libxcursor`
* `libxi`
 
```bash
sudo pacman -S pkg-config libglvnd wayland wayland-protocols libxkbcommon libx11 libxrandr libxinerama libxcursor libxi
```

