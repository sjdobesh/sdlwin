#ifndef _SDL_PNGZ_H_
#define _SDL_PNGZ_H_

#include <SDL2/SDL.h>
#include <pngz.h>

Uint32 SDL_get_pixel(SDL_Surface *surface, int x, int y);
int SDL_set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel_data);
int SDL_get_pixel_rgba(SDL_Surface *surface, int x, int y, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a);
int SDL_set_pixel_rgba(SDL_Surface *surface, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
pixel SDL_get_pngz_pixel(SDL_Surface *surface, int x, int y);
int SDL_set_pngz_pixel(SDL_Surface *surface, int x, int y, pixel p);
int SDL_surface_to_pngz(SDL_Surface *surface, pngz *z);
int SDL_pngz_to_surface(pngz z, SDL_Surface *surface);

#endif /* !_SDLPNGZ_H_ */
