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

#include "SDL.h"
#include "Screen.h"

namespace OpenXcom
{

namespace Flc

{

struct Flc_t {
  FILE *file;
  Uint8 *pMembuf;
  Uint32 membufSize;
  Uint8 *pFrame;
  Uint8 *pChunk;
  Uint16 FrameCount;    /* Frame Counter */
  /*
  */
  Uint32 HeaderSize;    /* Fli file size */
  Uint16 HeaderCheck;   /* Fli header check */
  Uint16 HeaderFrames;  /* Number of frames in flic */
  Uint16 HeaderWidth;   /* Fli width */
  Uint16 HeaderHeight;  /* Fli heigth */
  Uint16 HeaderDepth;   /* Color depth */
  double HeaderSpeed;   /* Number of video ticks between frame */
  Uint32 FrameSize;     /* Frame size in bytes */
  Uint16 FrameCheck;    /* Frame check */
  Uint16 FrameChunks;   /* Number of chunks in frame */
  Uint32 ChunkSize;     /* Size of chunk */
  Uint16 ChunkType;     /* Type of chunk */
  /*
  */
  // FRAME_TYPE extension, see http://www.compuphase.com/flic.htm
  Uint16 DelayOverride;
  //

  SDL_Surface *mainscreen;
  Screen *realscreen;
  SDL_Color colors[256];
  int screen_w;
  int screen_h;
  int screen_depth;
  int loop;
  bool quit;
};

extern struct Flc_t flc;

/// Open FLC or FLI file, read header, prepare to play it
int FlcInit(const char *filename);
/// Play the loaded file; set flc.mainscreen first!
void FlcMain(void (*frameCallBack)());
/// Free memory, free love, etc.
void FlcDeInit();

// lower-level functions:

/// Read the actual file?
void FlcReadFile(Uint32 frameSize);
/// write one frame to screen? still need to call SDL_UpdateRect() though??
void FlcDoOneFrame();

}

}
