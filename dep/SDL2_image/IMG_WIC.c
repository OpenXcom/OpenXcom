/*
  SDL_image:  An example image loading library for use with SDL
  Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

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

#if defined(SDL_IMAGE_USE_WIC_BACKEND)

#include "SDL_image.h"
#include <wincodec.h>

extern "C" {

#define SAFE_RELEASE(X) if ((X)) { (X)->Release(); }
#define DONE_IF_FAILED(X) if (FAILED((X))) { goto done; }

IWICImagingFactory2* wicFactory = NULL;

int WIC_Init()
{
    if (wicFactory == NULL) {
        HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
        if (FAILED(hr)) {
            return -1;
        }
    }

    return 0;
}

void WIC_Quit()
{
    SAFE_RELEASE(wicFactory);
}

int IMG_InitPNG()
{
    return WIC_Init();
}

void IMG_QuitPNG()
{
    WIC_Quit();
}

int IMG_InitJPG()
{
    return WIC_Init();
}

void IMG_QuitJPG()
{
    WIC_Quit();
}

int IMG_InitTIF()
{
    return WIC_Init();
}

void IMG_QuitTIF()
{
    WIC_Quit();
}

int IMG_isPNG(SDL_RWops *src)
{
    Sint64 start;
    int is_PNG;
    Uint8 magic[4];

    if ( !src ) {
        return 0;
    }

    start = SDL_RWtell(src);
    is_PNG = 0;
    if ( SDL_RWread(src, magic, 1, sizeof(magic)) == sizeof(magic) ) {
        if ( magic[0] == 0x89 &&
             magic[1] == 'P' &&
             magic[2] == 'N' &&
             magic[3] == 'G' ) {
            is_PNG = 1;
        }
    }
    SDL_RWseek(src, start, RW_SEEK_SET);
    return(is_PNG);
}

int IMG_isJPG(SDL_RWops *src)
{
    Sint64 start;
    int is_JPG;
    int in_scan;
    Uint8 magic[4];

    /* This detection code is by Steaphan Greene <stea@cs.binghamton.edu> */
    /* Blame me, not Sam, if this doesn't work right. */
    /* And don't forget to report the problem to the the sdl list too! */

    if (!src)
        return 0;
    start = SDL_RWtell(src);
    is_JPG = 0;
    in_scan = 0;
    if (SDL_RWread(src, magic, 2, 1)) {
        if ((magic[0] == 0xFF) && (magic[1] == 0xD8)) {
            is_JPG = 1;
            while (is_JPG == 1) {
                if (SDL_RWread(src, magic, 1, 2) != 2) {
                    is_JPG = 0;
                }
                else if ((magic[0] != 0xFF) && (in_scan == 0)) {
                    is_JPG = 0;
                }
                else if ((magic[0] != 0xFF) || (magic[1] == 0xFF)) {
                    /* Extra padding in JPEG (legal) */
                    /* or this is data and we are scanning */
                    SDL_RWseek(src, -1, RW_SEEK_CUR);
                }
                else if (magic[1] == 0xD9) {
                    /* Got to end of good JPEG */
                    break;
                }
                else if ((in_scan == 1) && (magic[1] == 0x00)) {
                    /* This is an encoded 0xFF within the data */
                }
                else if ((magic[1] >= 0xD0) && (magic[1] < 0xD9)) {
                    /* These have nothing else */
                }
                else if (SDL_RWread(src, magic + 2, 1, 2) != 2) {
                    is_JPG = 0;
                }
                else {
                    /* Yes, it's big-endian */
                    Sint64 innerStart;
                    Uint32 size;
                    Sint64 end;
                    innerStart = SDL_RWtell(src);
                    size = (magic[2] << 8) + magic[3];
                    end = SDL_RWseek(src, size - 2, RW_SEEK_CUR);
                    if (end != innerStart + size - 2) is_JPG = 0;
                    if (magic[1] == 0xDA) {
                        /* Now comes the actual JPEG meat */
#ifdef  FAST_IS_JPEG
                        /* Ok, I'm convinced.  It is a JPEG. */
                        break;
#else
                        /* I'm not convinced.  Prove it! */
                        in_scan = 1;
#endif
                    }
                }
            }
        }
    }
    SDL_RWseek(src, start, RW_SEEK_SET);
    return(is_JPG);
}

int IMG_isTIF(SDL_RWops* src)
{
    Sint64 start;
    int is_TIF;
    Uint8 magic[4];

    if (!src)
        return 0;
    start = SDL_RWtell(src);
    is_TIF = 0;
    if (SDL_RWread(src, magic, 1, sizeof(magic)) == sizeof(magic)) {
        if ((magic[0] == 'I' &&
            magic[1] == 'I' &&
            magic[2] == 0x2a &&
            magic[3] == 0x00) ||
            (magic[0] == 'M' &&
            magic[1] == 'M' &&
            magic[2] == 0x00 &&
            magic[3] == 0x2a)) {
            is_TIF = 1;
        }
    }
    SDL_RWseek(src, start, RW_SEEK_SET);
    return(is_TIF);
}

SDL_Surface* WIC_LoadImage(SDL_RWops *src)
{
    SDL_Surface* surface = NULL;

    IWICStream* stream = NULL;
    IWICBitmapDecoder* bitmapDecoder = NULL;
    IWICBitmapFrameDecode* bitmapFrame = NULL;
    IWICFormatConverter* formatConverter = NULL;
    
    Sint64 fileSize = SDL_RWsize(src);
    Uint8* memoryBuffer = (Uint8*)SDL_malloc(fileSize);
    if (!memoryBuffer) {
        SDL_OutOfMemory();
        return NULL;  
    }

    SDL_RWread(src, memoryBuffer, 1, fileSize);

    DONE_IF_FAILED(wicFactory->CreateStream(&stream));
    DONE_IF_FAILED(stream->InitializeFromMemory(memoryBuffer, fileSize));
    DONE_IF_FAILED(wicFactory->CreateDecoderFromStream(stream, NULL, WICDecodeMetadataCacheOnDemand, &bitmapDecoder));
    DONE_IF_FAILED(bitmapDecoder->GetFrame(0, &bitmapFrame));
    DONE_IF_FAILED(wicFactory->CreateFormatConverter(&formatConverter));
    DONE_IF_FAILED(formatConverter->Initialize(bitmapFrame, GUID_WICPixelFormat32bppPRGBA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom));
    
    UINT width, height;
    DONE_IF_FAILED(bitmapFrame->GetSize(&width, &height));
    
    Uint32 Amask = 0xFF000000;
    Uint32 Rmask = 0x00FF0000;
    Uint32 Gmask = 0x0000FF00;
    Uint32 Bmask = 0x000000FF;

    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, Rmask, Gmask, Bmask, Amask);

    formatConverter->CopyPixels(NULL, width * 4, width * height * 4, (BYTE*)surface->pixels);

done:
    SAFE_RELEASE(formatConverter);
    SAFE_RELEASE(bitmapFrame);
    SAFE_RELEASE(bitmapDecoder);
    SAFE_RELEASE(stream);

 SDL_free(memoryBuffer);

 return surface;
}

SDL_Surface *IMG_LoadPNG_RW(SDL_RWops *src)
{
    return WIC_LoadImage(src);
}

SDL_Surface *IMG_LoadJPG_RW(SDL_RWops *src)
{
    return WIC_LoadImage(src);
}

SDL_Surface *IMG_LoadTIF_RW(SDL_RWops *src)
{
    return WIC_LoadImage(src);
}

#endif /* SDL_IMAGE_USE_WIC_BACKEND */

