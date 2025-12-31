#ifndef _SDL_CONTEXT_H_
#define _SDL_CONTEXT_H_

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "sort.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 800
#define FONT_SIZE 20
#define MAX_STRING_LENGTH 200
#define MENU_LENGTH 4
extern const char* MENU_FORMAT[MENU_LENGTH];
extern char MENU[MENU_LENGTH][MAX_STRING_LENGTH];
extern const SDL_Color WHITE;
extern const SDL_Color BLACK;
extern const SDL_Color RED;
/**
 * context contains references to all necessary SDL components
 */
typedef struct SDL_Context {

  /* core */
  SDL_Window* win;             /** open window reference */
  SDL_Renderer* renderer;      /** renderer for window */
  SDL_Surface* screen_surface; /** cpu pixel data */
  SDL_Texture* screen_texture; /** gpu pixel data */

  /* font */
  TTF_Font* default_font;                  /** default font is BigBlueTerm */
  SDL_Surface* text_surfaces[MENU_LENGTH]; /** surface for text rendering (we just need one) */
  SDL_Texture* text_textures[MENU_LENGTH]; /** text textures */
  SDL_Rect text_rects[MENU_LENGTH];        /** where to display text */

  /* menu */
  SDL_Event event;             /** event*/
  const Uint8* kbstate;        /** keyboard */
  const char* path;            /** file path */
  const char* menu;            /** menu string */
  int menu_selection;          /** menu index selected */

  /* flags */
  int running;                 /** running flag */
  int update;                  /** set once text is modified and off when updated */
  int render;                  /** set once context is modified and off when rendered */
} SDL_Context;

int SDL_InitContext(SDL_Context* context);

int SDL_LoadContextScreenTexture(SDL_Context* context);
int SDL_LoadContextTextTexture(SDL_Context* context, int menu_i, int* offset);
int SDL_LoadContextMenu(SDL_Context* context, sort_info* si);
int SDL_LoadContext(SDL_Context* context, sort_info* si);

int SDL_InitFrom(SDL_Context* context, sort_info* si);

int SDL_UpdateContextScreenTexture(SDL_Context* context);
int SDL_UpdateContextTextTexture(SDL_Context* context, int menu_i, int* offset);
int SDL_UpdateContextMenu(SDL_Context* context, sort_info* si);
int SDL_UpdateContext(SDL_Context* context, sort_info* si);
int SDL_HandleContextInput(SDL_Context* context, sort_info* si);

int SDL_RenderContext(SDL_Context* context);
int SDL_QuitContext(SDL_Context* context);

#endif /* !_SDL_CONTEXT_H_ */
