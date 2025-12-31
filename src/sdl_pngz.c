/* SDL PNGZ interop *
 *
 * SDL surfaces store pixel data in a void ptr.
 * the void ptr contains pixel data as raw bytes.
 * you can use pitch and bytes per pixel to convert
 * pixel coordinates into an offset from the ptr.
 * the raw bytes are then put into a Uint32 that
 * can be decoded into rgba for editing
 *
 * key functions:
 *
 *   - pixel SDL_get_pngz_pixel(SDL_Surface* surface, int x, int y);
 *   - void SDL_set_pngz_pixel(SDL_Surface* surface, int x, int y, pixel p);
 *   - void pngz_to_surface(pngz* z, SDL_Surface* surface);
 *   - void surface_to_pngz(SDL_Surface* surface, pngz* z);
 *
 */

#include "sdl_pngz.h"
#include <stdio.h>

/**
 * gets raw pixel data from a surface at a given x and y coordinate.
 * uses pitch and bytes per pixel to index into a surfaces pixel buffer.
 *
 * @param surface containing a pixel data buffer we want to access
 * @param x pixel coordinate (column)
 * @param y pixel coordinate (row)
 * @return raw pixel data as unsigned int
 */
Uint32 SDL_get_pixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    if (bpp > 4 || bpp < 1) {
      fprintf(stderr, "ERROR > reading bytes per pixel\n");
      return 0;
    }
    switch (bpp) {
      case 1:
        return *p;
      case 2:
        return *(Uint16 *)p;
      case 3: /* <- pngz */
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
          return p[0] << 16 | p[1] << 8 | p[2];
        else
          return p[0] | p[1] << 8 | p[2] << 16;
      case 4:
        return *(Uint32 *)p;
    }
}

/**
 * sets raw pixel data of a surface at a given x and y coordinate.
 * uses pitch and bytes per pixel to index into a surfaces pixel buffer.
 *
 * @param surface containing a pixel data buffer we want to set
 * @param x pixel coordinate (column)
 * @param y pixel coordinate (row)
 * @param pixel_data unsigned integer of raw pixel bits
 * @return exit code
 */
int SDL_set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel_data) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    if (bpp > 4 || bpp < 1) {
      fprintf(stderr, "ERROR > reading bytes per pixel\n");
      return 1;
    }
    switch (bpp) {
      case 1:
        *p = pixel_data;
        break;
      case 2:
        *(Uint16 *)p = pixel_data;
        break;
      case 3: /* <- pngz */
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
          p[0] = (pixel_data >> 24) & 0xFF;
          p[1] = (pixel_data >> 16) & 0xFF;
          p[2] = (pixel_data >> 8) & 0xFF;
        } else {
          p[0] = pixel_data & 0xFF;
          p[1] = (pixel_data >> 8) & 0xFF;
          p[2] = (pixel_data >> 16) & 0xFF;
        }
        break;
      case 4:
        *(Uint32 *)p = pixel_data;
        break;
    }
    return 0;
}

/**
 * extract raw pixel data (Uint32) into rgba with SDL_GetRGBA function
 *
 * @param surface containing a pixel data buffer and format of how its stored
 * @param x pixel coordinate (column)
 * @param y pixel coordinate (row)
 * @param r ptr to red channel   (0-255)
 * @param g ptr to green channel (0-255)
 * @param b ptr to blue channel  (0-255)
 * @param a ptr to alpha channel (0-255)
 * @return exit code, result through reference
 */
int SDL_get_pixel_rgba(SDL_Surface *surface, int x, int y, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a) {
  Uint32 pixel_data = SDL_get_pixel(surface, x, y);
  SDL_GetRGBA(pixel_data, surface->format, r, g, b, a);
  return 0;
}

/**
 * convert raw pixel data (Uint32) into rgba with SDL_MapRGBA function
 *
 * @param surface containing a pixel data buffer and format of how its stored
 * @param x pixel coordinate (column)
 * @param y pixel coordinate (row)
 * @param r ptr to red channel   (0-255)
 * @param g ptr to green channel (0-255)
 * @param b ptr to blue channel  (0-255)
 * @param a ptr to alpha channel (0-255)
 * @return exit code, result through reference
 */
int SDL_set_pixel_rgba(SDL_Surface *surface, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  Uint32 pixel_data = SDL_MapRGBA(surface->format, r, g, b, a);
  return SDL_set_pixel(surface, x, y, pixel_data);
}

/**
 * wraper for pixel rgb extraction with a pngz pixel
 *
 * @param surface containing a pixel data buffer and format of how its stored
 * @param x pixel coordinate (column)
 * @param y pixel coordinate (row)
 * @return pixel containing SDL surface data
 */
pixel SDL_get_pngz_pixel(SDL_Surface *surface, int x, int y) {
  pixel p;
  SDL_get_pixel_rgba(surface, x, y, &p.r, &p.g, &p.b, &p.a);
  return p;
}

/**
 * wraper for pixel rgb setting with a pngz pixel
 *
 * @param surface containing a pixel data buffer and format of how its stored
 * @param x pixel coordinate (column)
 * @param y pixel coordinate (row)
 * @return exit code, result by reference in surface ptr
 */
int SDL_set_pngz_pixel(SDL_Surface *surface, int x, int y, pixel p) {
  return SDL_set_pixel_rgba(surface, x, y, p.r, p.g, p.b, p.a);
}


/**
 * pack all pixels from a surface and pngz.
 * must match size
 *
 * @param surface
 * @param pngz
 * @return exit code
 */
int SDL_surface_to_pngz(SDL_Surface *surface, pngz *z) {
  if (z->height != surface->h || z->width != surface->w) {
    fprintf(stderr, "ERROR > surface to pngz\n");
    return 1;
  }
  for (int i = 0; i < z->height; i++) {
    for (int j = 0; j < z->width; j++) {
      z->pixels[i][j] = SDL_get_pngz_pixel(surface, j, i);
    }
  }
  return 0;
}

/**
 * pack all pixels from a pngz to a surface
 * must match size
 *
 * @param surface
 * @param pngz
 * @return exit code
 */
int SDL_pngz_to_surface(pngz z, SDL_Surface *surface) {
  if (z.height != surface->h || z.width != surface->w) {
    fprintf(stderr, "ERROR > pngz to surface\n");
    return 1;
  }
  for (int i = 0; i < z.height; i++) {
    for (int j = 0; j < z.width; j++) {
      if (SDL_set_pngz_pixel(surface, j, i, z.pixels[i][j])) {
        return 1;
      }
    }
  }
  return 0;
}
