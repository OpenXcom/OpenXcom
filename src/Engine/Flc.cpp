/*      vi:set nowrap ts=2 sw=2:
*/
/** COPYING:


Don't care ware... ;)

Something like: "you can do with it what you want to do with it. But if
your cola vaporises you can't sue me. But if you make changes to the code
you can always send to diffs to me"

grt,

- Jasper

 +-----
 | Beheer Commisaris      | Homepage: http://www.il.fontys.nl/~jasper
 | IGV Interlink          | PGP-key:  finger jasper@il.fontys.nl      |
                          | E-mail:   jasper@il.fontys.nl             |
                                                                  ----+
*/

#define version "0.2"
/*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*
*/
#include "SDL.h"
/*
*/
#include "Flc.h"
#include "Logger.h"
#include "Exception.h"
#include "Zoom.h"
#include "../aresame.h"

namespace OpenXcom
{

namespace Flc
{

struct Flc_t flc;

#if 0
void SDLInit(char *header)
{ /* Initialize SDL
  */
  printf("SDL: Version %d.%d.%d.\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);

  if(SDL_Init(SDL_INIT_VIDEO)<0) {
    fprintf(stderr, "SDL: Couldn't initialize: %s\n",SDL_GetError());
    exit(2);
  } else {
    fprintf(stdout, "SDL: Video initialization succeeded.\n");
  }
  atexit(SDL_Quit);
/* Init screen
*/
  if((flc.mainscreen=SDL_SetVideoMode(flc.screen_w, flc.screen_h, flc.screen_depth, (SDL_HWPALETTE|SDL_FULLSCREEN))) == NULL){
    fprintf(stderr, "SDL: Couldn't set video mode %dx%dx%d: %s\n", flc.screen_w, flc.screen_h, flc.screen_depth, SDL_GetError());
    exit(3);
  }

/* Set titlebar and iconbar name
*/
  SDL_WM_SetCaption(header, header);
} /* SDLInit */

#endif


#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define ReadU16(tmp1, tmp2) { Uint8 *sp = (Uint8 *)tmp2, *dp = (Uint8 *)tmp1; dp[0]=sp[1]; dp[1]=sp[0]; }
#define ReadU32(tmp1, tmp2) { Uint8 *sp = (Uint8 *)tmp2, *dp = (Uint8 *)tmp1; dp[0]=sp[3]; dp[1]=sp[2]; dp[2]=sp[1]; dp[3]=sp[0]; }
#else
#define ReadU16(tmp1, tmp2) /* (Uint16) */ (*(tmp1) = ((Uint8)*(tmp2+1)<<8)+(Uint8)*(tmp2));
#define ReadU32(tmp1, tmp2) /* (Uint32) */ (*(tmp1) = (((((((Uint8)*(tmp2+3)<<8)+((Uint8)*(tmp2+2)))<<8)+((Uint8)*(tmp2+1)))<<8)+(Uint8)*(tmp2)));
#endif

void FlcReadFile(Uint32 size)
{ 
if(size>flc.membufSize) {
    if(!(flc.pMembuf=(Uint8*)realloc(flc.pMembuf, size+1))) {
      //printf("Realloc failed: %d\n", size);
      Log(LOG_FATAL) << "Realloc failed: " << size;
      throw Exception("Realloc failed!");
    }
  }

  if(fread(flc.pMembuf, sizeof(Uint8), size, flc.file)==0) {
    //printf("Can't read flx file");
    Log(LOG_ERROR) << "Can't read flx file :(";
		return;
  }
} /* FlcReadFile */

int FlcCheckHeader(const char *filename)
{ 
if((flc.file=fopen(filename, "rb"))==NULL) {
    Log(LOG_ERROR) << "Could not open flx file: " << filename;
		return -1;
  }

  FlcReadFile(128);

  ReadU32(&flc.HeaderSize, flc.pMembuf);
  ReadU16(&flc.HeaderCheck, flc.pMembuf+4);
  ReadU16(&flc.HeaderFrames, flc.pMembuf+6);
  ReadU16(&flc.HeaderWidth, flc.pMembuf+8);
  ReadU16(&flc.HeaderHeight, flc.pMembuf+10);
  ReadU16(&flc.HeaderDepth, flc.pMembuf+12);
  ReadU16(&flc.HeaderSpeed, flc.pMembuf+16);

#ifdef DEBUG
  printf("flc.HeaderSize: %d\n", flc.HeaderSize);
  printf("flc.HeaderCheck: %d\n", flc.HeaderCheck);
  printf("flc.HeaderFrames: %d\n", flc.HeaderFrames);
  printf("flc.HeaderWidth: %d\n", flc.HeaderWidth);
  printf("flc.HeaderHeight: %d\n", flc.HeaderHeight);
  printf("flc.HeaderDepth: %d\n", flc.HeaderDepth);
  printf("flc.HeaderSpeed: %lf\n", flc.HeaderSpeed);
#endif


#if SDL_BYTEORDER == SDL_BIG_ENDIAN

  if((flc.HeaderCheck==0x0AF12) || (flc.HeaderCheck==0x0AF11)) { 
    flc.screen_w=flc.HeaderWidth;
    flc.screen_h=flc.HeaderHeight;
	Log(LOG_INFO) << "Playing flx, " << flc.screen_w << "x" << flc.screen_h << ", " << flc.HeaderFrames << " frames";
    flc.screen_depth=8;
    if(flc.HeaderCheck==0x011AF) {
      flc.HeaderSpeed*=1000.0/70.0;
    }
#else
  if((flc.HeaderCheck==0x0AF12) || (flc.HeaderCheck==0x0AF11)) { 
    flc.screen_w=flc.HeaderWidth;
    flc.screen_h=flc.HeaderHeight;
	Log(LOG_INFO) << "Playing flx, " << flc.screen_w << "x" << flc.screen_h << ", " << flc.HeaderFrames << " frames";
    flc.screen_depth=8;
    if(flc.HeaderCheck==0x0AF11) {
      flc.HeaderSpeed*=1000.0/70.0;
    }
#endif
    return(0);
  }
  return(1);

} /* FlcCheckHeader */

int FlcCheckFrame()
{ 
flc.pFrame=flc.pMembuf+flc.FrameSize-16;
  ReadU32(&flc.FrameSize, flc.pFrame+0);
  ReadU16(&flc.FrameCheck, flc.pFrame+4);
  ReadU16(&flc.FrameChunks, flc.pFrame+6);
  ReadU16(&flc.DelayOverride, flc.pFrame+8); // not actually used in UFOINT.FLI, it turns out

#ifdef DEBUG
  printf("flc.FrameSize: %d\n", flc.FrameSize);
  printf("flc.FrameCheck: %d\n", flc.FrameCheck);
  printf("flc.FrameChunks: %d\n", flc.FrameChunks);
  printf("flc.DelayOverride: %d\n", flc.DelayOverride);
#endif

  flc.pFrame+=16;
  if(flc.FrameCheck==0x0f1fa) { 
    return(0);
  }

  flc.DelayOverride = 0; // not FRAME_TYPE means the value we read wasn't a delay at all

  if(flc.FrameCheck==0x0f100) { 
#ifdef DEBUG
    printf("Ani info!!!\n");
#endif	
    return(0);
  }

  return(1);
//#else
//  return(0);
//#endif
} /* FlcCheckFrame */

void COLORS256()
{ Uint8 *pSrc;
  Uint16 NumColorPackets;
  Uint16 NumColors;
  Uint8 NumColorsSkip;
  int i;

  pSrc=flc.pChunk+6;
  ReadU16(&NumColorPackets, pSrc);
  pSrc+=2;
  while(NumColorPackets--) {
    NumColorsSkip=*(pSrc++);
    if(!(NumColors=*(pSrc++))) {
      NumColors=256;
    }
    i=0;
    while(NumColors--) {
      flc.colors[i].r=*(pSrc++);
      flc.colors[i].g=*(pSrc++);
      flc.colors[i].b=*(pSrc++);
      i++;
    }
	flc.realscreen->setPalette(flc.colors, NumColorsSkip, i);
	SDL_SetColors(flc.mainscreen, flc.colors, NumColorsSkip, i);
	flc.realscreen->getSurface(); // force palette update to really happen
  }
} /* COLORS256 */

void SS2()
{ Uint8 *pSrc, *pDst, *pTmpDst;
  Sint8 CountData;
  Uint8 ColumSkip, Fill1, Fill2;
  Uint16 Lines, Count;

  pSrc=flc.pChunk+6;
  pDst=(Uint8*)flc.mainscreen->pixels + flc.offset;
  ReadU16(&Lines, pSrc);
  
  pSrc+=2;
  while(Lines--) {
    ReadU16(&Count, pSrc);
    pSrc+=2;

    while(Count & 0xc000) {
/* Upper bits 11 - Lines skip 
*/
      if((Count & 0xc000)==0xc000) {  // 0xc000h = 1100000000000000
        pDst+=(0x10000-Count)*flc.mainscreen->pitch;
      }

      if((Count & 0xc000)==0x4000) {  // 0x4000h = 0100000000000000
/* Upper bits 01 - Last pixel
*/
#ifdef DEBUG
            printf("Last pixel not implemented");
#endif
      }
      ReadU16(&Count, pSrc);
      pSrc+=2;
    }

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    if((Count & 0x00c0)==0x0000) {      // 0xc000h = 1100000000000000
#else
    if((Count & 0xc000)==0x0000) {      // 0xc000h = 1100000000000000
#endif
      pTmpDst=pDst;
      while(Count--) {
        ColumSkip=*(pSrc++);
        pTmpDst+=ColumSkip;
        CountData=*(pSrc++);
        if(CountData>0) {
          while(CountData--) {
            *(pTmpDst++)=*(pSrc++);
            *(pTmpDst++)=*(pSrc++);
          }
        } else { 
          if(CountData<0) {
            CountData=(0x100-CountData);
            Fill1=*(pSrc++);
            Fill2=*(pSrc++);
            while(CountData--) {
              *(pTmpDst++)=Fill1;
              *(pTmpDst++)=Fill2;
            }
          }
        }
      }
      pDst+=flc.mainscreen->pitch;
    } 
  }
} /* SS2 */

void DECODE_BRUN()
{ Uint8 *pSrc, *pDst, *pTmpDst, Fill;
  Sint8 CountData;
  int HeightCount, PacketsCount;

  HeightCount=flc.HeaderHeight;
  pSrc=flc.pChunk+6;
  pDst=(Uint8*)flc.mainscreen->pixels + flc.offset;
  while(HeightCount--) {
    pTmpDst=pDst;
    PacketsCount=*(pSrc++);
    while(PacketsCount--) {
      CountData=*(pSrc++);
      if(CountData>0) {
        Fill=*(pSrc++);
        while(CountData--) {
          *(pTmpDst++)=Fill;
        }
      } else { 
        if(CountData<0) {
          CountData=(0x100-CountData);
          while(CountData--) {
          *(pTmpDst++)=*(pSrc++);
          }
        }
      }
    }
    pDst+=flc.mainscreen->pitch;
  }
} /* DECODE_BRUN */


void DECODE_LC() 
{ Uint8 *pSrc, *pDst, *pTmpDst;
  Sint8 CountData;
  Uint8 CountSkip;
  Uint8 Fill;
  Uint16 Lines, tmp;
  int PacketsCount;

  pSrc=flc.pChunk+6;
  pDst=(Uint8*)flc.mainscreen->pixels + flc.offset;

  ReadU16(&tmp, pSrc);
  pSrc+=2;
  pDst+=tmp*flc.mainscreen->pitch;
  ReadU16(&Lines, pSrc);
  pSrc+=2;
  while(Lines--) {
    pTmpDst=pDst;
    PacketsCount=*(pSrc++);
    while(PacketsCount--) {
      CountSkip=*(pSrc++);
      pTmpDst+=CountSkip;
      CountData=*(pSrc++);
      if(CountData>0) {
        while(CountData--) {
          *(pTmpDst++)=*(pSrc++);
        }
      } else { 
        if(CountData<0) {
          CountData=(0x100-CountData);
          Fill=*(pSrc++);
          while(CountData--) {
            *(pTmpDst++)=Fill;
          }
        }
      }
    }
    pDst+=flc.mainscreen->pitch;
  }
} /* DECODE_LC */

void DECODE_COLOR()
{ Uint8 *pSrc;
  Uint16 NumColors, NumColorPackets;
  Uint8 NumColorsSkip;
  int i;

  pSrc=flc.pChunk+6;
  ReadU16(&NumColorPackets, pSrc);
  pSrc+=2;
  while(NumColorPackets--) {
    NumColorsSkip=*(pSrc++);
    if(!(NumColors=*(pSrc++))) {
      NumColors=256;
    }
    i=0;
    while(NumColors--) {
      flc.colors[i].r=*(pSrc++)<<2;
      flc.colors[i].g=*(pSrc++)<<2;
      flc.colors[i].b=*(pSrc++)<<2;
      i++;
    }
	flc.realscreen->setPalette(flc.colors, NumColorsSkip, i);
    SDL_SetColors(flc.mainscreen, flc.colors, NumColorsSkip, i);
	flc.realscreen->getSurface(); // force palette update to really happen
  }
} /* DECODE_COLOR  */


void DECODE_COPY()
{ Uint8 *pSrc, *pDst;
  int Lines = flc.screen_h;
  pSrc=flc.pChunk+6;
  pDst=(Uint8*)flc.mainscreen->pixels + flc.offset;
  while(Lines-- > 0) {
    memcpy(pDst, pSrc, flc.screen_w);
    pSrc+=flc.screen_w;
    pDst+=flc.mainscreen->pitch;
  }
} /* DECODE_COPY */

void BLACK()
{ Uint8 *pDst;
  int Lines = flc.screen_h;
  pDst=(Uint8*)flc.mainscreen->pixels + flc.offset;
  while(Lines-- > 0) {
    memset(pDst, 0, flc.screen_w);
    pDst+=flc.mainscreen->pitch;
  }
} /* BLACK */


void FlcDoOneFrame()
{ int ChunkCount; 
  ChunkCount=flc.FrameChunks;
  flc.pChunk=flc.pMembuf;
  if ( SDL_LockSurface(flc.mainscreen) < 0 )
    return;
  // if (!ChunkCount) printf("Empty frame! %d\n", flc.FrameCount); // this is normal and used for delays
  while(ChunkCount--) {
    ReadU32(&flc.ChunkSize, flc.pChunk+0);
    ReadU16(&flc.ChunkType, flc.pChunk+4);

#ifdef DEBUG
    printf("flc.ChunkSize: %d\n", flc.ChunkSize);
    printf("flc.ChunkType: %d aka %x\n", flc.ChunkType, flc.ChunkType);
	if (flc.DelayOverride) printf("DelayOverride: %d\n", flc.DelayOverride);
#endif

    switch(flc.ChunkType) {
      case 4:
        COLORS256();
      break;
      case 7:
        SS2();
      break;
      case 11:
        DECODE_COLOR();
      break;
      case 12:
        DECODE_LC();
      break;
      case 13:
        BLACK();
      break;
      case 15:
        DECODE_BRUN();
      break;
      case 16:
        DECODE_COPY();
      break;
      case 18:
#ifdef DEBUG
        printf("Chunk 18 not yet done.\n");
#endif
      break;
      default:
        Log(LOG_WARNING) << "Ieek an non implemented chunk type:" << flc.ChunkType;
    }
    flc.pChunk+=flc.ChunkSize;
  }
  SDL_UnlockSurface(flc.mainscreen);
} /* FlcDoOneFrame */

void SDLWaitFrame(void)
{ 
//#ifndef __NO_FLC
static double oldTick=0.0;
  Uint32 currentTick;
  double waitTicks;
  double delay = flc.DelayOverride ? flc.DelayOverride : flc.HeaderSpeed;

	if ( AreSame(oldTick, 0.0) ) oldTick = SDL_GetTicks();

	currentTick=SDL_GetTicks(); 
	waitTicks=(oldTick+=(delay))-currentTick;


	do {
		waitTicks = (oldTick + delay - SDL_GetTicks());

		if(waitTicks > 0.0) {
			//SDL_Delay(floor(waitTicks + 0.5)); // biased rounding? mehhh?
			SDL_Delay(1);
		}
	} while (waitTicks > 0.0); 
//#endif
} /* SDLWaitFrame */

void FlcInitFirstFrame()
{ flc.FrameSize=16;
  flc.FrameCount=0;
  if(fseek(flc.file, 128, SEEK_SET)) {
    //printf("Fseek read failed\n");
    throw Exception("Fseek read failed for flx file");
  };
  FlcReadFile(flc.FrameSize);
} /* FlcInitFirstFrame */

int FlcInit(const char *filename)
{ flc.pMembuf=NULL;
  flc.membufSize=0;

  if(FlcCheckHeader(filename)) {
    Log(LOG_ERROR) << "Flx file failed header check.";
    //exit(1);
	return -1;
  }
  if (flc.realscreen->getSurface()->getSurface()->format->BitsPerPixel == 8)
  {
	  flc.mainscreen = flc.realscreen->getSurface()->getSurface();
  } else
  {
	  flc.mainscreen = SDL_AllocSurface(SDL_SWSURFACE, flc.screen_w, flc.screen_h, 8, 0, 0, 0, 0);
  }
  return 0;
  //SDLInit(filename);
} /* FlcInit */

void FlcDeInit()
{ 
	if (flc.mainscreen != flc.realscreen->getSurface()->getSurface()) SDL_FreeSurface(flc.mainscreen);
	fclose(flc.file);
	free(flc.pMembuf);
} /* FlcDeInit */

void FlcMain(void (*frameCallBack)())
{ flc.quit=false;
  SDL_Event event;
  
//#ifndef __NO_FLC
  FlcInitFirstFrame();
  flc.offset = flc.dy*flc.mainscreen->pitch + flc.mainscreen->format->BytesPerPixel*flc.dx;
  while(!flc.quit) {
	if (frameCallBack) (*frameCallBack)();
    flc.FrameCount++;
    if(FlcCheckFrame()) {
      if (flc.FrameCount<=flc.HeaderFrames) {
        //printf("Frame failure -- corrupt file?\n");
        Log(LOG_ERROR) << "Frame failure -- corrupt file?";
				return;
      } else {
        if(flc.loop)
          FlcInitFirstFrame();
        else {
          SDL_Delay(1000);
          flc.quit=true;
        }
        continue;
      }
    }

    FlcReadFile(flc.FrameSize);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    if(flc.FrameCheck!=0x000f1) {
#else
    if(flc.FrameCheck!=0x0f100) {
#endif
      FlcDoOneFrame();
      SDLWaitFrame();
      /* TODO: Track which rectangles have really changed */
      //SDL_UpdateRect(flc.mainscreen, 0, 0, 0, 0);
      if (flc.mainscreen != flc.realscreen->getSurface()->getSurface()) SDL_BlitSurface(flc.mainscreen, 0, flc.realscreen->getSurface()->getSurface(), 0);
      flc.realscreen->flip();
    }

	bool finalFrame = !flc.loop && (flc.FrameCount == flc.HeaderFrames);
	Uint32 pauseStart = 0;
	if (finalFrame) pauseStart = SDL_GetTicks();

	do 
	{
		while(SDL_PollEvent(&event)) {
		  switch(event.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_KEYDOWN:
			  return;
			break;
			case SDL_QUIT:
			  exit(0);
			default:
			break;
		  }
		}
		if (finalFrame) SDL_Delay(50);
	} while (!flc.quit && finalFrame && SDL_GetTicks() - pauseStart < 10000); // 10 sec pause but we're actually just fading out and going to main menu when the music ends
	if (finalFrame) flc.quit = true;;
  }
//#endif
} /* FlcMain */


#if 0
void FlxplayHelp()
{ printf("FLX player (%s) with SDL output (jasper@il.fontys.nl)\n", version);
  printf("View readme file for more information\n\n");
  printf("flxplay [-l] [filename]\n");
  exit(1);
} /* FlxplayHelp */


main(int argc, char **argv)
{ int c;

  flc.loop=0;
  for(c = 1; argv[c] && (argv[c][0] == '-'); ++c) {
    if(strcmp(argv[c], "-l") == 0) {
      printf("Looping mode\n");
      flc.loop = 1;
    } else {
      FlxplayHelp();
    }
  }
  if(!argv[c]) {
    FlxplayHelp();
  }

  FlcInit(argv[c]);
  FlcMain();
  FlcDeInit();
  exit(0);
} /* main */
#endif

}

}
