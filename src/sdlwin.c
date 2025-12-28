/*===========================*
 *                           *
 *    main driver program    *
 *                           *
 *======================================*
 * samantha jane                        *
 * driver program for window renderering. *
 *==========================================================================80*/

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pngz.h>
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

// handle byte order and pixel formatting
Uint32 get_pixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch (bpp) {
      case 1:
        return *p;
      case 2:
        return *(Uint16 *)p;
      case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
          return p[0] << 16 | p[1] << 8 | p[2];
        else
          return p[0] | p[1] << 8 | p[2] << 16;
      case 4:
        return *(Uint32 *)p;
      default:
        return 0; /*err*/
    }
}

void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel_data) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
      case 1:
        *p = pixel_data;
        return;
      case 2:
        *(Uint16 *)p = pixel_data;
        return;
      case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
          p[0] = (pixel_data >> 24) & 0xFF;
          p[1] = (pixel_data >> 16) & 0xFF;
          p[2] = (pixel_data >> 8) & 0xFF;
        } else {
          p[0] = pixel_data & 0xFF;
          p[1] = (pixel_data >> 8) & 0xFF;
          p[2] = (pixel_data >> 16) & 0xFF;
        }
        return;
      case 4:
        *(Uint32 *)p = pixel_data;
        return;
      default:
        return; /* error */
    }
}

void get_pixel_rgba(SDL_Surface *surface, int x, int y, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a) {
  Uint32 pixel_data = get_pixel(surface, x, y);
  SDL_GetRGBA(pixel_data, surface->format, r, g, b, a);
}

void set_pixel_rgba(SDL_Surface *surface, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  Uint32 pixel_data = SDL_MapRGBA(surface->format, r, g, b, a);
  set_pixel(surface, x, y, pixel_data);
}

pixel get_pngz_pixel(SDL_Surface *surface, int x, int y) {
  pixel p;
  get_pixel_rgba(surface, x, y, &p.r, &p.g, &p.b, &p.a);
  return p;
}

void set_pngz_pixel(SDL_Surface *surface, int x, int y, pixel p) {
  set_pixel_rgba(surface, x, y, p.r, p.g, p.b, p.a);
}

void surface_to_pngz(SDL_Surface *surface, pngz *z) {
  // pack into a pngz
  for (int i = 0; i < z->height; i++) {
    for (int j = 0; j < z->width; j++) {
      z->pixels[i][j] = get_pngz_pixel(surface, j, i);
    }
  }
}

void pngz_to_surface(pngz z, SDL_Surface *surface) {
  // now lets write it back to a surface
  for (int i = 0; i < z.height; i++) {
    for (int j = 0; j < z.width; j++) {
      // z.pixels[i][j] = get_pngz_pixel(surface, j, i);
      set_pngz_pixel(surface, j, i, z.pixels[i][j]);
    }
  }
}

typedef struct SDL_Context {
  SDL_Window* win;
  SDL_Renderer* renderer;
  SDL_Surface* surface;
  SDL_Texture* texture;
  char* path;
} SDL_Context;

int SDL_InitContext(SDL_Context* context) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("error in sdl initalization\n");
    return 1;
  }
  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    printf("error in img initalization\n");
    return 1;
  }
  if (!(context->win = SDL_CreateWindow(
    "SDL Tutorial",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    SCREEN_WIDTH, SCREEN_HEIGHT,
    SDL_WINDOW_SHOWN
  ))) {
    printf("error creating window -> SDL_error: %s\n", SDL_GetError());
    return 1;
  }
  if(!(context->renderer = SDL_CreateRenderer(
      context->win,
      -1,
      SDL_RENDERER_ACCELERATED
  ))){
    printf("error creating rendererer -> SDL_error: %s\n", SDL_GetError());
    return 1;
  }
  return 0;
}

int SDL_LoadContextTexture(SDL_Context* context, char* path) {
  context->path = path;
  if (!(context->surface = IMG_Load(context->path))) {
    printf("error loading img -> IMG_error: %s\n", IMG_GetError());
    return 1;
  }
  if (!(context->texture = SDL_CreateTextureFromSurface(context->renderer, context->surface))) {
    printf("error loading img -> IMG_error: %s\n", IMG_GetError());
    return 1;
  }
  return 0;
}

int SDL_UpdateContextTexture(SDL_Context* context) {
  if (!(context->texture = SDL_CreateTextureFromSurface(context->renderer, context->surface))) {
    printf("error loading img -> IMG_error: %s\n", IMG_GetError());
    return 1;
  }
  return 0;
}

void SDL_RenderContext(SDL_Context* context) {
  SDL_RenderClear(context->renderer);
  SDL_RenderCopy(context->renderer, context->texture, NULL, NULL);
  SDL_RenderPresent(context->renderer);
  SDL_Delay(1000);
}

void SDL_QuitContext(SDL_Context* context) {
  IMG_Quit();
  SDL_Quit();
}

int main(int argc, char* args[]) {

  char* path = "png/goodbye.png";
  pngz z;
  pngz_load_from(&z, path);

  SDL_Context context;
  SDL_InitContext(&context);
  SDL_LoadContextTexture(&context, path);
  pngz_to_surface(z, context.surface);

  surface_to_pngz(context.surface, &z);
  pngz_save_as(z, "png/goodbyetest.png");
  pngz_load_from(&z,"png/goodbyetest.png");

  SDL_RenderContext(&context);
  SDL_QuitContext(&context);
  exit(0);
}
