# Building for MacOS

## Dependencies
To successfully build an OpenXcom OSX bundle you will need to ensure all the relevant dependencies installed in /usr/local/.

The dependencies you require are following:
- cmake, version 3.12.0 or later
- SDL (libsdl1.2)
- SDL_mixer (libsdl-mixer1.2)
- SDL_gfx (libsdl-gfx1.2), version 2.0.22 or later
- SDL_image (libsdl-image1.2)
- yaml-cpp, version 0.5 or later

We recommend using [Homebrew](https://brew.sh) to facilitate this.  With a working brew utility installed, run the following:
```
$ brew install cmake yaml-cpp --with-static-lib sdl sdl_gfx sdl_image sdl_mixer --with-flac --with-libmikmod --with-libvorbis --with-static-lib
```
This should install all of these necessary dependencies to their appropriate place under /usr/local.

## Building
This project has two ways to build: one with make and the other with Xcode.  The make way is typically used for CI builds and produces a similar artifact as the Linux build.  If you're just looking to get a quick build, this is a good way to do so.  The Xcode way is useful if you're developing features (or bug fixes) and require the need for a debugger or an IDE experience.

### Make
1. In the root of the repository, generate the Makefile with cmake (Release is also a valid build type):
```
cmake -DCMAKE_BUILD_TYPE=Debug -B build .
```
1. Copy the vanilla game assets (UFO/TFTD) into the build's `bin/` directory.
1. Build the project (2 being the number of threads to run):
```
cmake --build build -j2
```
1. Make sure that you set up your [Data directory](https://github.com/OpenXcom/OpenXcom#directory-locations) properly.
1. Your artifact will be named `openxcom.app` in the build target directory.


### Xcode
1. In the root of the repository, generate the XCode project file with cmake (Release is also a valid build type):
```
cmake -DCMAKE_BUILD_TYPE=Debug -G Xcode -B build .
```
1. Open the `OpenXcom.xcodeproj` in Xcode.
1. Copy the vanilla game assets (UFO/TFTD) into the build's `bin/` directory.
1. Build with cmd-B
1. Your artifact will be named `openxcom.app` in the build target directory (Debug or Release).

## Additional Help
Should you require any further assistance regarding the OSX build - feel free to ask around on the official forums - https://openxcom.org/forum/ or contact us directly on GitHub - [@rcreasey](http://github.com/rcreasey)
