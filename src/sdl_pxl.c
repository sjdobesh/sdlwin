/*===========================*
 *                           *
 *    main driver program    *
 *                           *
 *============================================*
 * samantha jane                              *
 * driver program for sdl window renderering. *
 *==========================================================================80*/

#include "sdl_pngz.h"
#include "sdl_context.h"

int main(int argc, char* args[]) {

  char* path = "png/sheridan_cute.png";

  /* load pngz */
  pngz z_sorted, z_original;
  pngz_load_from(&z_sorted, path);
  pngz_load_from(&z_original, path);

  /* sort data */
  sort_info si = {
    .img_ref = &z_sorted,
    .vertical_sort = 0,
    .start_threshold = 0,
    .stop_threshold = 0,
    .min_length = 1,
    .max_length = 1,
    .random_start = 0,
    .random_stop = 0,
    .comparison_function = compare_brightness,
    .threshold_function = threshold_brightness
  };

  /* load sdl */
  SDL_Context context = {0};
  SDL_InitFrom(&context, &si);
  SDL_RenderContext(&context);

  /* main loop */
  printf("entering main loop\n");
  while (context.running) {
    /* handle input */
    SDL_HandleContextInput(&context, &si);
    if (context.update) {
      /* refresh and resort */
      pngz_copy(z_original, &z_sorted);
      fuck_sort(si);
      SDL_pngz_to_surface(z_sorted, context.screen_surface);
    }
    SDL_UpdateContext(&context, &si);
    SDL_RenderContext(&context);
  }

  /* save and exit */
  SDL_surface_to_pngz(context.screen_surface, &z_sorted);
  pngz_save_as(z_sorted, "png/goodbyetest.png");
  SDL_QuitContext(&context);
  return 0;
}
