#pragma once

// uncomment to check memory leaks in VS
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

// c++ headers
#ifdef _MSC_VER
#ifndef _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#endif
#define _CRT_SECURE_NO_WARNINGS
#endif
#define _USE_MATH_DEFINES
#include <algorithm>
#include <assert.h>
#include <bitset>
#include <cassert>
#include <cctype>
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <errno.h>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <locale>
#include <map>
#include <memory.h>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <typeinfo>
#include <utility>
#include <vector>
#include <wchar.h>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <malloc.h>
#else
#include <dirent.h>
#include <execinfo.h>
#include <pwd.h>
#include <sys/param.h>
#include <signal.h>
#include <unistd.h>
#endif

#ifdef __MORPHOS__
#include <ppcinline/exec.h>
#endif

#if (_MSC_VER >= 1400) || (defined(__MINGW32__) && defined(__SSE2__))

#ifndef __SSE2__
#define __SSE2__ true
#endif
// probably Visual Studio (or Intel C++ which should also work)
#include <intrin.h>
#endif

#ifdef __GNUC__
#if (__i386__ || __x86_64__)
#include <cpuid.h>
#endif
#endif

#ifdef __SSE2__
#include <emmintrin.h> // for SSE2 intrinsics; see http://msdn.microsoft.com/en-us/library/has3d153%28v=vs.71%29.aspx
#endif

// library headers
#ifndef __NO_OPENGL
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#endif
#include <SDL_opengl.h>
#endif

#include <SDL.h>
#include <SDL_endian.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_image.h>
#include <SDL_keysym.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include <SDL_syswm.h>
#include <SDL_thread.h>
#include <SDL_types.h>
#include <SDL_video.h>
#include <yaml-cpp/yaml.h>
#include "./dirent.h"
#include "./lodepng.h"

// uncomment to check memory leaks in VS
//#ifndef DBG_NEW
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//#define new DBG_NEW
//#endif

// openxcom headers
#include "./fmath.h"
#include "./version.h"

#include "./Engine/Adlib/adlplayer.h"
#include "./Engine/Adlib/fmopl.h"
#include "./Engine/Scalers/common.h"
#include "./Engine/Scalers/config.h"
#include "./Engine/Scalers/hqx.h"
#include "./Engine/Scalers/scale2x.h"
#include "./Engine/Scalers/scale3x.h"
#include "./Engine/Scalers/scalebit.h"
#include "./Engine/Scalers/xbrz.h"
#include "./Engine/Action.h"
#include "./Engine/AdlibMusic.h"
#include "./Engine/CatFile.h"
#include "./Engine/CrossPlatform.h"
#include "./Engine/Exception.h"
#include "./Engine/FastLineClip.h"
#include "./Engine/FileMap.h"
#include "./Engine/FlcPlayer.h"
#include "./Engine/Font.h"
#include "./Engine/Game.h"
#include "./Engine/GMCat.h"
#include "./Engine/GraphSubset.h"
#include "./Engine/InteractiveSurface.h"
#include "./Engine/Language.h"
#include "./Engine/LanguagePlurality.h"
#include "./Engine/LocalizedText.h"
#include "./Engine/Logger.h"
#include "./Engine/ModInfo.h"
#include "./Engine/Music.h"
#include "./Engine/OpenGL.h"
#include "./Engine/OptionInfo.h"
#include "./Engine/Options.h"
#include "./Engine/Palette.h"
#include "./Engine/RNG.h"
#include "./Engine/Screen.h"
#include "./Engine/ShaderDraw.h"
#include "./Engine/ShaderDrawHelper.h"
#include "./Engine/ShaderMove.h"
#include "./Engine/ShaderRepeat.h"
#include "./Engine/Sound.h"
#include "./Engine/SoundSet.h"
#include "./Engine/State.h"
#include "./Engine/Surface.h"
#include "./Engine/SurfaceSet.h"
#include "./Engine/Timer.h"
#include "./Engine/Unicode.h"
#include "./Engine/Zoom.h"
#include "./Interface/ArrowButton.h"
#include "./Interface/Bar.h"
#include "./Interface/BattlescapeButton.h"
#include "./Interface/ComboBox.h"
#include "./Interface/Cursor.h"
#include "./Interface/FpsCounter.h"
#include "./Interface/Frame.h"
#include "./Interface/ImageButton.h"
#include "./Interface/NumberText.h"
#include "./Interface/ScrollBar.h"
#include "./Interface/Slider.h"
#include "./Interface/Text.h"
#include "./Interface/TextButton.h"
#include "./Interface/TextEdit.h"
#include "./Interface/TextList.h"
#include "./Interface/ToggleTextButton.h"
#include "./Interface/Window.h"
