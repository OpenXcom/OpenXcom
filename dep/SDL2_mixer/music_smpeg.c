/*
  SDL_mixer:  An audio mixer library based on the SDL library
  Copyright (C) 1997-2017 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifdef MUSIC_MP3_SMPEG

#include "SDL_loadso.h"

#include "music_smpeg.h"

#if SDL_VERSION_ATLEAST(2, 0, 0)
/* Forward declaration for SDL 2.0  because struct is not available there but
   still used in a file included with smpeg.h. May not compile if missing. */
typedef struct SDL_Overlay SDL_Overlay;
#endif

#include "smpeg.h"

typedef struct {
    int loaded;
    void *handle;
    void (*SMPEG_actualSpec)(SMPEG *mpeg, SDL_AudioSpec *spec);
    void (*SMPEG_delete)(SMPEG* mpeg);
    void (*SMPEG_enableaudio)(SMPEG* mpeg, int enable);
    void (*SMPEG_enablevideo)(SMPEG* mpeg, int enable);
    SMPEG* (*SMPEG_new_rwops)(SDL_RWops *src, SMPEG_Info* info, int freesrc, int sdl_audio);
    void (*SMPEG_play)(SMPEG* mpeg);
    int (*SMPEG_playAudio)(SMPEG *mpeg, Uint8 *stream, int len);
    void (*SMPEG_rewind)(SMPEG* mpeg);
    void (*SMPEG_setvolume)(SMPEG* mpeg, int volume);
    void (*SMPEG_skip)(SMPEG* mpeg, float seconds);
    SMPEGstatus (*SMPEG_status)(SMPEG* mpeg);
    void (*SMPEG_stop)(SMPEG* mpeg);
} smpeg_loader;

static smpeg_loader smpeg = {
    0, NULL
};

#ifdef SMPEG_DYNAMIC
#define FUNCTION_LOADER(FUNC, SIG) \
    smpeg.FUNC = (SIG) SDL_LoadFunction(smpeg.handle, #FUNC); \
    if (smpeg.FUNC == NULL) { SDL_UnloadObject(smpeg.handle); return -1; }
#else
#define FUNCTION_LOADER(FUNC, SIG) \
    smpeg.FUNC = FUNC;
#endif

static int SMPEG_Load(void)
{
    if (smpeg.loaded == 0) {
#ifdef SMPEG_DYNAMIC
        smpeg.handle = SDL_LoadObject(SMPEG_DYNAMIC);
        if (smpeg.handle == NULL) {
            return -1;
        }
#elif defined(__MACOSX__)
        extern SMPEG* SMPEG_new_rwops(SDL_RWops*, SMPEG_Info*, int, int) __attribute__((weak_import));
        if (SMPEG_new_rwops == NULL)
        {
            /* Missing weakly linked framework */
            Mix_SetError("Missing smpeg2.framework");
            return -1;
        }
#endif
        FUNCTION_LOADER(SMPEG_actualSpec, void (*)(SMPEG *, SDL_AudioSpec *))
        FUNCTION_LOADER(SMPEG_delete, void (*)(SMPEG*))
        FUNCTION_LOADER(SMPEG_enableaudio, void (*)(SMPEG*, int))
        FUNCTION_LOADER(SMPEG_enablevideo, void (*)(SMPEG*, int))
        FUNCTION_LOADER(SMPEG_new_rwops, SMPEG* (*)(SDL_RWops *, SMPEG_Info*, int, int))
        FUNCTION_LOADER(SMPEG_play, void (*)(SMPEG*))
        FUNCTION_LOADER(SMPEG_playAudio, int (*)(SMPEG *, Uint8 *, int))
        FUNCTION_LOADER(SMPEG_rewind, void (*)(SMPEG*))
        FUNCTION_LOADER(SMPEG_setvolume, void (*)(SMPEG*, int))
        FUNCTION_LOADER(SMPEG_skip, void (*)(SMPEG*, float))
        FUNCTION_LOADER(SMPEG_status, SMPEGstatus (*)(SMPEG*))
        FUNCTION_LOADER(SMPEG_stop, void (*)(SMPEG*))
    }
    ++smpeg.loaded;

    return 0;
}

static void SMPEG_Unload(void)
{
    if (smpeg.loaded == 0) {
        return;
    }
    if (smpeg.loaded == 1) {
#ifdef SMPEG_DYNAMIC
        SDL_UnloadObject(smpeg.handle);
#endif
    }
    --smpeg.loaded;
}


typedef struct
{
    SMPEG *mp3;
    SDL_RWops *src;
    int freesrc;
} SMPEG_Music;

static void *SMPEG_CreateFromRW(SDL_RWops *src, int freesrc)
{
    SMPEG_Music *music;
    SMPEG_Info info;

    music = (SMPEG_Music *)SDL_calloc(1, sizeof(*music));
    if (!music) {
        SDL_OutOfMemory();
        return NULL;
    }
    music->src = src;

    music->mp3 = smpeg.SMPEG_new_rwops(src, &info, SDL_FALSE, 0);
    if (!info.has_audio) {
        Mix_SetError("MPEG file does not have any audio stream.");
        smpeg.SMPEG_delete(music->mp3);
        SDL_free(music);
        return NULL;
    }
    smpeg.SMPEG_actualSpec(mp3, &music_spec);

    music->freesrc = freesrc;
    return music;
}

static void SMPEG_SetVolume(void *context, int volume)
{
    SMPEG_Music *music = (SMPEG_Music *)context;
    smpeg.SMPEG_setvolume(music->mp3,(int)(((float)volume/(float)MIX_MAX_VOLUME)*100.0));
}

static int SMPEG_Play(void *context)
{
    SMPEG_Music *music = (SMPEG_Music *)context;
    smpeg.SMPEG_enableaudio(music->mp3, 1);
    smpeg.SMPEG_enablevideo(music->mp3, 0);
    smpeg.SMPEG_rewind(music->mp3);
    smpeg.SMPEG_play(music->mp3);
    return 0;
}

static SDL_bool SMPEG_IsPlaying(void *context)
{
    SMPEG_Music *music = (SMPEG_Music *)context;
    return smpeg.SMPEG_status(music->mp3) == SMPEG_PLAYING ? SDL_TRUE : SDL_FALSE;
}

static int SMPEG_GetAudio(void *context, void *data, int bytes)
{
    SMPEG_Music *music = (SMPEG_Music *)context;
    Uint8 *stream = (Uint8 *)data;
    int len = bytes;
    int left = (len - smpeg.SMPEG_playAudio(music->mp3, stream, len));
    if (left > 0) {
        stream += (len - left);
    return left;
}

static int SMPEG_Seek(void *context, double position)
{
    SMPEG_Music *music = (SMPEG_Music *)context;
    smpeg.SMPEG_rewind(music->mp3);
    smpeg.SMPEG_play(music->mp3);
    if (position > 0.0) {
        smpeg.SMPEG_skip(music->mp3, (float)position);
    }
    return 0;
}

static void SMPEG_Stop(void *context)
{
    SMPEG_Music *music = (SMPEG_Music *)context;
    smpeg.SMPEG_stop(music->mp3);
}

static void SMPEG_Delete(void *context)
{
    SMPEG_Music *music = (SMPEG_Music *)context;

    smpeg.SMPEG_delete(music->mp3);

    if (music->freesrc) {
        SDL_RWclose(music->src);
    }
    SDL_free(music);
}

Mix_MusicInterface Mix_MusicInterface_SMPEG =
{
    "SMPEG",
    MIX_MUSIC_SMPEG,
    MUS_MP3,
    SDL_FALSE,
    SDL_FALSE,

    SMPEG_Load,
    NULL,   /* Open */
    SMPEG_CreateFromRW,
    NULL,   /* CreateFromFile */
    SMPEG_SetVolume,
    SMPEG_Play,
    SMPEG_IsPlaying,
    SMPEG_GetAudio,
    SMPEG_Seek,
    NULL,   /* Pause */
    NULL,   /* Resume */
    SMPEG_Stop,
    SMPEG_Delete,
    NULL,   /* Close */
    SMPEG_Unload,
};

#endif /* MUSIC_MP3_SMPEG */

/* vi: set ts=4 sw=4 expandtab: */
