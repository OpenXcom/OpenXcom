# Building for MacOS

## Dependencies

To successfully build an OpenXcom macOS bundle you will need relevant dependencies installed:

The dependencies you require are following:
- cmake, version 3.12.0 or later
- SDL (libsdl1.2)
- SDL_mixer (libsdl-mixer1.2)
- SDL_gfx (libsdl-gfx1.2), version 2.0.22 or later
- SDL_image (libsdl-image1.2)
- yaml-cpp, version 0.5 or later

CMake is used to configure the project. Download it [manually](https://cmake.org/download/) or install it with [Homebrew](https://brew.sh) (`brew install cmake`).

The easiest way of getting library dependencies is [MacPorts](https://www.macports.org/):
```sh
port -N install libsdl -compat +legacy libsdl_gfx libsdl_image libsdl_mixer yaml-cpp
```

Following instructions assume MacPorts is installed in default prefix `/opt/local`. If you installed ports into different location, replace it.

## Building
This project has two ways to build: one with make and the other with Xcode.  The make way is typically used for CI builds and produces a similar artifact as the Linux build.  If you're just looking to get a quick build, this is a good way to do so.  The Xcode way is useful if you're developing features (or bug fixes) and require the need for a debugger or an IDE experience.

### Make
1. In the root of the repository, generate the Makefile with cmake (Release is also a valid build type):
    ```
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/opt/local -B build .
    ```
1. Copy the vanilla game assets (UFO/TFTD) into the build's `bin/` directory. If the game is next to the repo, cd into game and run:
    ```sh
    cp  -r GEODATA GEOGRAPH MAPS ROUTES SOUND TERRAIN UFOGRAPH UFOINTRO* UNITS ../OpenXcom/bin/UFO/
    ```
1. Build the project (2 being the number of threads to run):
    ```sh
    cmake --build build -j2
    ```
1. Make sure that you set up your [Data directory](https://github.com/OpenXcom/OpenXcom#directory-locations) properly.
1. Your artifact will be named `openxcom.app` in the build target directory.


### Xcode
1. In the root of the repository, generate the XCode project file with cmake (Release is also a valid build type):
    ```sh
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/opt/local -G Xcode -B build .
    ```
1. Open the `OpenXcom.xcodeproj` in Xcode.
1. Copy the vanilla game assets (UFO/TFTD) into the build's `bin/` directory.
1. Build with cmd-B
1. Your artifact will be named `openxcom.app` in the build target directory (Debug or Release).

## Additional Help
Should you require any further assistance regarding the macOS build - feel free to ask around on the official forums - https://openxcom.org/forum/ or contact us directly on GitHub - [@rcreasey](http://github.com/rcreasey)
