To successfully build an OpenXcom OSX bundle you will need the relevant Xcode project files provided herewith as well as all the relevant dependencies installed in /usr/local/.

The dependencies you require are following:
- SDL (libsdl1.2)
- SDL_mixer (libsdl-mixer1.2)
- SDL_gfx (libsdl-gfx1.2), version 2.0.22 or later
- SDL_image (libsdl-image1.2)
- yaml-cpp, version 0.5 or later 

If you want to save yourself the trouble - don’t do it manually, use Homebrew package manager - http://mxcl.github.com/homebrew/ or MacPorts - https://www.macports.org/ instead. 

This guide assumes you listened to the previous recommendation and had your dependencies installed with brew terminal command (for Homebrew), like this:
```
$ brew install cmake yaml-cpp —-with-static-lib sdl sdl_gfx sdl_image sdl_mixer --with-flac --with-libmikmod --with-libvorbis --with-static-lib
```
or port terminal command (for MacPorts), like this:
```
$ sudo port install yaml-cpp+universal libsdl+universal libsdl_gfx+universal libsdl_image+universal libsdl_mixer+universal
```

1. Open the `OpenxXcom.xcodeproj` with Xcode.  Right click on the project (on the left pane) and click 'Add files to "OpenxXcom"'.  Select the 'src' directory from the root of the project.  Don't copy the files and do create folder groups.

1. Push the ‘Build’ button and cross your fingers. Congratulations, you have a working build! But it is still unplayable though.

1. Find your newly created bundle in ‘./Build/Products/Release’ folder or otherwise 'DerivedData' (find it within your Xcode preferences), and copy it to some place you feel comfortable keeping your games in.

If you want to use the portable bundle, then:

1. Copy the relevant original game data files you possess into the ‘UFO’ (for UFO: Enemy Unknown) and/or ‘TFTD’ (for XCOM: Terror from the Deep) folder within the bundle respectively. If asked by the system what to do with the files and folders already present there, select ‘Merge’.

Or, if you prefer to keep your build always separate from the game data files for various reasons: 

1. Create the directory `/Users/Shared/OpenXcom/` and move your original xcom data files (UFO and TFTD directories) there.


1. Enjoy!

Should you require any further assistance regarding the OSX build - feel free to ask around on the official forums - http://openxcom.org/forum/ or contact us directly on GitHub - http://github.com/luciderous, http://github.com/rcreasey
