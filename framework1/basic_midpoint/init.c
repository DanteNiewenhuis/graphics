/* Computer Graphics, Assignment 1, Bresenham's Midpoint Line-Algorithm
 *
 * Filename ........ init.c
 * Description ..... SDL Texture initialization, Pixel operations
 * Created by ...... Jurgen Sturm
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL.h"
#include "init.h"
#include "mla.h"

void PutPixel(SDL_Texture *texture, int x, int y, Uint32 pixel);

void ReportPixelFormat(SDL_Texture *t) {
  Uint32 f;
  int w, h;
  if(SDL_QueryTexture(t, &f, NULL, &w, &h) < 0) {
      printf("Error while querying SDL texture: %s\n", SDL_GetError());
      return;
  }

  printf("screen(%d,%d) %d bits/%d bytes per pixel\n", w, h,
         SDL_BITSPERPIXEL(f), SDL_BYTESPERPIXEL(f));

  return;
}

void PutPixel(SDL_Texture *texture, int x, int y, Uint32 pixel) {
  Uint32 f;
  int w, bpp;
  if(SDL_QueryTexture(texture, &f, NULL, &w, NULL) < 0) {
      printf("Error while querying SDL texture: %s\n", SDL_GetError());
      exit(-1);
  }
  bpp = SDL_BYTESPERPIXEL(f);
  /* Here p is the address to the pixel we want to set */
  Uint8 *p = (Uint8 *)pixels + y * bpp * w + x * bpp;

  switch(bpp) {
  case 1:
    *p = pixel;
    break;

  case 2:
    *(Uint16 *)p = pixel;
    break;

  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    } else {
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    }
    break;

  case 4:
    *(Uint32 *)p = pixel;
    break;
  }
}


void DrawFigure(SDL_Texture *texture) {
  Uint32 f;
  int w, h;
  if(SDL_QueryTexture(texture, &f, NULL, &w, &h) < 0) {
    printf("Error while querying SDL texture: %s\n", SDL_GetError());
    exit(-1);
  }
  int i, mid_x = w/2, mid_y = h/2, size = 100;
  Uint32 colour;
  int pitch;

  if(SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch) < 0) {
    printf("Unable to lock texture: %s\n", SDL_GetError());
    exit(-1);
  }

  for(i=0; i<32; i++) {
    /* Set drawing colour */
    switch((i>>2) % 4) {
      case 0:
        colour=0xFFFF0000;// SDL_MapRGB(f,0xFF,0,0);
        break;
      case 1:
        colour=0xFF00FF00;
        break;
      case 2:
        colour=0xFF0000FF;
        break;
      case 3:
        colour=0xFFFFFFFF;
        break;
    }

    /* draw a line */
    mla(texture, mid_x, mid_y,
        (int)roundf(size*cos(2*M_PI/32*i))+mid_x,
        (int)roundf(size*sin(2*M_PI/32*i))+mid_y, colour);
  }
  /* It would be more efficient to call SDL_UpdateRect(), but I do not really
   * care.
   */
  SDL_UpdateTexture(texture, NULL, pixels, pitch);

  SDL_RenderClear(rend);
  SDL_RenderCopy(rend, texture, NULL, NULL);
  SDL_RenderPresent(rend);

  SDL_UnlockTexture(texture);

  return;
}


SDL_Texture* InitialiseScreen(int w, int h) {
  SDL_Window *win;
  SDL_Texture *screen;
  if(SDL_Init(SDL_INIT_VIDEO) == -1) {
    printf("Unable to initialise SDL video subsystem: %s", SDL_GetError());
    exit(-1);
  }

  atexit(SDL_Quit);

  SDL_CreateWindowAndRenderer(w, h, 0, &win, &rend);

  if(win == NULL) {
    printf("Unable to create SDL window: `%s', ",
           SDL_GetError());
    exit(-1);
  }
  if(rend == NULL) {
    printf("Unable to create SDL renderer: `%s', ",
           SDL_GetError());
    exit(-1);
  }

  screen = SDL_CreateTexture(rend,
                             SDL_PIXELFORMAT_ARGB8888,
                             SDL_TEXTUREACCESS_STREAMING,
                             w, h);

  if(screen == NULL) {
    printf("Unable to create SDL texture: `%s', ",
           SDL_GetError());
    exit(-1);
  }

  pixels = malloc(w * h * sizeof(Uint32));

  return(screen);
}


void WaitForEvent(void) {
  int quit=0;

  printf("Press anykey to quit\n");

  while(! quit) {
    SDL_Event event;
    int res=SDL_WaitEvent(&event);
    if(res == 0) {
      printf("Waiting for an event failed: %s\n", SDL_GetError());
      free(pixels);
      exit(-1);
    }

    switch(event.type) {
    case SDL_KEYDOWN:
      quit=1;
      break;
    case SDL_QUIT:
      quit=1;
      break;
    }
  }

  free(pixels);
  return;
}
