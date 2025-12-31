#include "sdl_context.h"

/* defined as extern in header */
const char* MENU_FORMAT[] = {
  "[L]ength [%d - %d] ",
  "[R]andom [%d - %d] ",
  "[T]hreshold [%0.2f - %0.2f] ",
  "[V]ertical sort [%d]"
};
const SDL_Color WHITE = {255, 255, 255};
const SDL_Color BLACK = {0, 0, 0, 255};
const SDL_Color RED =   {255, 0, 0};
char MENU[MENU_LENGTH][MAX_STRING_LENGTH] = {0};

/**
 * initialize SDL and SDL_Img.
 *
 * @param context to initialize
 * @return exit code
 */
int SDL_InitContext(SDL_Context* context) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "ERROR > failed SDL init.\n");
    return 1;
  }
  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    fprintf(stderr, "ERROR > failed SDL_image init.\n");
    return 1;
  }
  if (TTF_Init() < 0) {
    fprintf(stderr, "ERROR > failed SDL_ttf init.\n");
    return 1;
  }
  if (!(context->default_font = TTF_OpenFont(
    "fonts/BigBlue_TerminalPlus.TTF",
    FONT_SIZE
  ))) {
    fprintf(stderr, "ERROR > couldn't find default font.\n");
    return 1;
  }
  if (!(context->win = SDL_CreateWindow(
    "SDL Tutorial",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    SCREEN_WIDTH, SCREEN_HEIGHT,
    SDL_WINDOW_SHOWN
  ))) {
    fprintf(stderr, "ERROR > creating window\n");
    fprintf(stderr, "SDL_ERROR > %s\n", SDL_GetError());
    return 1;
  }
  if(!(context->renderer = SDL_CreateRenderer(
      context->win,
      -1,
      SDL_RENDERER_ACCELERATED
  ))){
    fprintf(stderr, "ERROR > creating rendererer\n");
    fprintf(stderr, "SDL_ERROR > %s\n", SDL_GetError());
    return 1;
  }
  context->menu_selection = 0;
  return 0;
}

/**
 * load a context surface with path and then copy that to
 * texture for display
 *
 * @param context ptr with renderer and surface ptr
 * @param path to load from fs
 * @return exit code
 */
int SDL_LoadContextScreenTexture(SDL_Context* context) {
  if (!(context->screen_surface = IMG_Load(context->path))) {
    fprintf(stderr, "ERROR > loading image\n");
    fprintf(stderr, "IMG_ERROR > %s\n", IMG_GetError());
    return 1;
  }
  if (!(context->screen_texture =
          SDL_CreateTextureFromSurface(
            context->renderer,
            context->screen_surface
          )
  )) {
    fprintf(stderr, "ERROR > creating texture\n");
    fprintf(stderr, "IMG_ERROR > %s\n", IMG_GetError());
    return 1;
  }
  context->update = 1;
  context->render = 1;
  return 0;
}
/**
 * load text for display
 *
 * @param context ptr with renderer and surface ptr
 * @param path to load from fs
 * @return exit code
 */
int SDL_LoadContextTextTexture(SDL_Context* context, int menu_index, int* offset) {
  if (!(context->text_surfaces[menu_index] =
          TTF_RenderUTF8_Solid(
            context->default_font,
            MENU[menu_index],
            (context->menu_selection == menu_index ? RED : WHITE)
            // RED
          )
       )
  ) {
    fprintf(stderr, "ERROR > creating text.\n");
    fprintf(stderr, "TTF_ERROR > %s\n", TTF_GetError());
    return 1;
  }
  if (!(context->text_textures[menu_index] = SDL_CreateTextureFromSurface(context->renderer, context->text_surfaces[menu_index]))) {
    fprintf(stderr, "ERROR > creating text.\n");
    fprintf(stderr, "TTF_ERROR > %s\n", TTF_GetError());
    return 1;
  }
  context->text_rects[menu_index] = (SDL_Rect){ 0 };
  TTF_SizeText(
    context->default_font,
    MENU[menu_index],
    &(context->text_rects[menu_index].x),
    &(context->text_rects[menu_index].h)
  );
  context->text_rects[menu_index].y = SCREEN_HEIGHT - context->text_rects[menu_index].h;
  context->text_rects[menu_index].x = *offset;
  *offset += context->text_rects[menu_index].w;
  context->render = 1;
  return 0;
}

/**
 * formats data from sort info to load as text
 */
int SDL_LoadContextMenu(SDL_Context* context, sort_info* si) {
  memset(MENU, 0, sizeof(MENU));
  int offset = 0; /* keep track of each items offset */
  snprintf(MENU[0], sizeof(MENU[0]), MENU_FORMAT[0], si->min_length, si->max_length);
  snprintf(MENU[1], sizeof(MENU[1]), MENU_FORMAT[1], si->random_start, si->random_stop);
  snprintf(MENU[2], sizeof(MENU[2]), MENU_FORMAT[2], si->start_threshold, si->stop_threshold);
  snprintf(MENU[3], sizeof(MENU[3]), MENU_FORMAT[3], si->vertical_sort);
  for (int i = 0; i < MENU_LENGTH; i++)
    SDL_LoadContextTextTexture(context, i, &offset);
}

int SDL_LoadContext(SDL_Context* context, sort_info* si) {
  if (SDL_LoadContextScreenTexture(context))
    return 1;
  if (SDL_LoadContextMenu(context, si))
    return 1;
}

int SDL_InitFrom(SDL_Context* context, sort_info* si) {
  context->path = si->img_ref->path;
  if (SDL_InitContext(context))
    return 1;
  if (SDL_LoadContext(context, si))
    return 1;
  return 0;
}

/**
 * update a contexts texture from its surface
 *
 * @param context with texture and surface components
 * @return exit code
 */
int SDL_UpdateContextScreenTexture(SDL_Context* context) {
  if (!context->screen_texture) {
    fprintf(stderr, "ERROR > updating nonexistent texture\n");
    return 1;
  }
  SDL_DestroyTexture(context->screen_texture);
  if (!(context->screen_texture =
          SDL_CreateTextureFromSurface(
            context->renderer,
            context->screen_surface
          )
  )) {
    fprintf(stderr, "ERROR > updating texture.\n");
    fprintf(stderr, "IMG_ERROR > %s\n", IMG_GetError());
    return 1;
  }
  return 0;
}

/**
 * update a contexts text from its surface
 *
 * @param context with text texture and surface components
 * @return exit code
 */
int SDL_UpdateContextTextTexture(SDL_Context* context, int menu_index, int* offset) {
  /* verify inputs */
  if (!context || !context->text_textures[menu_index]) {
    fprintf(stderr, "ERROR > updating null.\n");
    return 1;
  }

  /* rewrite text to surface */
  if (!(context->text_surfaces[menu_index] =
          TTF_RenderUTF8_Solid(
            context->default_font,
            MENU[menu_index],
            // RED
            (context->menu_selection == menu_index ? RED : WHITE)
          )
       )
  ) {
    fprintf(stderr, "ERROR > creating text.\n");
    fprintf(stderr, "TTF_ERROR > %s\n", TTF_GetError());
    return 1;
  }
  /* dont leak memory! probably a better way to do this (SDL_UpdateTexture)*/
  SDL_DestroyTexture(context->text_textures[menu_index]);
  if (!(context->text_textures[menu_index] = SDL_CreateTextureFromSurface(context->renderer, context->text_surfaces[menu_index]))) {
    fprintf(stderr, "ERROR > creating text.\n");
    fprintf(stderr, "TTF_ERROR > %s\n", TTF_GetError());
    return 1;
  }

  // clear rect and reset size
  context->text_rects[menu_index] = (SDL_Rect){ 0 };
  TTF_SizeText(
    context->default_font,
    MENU[menu_index],
    &(context->text_rects[menu_index].w),
    &(context->text_rects[menu_index].h)
  );
  context->text_rects[menu_index].x = *offset;
  *offset += context->text_rects[menu_index].w;
  // SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 255);
  // SDL_RenderFillRect(context->renderer, &context->text_rect);
  context->render = 1;
  return 0;

}

int SDL_UpdateContextMenu(SDL_Context* context, sort_info* si) {
  memset(MENU, 0, sizeof(MENU));
  int offset = 0; /* keep track of each items offset */
  snprintf(MENU[0], sizeof(MENU[0]), MENU_FORMAT[0], si->min_length, si->max_length);
  snprintf(MENU[1], sizeof(MENU[1]), MENU_FORMAT[1], si->random_start, si->random_stop);
  snprintf(MENU[2], sizeof(MENU[2]), MENU_FORMAT[2], si->start_threshold, si->stop_threshold);
  snprintf(MENU[3], sizeof(MENU[3]), MENU_FORMAT[3], si->vertical_sort);
  for (int i = 0; i < MENU_LENGTH; i++)
    SDL_UpdateContextTextTexture(context, i, &offset);
}

/**
 * update a everything in a context
 * screen and menu textures
 *
 * @param context with texture and surface components
 * @return exit code
 */
int SDL_UpdateContext(SDL_Context* context, sort_info* si) {
  /* skip if flag not set */
  if (!context->update) {
    return 0;
  }
  SDL_UpdateContextScreenTexture(context);
  SDL_UpdateContextMenu(context, si);
  context->update = 0;
  context->render = 1;
  return 0;
}

/**
 * handles inputs to adjust sort info
 * stores SDL event and and keyboard state in context
 */
int SDL_HandleContextInput(SDL_Context* context, sort_info* si) {
  /* verify inputs */
  if (!context || !si) {
    fprintf(stderr, "ERROR > null context in handle input.\n");
    return 1;
  }
  /* check keyboard inputs */
  context->kbstate = SDL_GetKeyboardState(NULL);

  if (context->kbstate[SDL_SCANCODE_J]) {
    switch(context->menu_selection) {
      case 0:
          si->min_length++;
      break;
      case 1:
          si->random_stop++;
      break;
      case 2:
        si->start_threshold += 0.001;
      break;
      case 3:
        si->vertical_sort = si->vertical_sort ? 0 : 1;
      break;
    }
    context->update = 1;
  }
  if (context->kbstate[SDL_SCANCODE_K]) {
    switch(context->menu_selection) {
      case 0:
          si->min_length--;
      break;
      case 1:
          si->random_stop--;
      break;
      case 2:
        si->start_threshold -= 0.001;
      break;
      case 3:
        si->vertical_sort = si->vertical_sort ? 0 : 1;
      break;
    }
    context->update = 1;
  }
  if (context->kbstate[SDL_SCANCODE_Q]) {
    context->running = 0;
  }

  /* handle queued events */
  while (SDL_PollEvent(&(context->event))) {
    switch (context->event.type) {
      case SDL_QUIT:
        context->running = 0;
        break;
      case SDL_KEYDOWN:
        switch (context->event.key.keysym.sym) {
          case SDLK_l:
            context->menu_selection += (context->menu_selection < MENU_LENGTH - 1 ? 1 : 0);
            context->update = 1;
          break;
          case SDLK_h:
            context->menu_selection -= (context->menu_selection > 0 ? 1 : 0);
            context->update = 1;
          break;
          case SDLK_v:
            si->vertical_sort = si->vertical_sort ? 0 : 1;
            context->update = 1;
          break;
        }
    }
  }
  return 0;
}

/**
 * clear and refresh contexts renderer
 */
int SDL_RenderContext(SDL_Context* context) {
  /* skip if flag not set */
  if (!context->render)
    return 0;
  SDL_RenderClear(context->renderer);
  SDL_RenderCopy(context->renderer, context->screen_texture, NULL, NULL);
  /* add black under text */

  for (int i = 0; i < MENU_LENGTH; i++) {
    SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(context->renderer, &context->text_rects[i]);
    SDL_RenderCopy(context->renderer, context->text_textures[i], NULL, &(context->text_rects[i]));
  }
  SDL_RenderPresent(context->renderer);
  context->render = 0;
  context->running = 1;
  return 0;
}

/**
 * free resources and quit
 */
int SDL_QuitContext(SDL_Context* context) {
  /* free screen stuff  */
  if (context->screen_surface) {
    SDL_FreeSurface(context->screen_surface);
  } else {
    fprintf(stderr, "ERROR > freeing surface\n");
    return 1;
  }
  if (context->screen_texture) {
    SDL_DestroyTexture(context->screen_texture);
  } else {
    fprintf(stderr, "ERROR > destroying texture\n");
    return 1;
  }
  /* free text stuff  */
  for (int i = 0; i < MENU_LENGTH; i++) {
    if (context->text_surfaces[i]) {
      SDL_FreeSurface(context->text_surfaces[i]);
    } else {
      fprintf(stderr, "ERROR > freeing surface\n");
      return 1;
    }
    if (context->text_textures[i]) {
      SDL_DestroyTexture(context->text_textures[i]);
    } else {
      fprintf(stderr, "ERROR > destroying texture\n");
      return 1;
    }
  }
  IMG_Quit();
  SDL_Quit();
}
