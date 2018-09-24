set ( sdl2_mixer_src
    "effects_internal.c"
    "effect_position.c"
    "effect_stereoreverse.c"
    "load_aiff.c"
    "load_voc.c"
    "mixer.c"
    "music.c"
    "music_cmd.c"
    "music_flac.c"
    "music_fluidsynth.c"
    "music_mad.c"
    "music_mikmod.c"
    "music_modplug.c"
    "music_mpg123.c"
    "music_nativemidi.c"
    "music_ogg.c"
    "music_smpeg.c"
    "music_timidity.c"
    "music_wav.c"
)

#ADD_DEFINITIONS(-DMUSIC_CMD=1)
ADD_DEFINITIONS(-DMUSIC_WAV=1)
#ADD_DEFINITIONS(-DMUSIC_FLAC=1)
#ADD_DEFINITIONS(-DMUSIC_OGG=1)
#ADD_DEFINITIONS(-DMUSIC_MP3_MPG123=1)
#ADD_DEFINITIONS(-DMUSIC_MP3_MAD=1)
#ADD_DEFINITIONS(-DMUSIC_MP3_SMPEG=1)
#ADD_DEFINITIONS(-DMUSIC_MOD_MODPLUG=1)
#ADD_DEFINITIONS(-DMUSIC_MOD_MIKMOD=1)
#ADD_DEFINITIONS(-DMUSIC_MID_FLUIDSYNTH=1)
#ADD_DEFINITIONS(-DMUSIC_MID_TIMIDITY=1)
#ADD_DEFINITIONS(-DMUSIC_MID_NATIVE=1)

include_directories(
  ${SDL2_SOURCE_DIR}/include
)

add_library(SDL2Mixer STATIC ${sdl2_mixer_src} )

set_target_properties(SDL2Mixer
    PROPERTIES
      FOLDER
        "dep")