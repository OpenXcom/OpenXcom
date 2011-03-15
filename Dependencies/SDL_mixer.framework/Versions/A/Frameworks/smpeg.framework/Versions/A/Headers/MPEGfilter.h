/*
    SMPEG - SDL MPEG Player Library
    Copyright (C) 1999  Loki Entertainment Software

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

/* MPEG filters */

#ifndef _MPEGFILTER_H_
#define _MPEGFILTER_H_

/* SMPEG filter info flags */
#define SMPEG_FILTER_INFO_MB_ERROR    1
#define SMPEG_FILTER_INFO_PIXEL_ERROR 2

/* Filter info from SMPEG */
typedef struct SMPEG_FilterInfo {
  Uint16* yuv_mb_square_error;
  Uint16* yuv_pixel_square_error;
} SMPEG_FilterInfo;

/* MPEG filter definition */
struct SMPEG_Filter;

/* Callback functions for the filter */
typedef void (* SMPEG_FilterCallback)( SDL_Overlay * dest, SDL_Overlay * source, SDL_Rect * region, SMPEG_FilterInfo * filter_info, void * data );
typedef void (* SMPEG_FilterDestroy)( struct SMPEG_Filter * filter );

/* The filter definition itself */
typedef struct SMPEG_Filter {
  Uint32 flags;
  void * data;
  SMPEG_FilterCallback callback;
  SMPEG_FilterDestroy destroy;
} SMPEG_Filter;

/* SMPEG built-in filters. */
#ifdef __cplusplus
extern "C" {
#endif

/* The null filter (default). It simply copies the source rectangle to the video overlay. */ 
extern DECLSPEC SMPEG_Filter * SMPEGfilter_null(void);

/* The bilinear filter. A basic low-pass filter that will produce a smoother image. */ 
extern DECLSPEC SMPEG_Filter * SMPEGfilter_bilinear(void);

/* The deblocking filter. It filters block borders and non-intra coded blocks to reduce blockiness */ 
extern DECLSPEC SMPEG_Filter * SMPEGfilter_deblocking(void);

#ifdef __cplusplus
};
#endif
#endif /* _MPEGFILTER_H_ */
