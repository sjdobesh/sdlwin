/*===========================*
 *                           *
 *    main driver program    *
 *                           *
 *======================================*
 * samantha jane                        *
 * driver program for window rendering. *
 *==========================================================================80*/

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char* args[]) {

  printf("beginning SDL init\n");
  SDL_Window* win = NULL;
  SDL_Surface* surface = NULL;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("error in sdl initalization\n");
    return 1;
  }
  printf("finished SDL init\n");

  int initflags;
  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    printf("error in img initalization\n");
    return 1;
  }



  win = SDL_CreateWindow(
    "SDL Tutorial",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    SCREEN_WIDTH, SCREEN_HEIGHT,
    SDL_WINDOW_SHOWN
  );

  if(!win) {
    printf("error creating window -> SDL_error: %s\n", SDL_GetError());
  }
  printf("SDL window created\n");
  SDL_Renderer* render = NULL;
  render = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  if(!render) {
    printf("error creating renderer -> SDL_error: %s\n", SDL_GetError());
  }
  // load in a texture from memory
  SDL_Texture* texture = NULL;
  SDL_RWops* rw = NULL;

  //// pngz stuff here

  //if (!(rw = SDLRWFromConstMem(/**/)) {
  //  printf("error creating SDLRW -> SDL_error: %s\n", SDL_GetError());
  //}
  //if (!(texture = IMG_LoadTextureTyped_RW(render, rw, 1,"PNG"))) {
  //  printf("error loading texture -> SDL_error: %s\n", IMG_GetError());
  //}
  //SDL_Rect texture_rect;
  //texture_rect.x = 0; //the x coordinate
  //texture_rect.y = 0; //the y coordinate
  //texture_rect.w = 50; //the width of the texture
  //texture_rect.h = 50; //the height of the texture
  SDL_RenderClear(render);
  //SDL_RenderCopy(render, texture, NULL, &texture_rect);
  SDL_SetRenderDrawColor(render, 96, 128, 255, 255);
  SDL_RenderPresent(render);
  // wait
  SDL_Delay(1000);
  IMG_Quit();
  SDL_Quit();
  exit(0);
}
