/*
    SMPEG - SDL MPEG Player Library
    Copyright (C) 1999  Loki Entertainment Software
    
    - Modified by Michel Darricau from eProcess <mdarricau@eprocess.fr>  for popcorn -

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* A class based on the MPEG stream class, used to parse and play video */

#ifndef _MPEGVIDEO_H_
#define _MPEGVIDEO_H_

#include "SDL.h"
#include "SDL_thread.h"
#include "MPEGerror.h"
#include "MPEGaction.h"

class MPEGstream;

/* This is the MPEG video stream structure in the mpeg_play code */
struct vid_stream;
typedef struct vid_stream VidStream;

/* Temporary definition of time stamp structure. */

typedef double TimeStamp;

class MPEGvideo : public MPEGerror, public MPEGvideoaction {

    /* Thread to play the video asynchronously */
    friend int Play_MPEGvideo(void *udata);

    /* Various mpeg_play functions that need our data */
    friend VidStream* mpegVidRsrc( TimeStamp time_stamp, VidStream* vid_stream, int first );
    friend int get_more_data( VidStream* vid_stream );

public:
    MPEGvideo(MPEGstream *stream);
    virtual ~MPEGvideo();

    /* MPEG actions */
    void Play(void);
    void Stop(void);
    void Rewind(void);
    void ResetSynchro(double time);
     void Skip(float seconds);
		/* Michel Darricau from eProcess <mdarricau@eprocess.fr> conflict name in popcorn */
    MPEGstatus GetStatus(void);

    /* MPEG video actions */
    bool GetVideoInfo(MPEG_VideoInfo *info);
    bool SetDisplay(SDL_Surface *dst, SDL_mutex *lock,
                                            MPEG_DisplayCallback callback);
    void MoveDisplay(int x, int y);
    void ScaleDisplayXY(int w, int h);
    void SetDisplayRegion(int x, int y, int w, int h);
    void RenderFrame(int frame);
    void RenderFinal(SDL_Surface *dst, int x, int y);
    SMPEG_Filter * Filter(SMPEG_Filter * filter);

    /* Display and sync functions */
    void DisplayFrame( VidStream* vid_stream );
    void ExecuteDisplay( VidStream* vid_stream );
    int timeSync( VidStream* vid_stream );

    /* Yes, it's a hack.. */
    MPEGaudioaction *TimeSource(void ) {
        return time_source;
    }

protected:
    MPEGstream *mpeg;

    VidStream* _stream;
    SDL_Surface* _dst;
    SDL_mutex* _mutex;
    SDL_Thread* _thread;

    MPEG_DisplayCallback _callback;

    int _ow;            // original width of the movie
    int _oh;            // original height of the movie
    int _w;             // mb aligned width of the movie
    int _h;             // mb aligned height of the movie
    SDL_Rect _srcrect;	// source area
    SDL_Rect _dstrect;	// display area
    SDL_Overlay *_image;// source image
    float _fps;         // frames per second
    SMPEG_Filter * _filter; // pointer to the current filter used
    SDL_mutex* _filter_mutex; // make sure the filter is not changed while being used

    void RewindStream(void);
};

#endif /* _MPEGVIDEO_H_ */
