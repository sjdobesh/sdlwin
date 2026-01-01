#include "sdl_context.h"

/* CONSTANTS/GLOBALS */
/* defined as extern in sdl_context.h */
const char* MENU_FORMAT[] = {
  "min length [%d] ",
  "max length [%d] ",
  "random start [%d] ",
  "random stop  [%d] ",
  "start threshold [%0.3f] ",
  "stop threshold  [%0.3f] ",
  "vertical [%d]"
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
  /* init libraries */
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
  /* find font */
  if (!(context->default_font =
          TTF_OpenFont(
            "fonts/BigBlue_TerminalPlus.TTF",
            FONT_SIZE
          )
  )) {
    fprintf(stderr, "ERROR > couldn't find default font.\n");
    return 1;
  }
  /* create window and renderer */
  if (!(context->win =
          SDL_CreateWindow(
            "pxl",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN
          )
  )) {
    fprintf(stderr, "ERROR > creating window.\n%s\n", SDL_GetError());
    return 1;
  }
  if(!(context->renderer =
         SDL_CreateRenderer(
           context->win,
           -1,
           SDL_RENDERER_ACCELERATED
         )
  )){
    fprintf(stderr, "ERROR > creating renderer.\n%s\n", SDL_GetError());
    return 1;
  }
  /* init a bunch of constants just in case */
  context->menu_selection = context->update = context->render = 0;
  return 0;
}

/* creates texture from surface with streaming access */
int SDL_LoadTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surface, SDL_Texture** texture) {
  /* create texture object */
  if (!(*texture =
           SDL_CreateTexture(
             renderer,
             surface->format->format,
             SDL_TEXTUREACCESS_STREAMING,
             surface->w,
             surface->h
           )
  )) {
    fprintf(stderr,"ERROR > creating texture.\n%s\n",SDL_GetError());
    return 1;
  }
  return 0;
}

/**
 * load a context surface from a path and then copy
 * that to screen texture for display.
 * SDL_Context wrapper for generic LoadTextureFromSurface
 *
 * @param context ptr with renderer and surface ptr
 * @param path to load from fs
 * @return exit code
 */
int SDL_LoadContextScreenTexture(SDL_Context* context) {
  if (!(context->screen_surface = IMG_Load(context->path))) {
    fprintf(
      stderr, "ERROR > loading %s to screen surface .\n%s\n",
      context->path, IMG_GetError()
    );
    return 1;
  }
  return SDL_LoadTextureFromSurface(
           context->renderer,
           context->screen_surface,
           &(context->screen_texture)
         );
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
          )
       )
  ) {
    fprintf(stderr, "ERROR > creating text surface.\n%s\n", TTF_GetError());
    return 1;
  }
  if (!(context->text_textures[menu_index] =
          SDL_CreateTextureFromSurface(
            context->renderer,
            context->text_surfaces[menu_index]
          )
  )) {
    fprintf(stderr, "ERROR > creating text texture.\n%s\n", SDL_GetError());
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
  return 0;
}

int SDL_ReadContextMenu(sort_info* si) {
  memset(MENU, 0, sizeof(MENU));
  snprintf(MENU[0], sizeof(MENU[0]), MENU_FORMAT[0], si->min_length);
  snprintf(MENU[1], sizeof(MENU[1]), MENU_FORMAT[1], si->max_length);
  snprintf(MENU[2], sizeof(MENU[2]), MENU_FORMAT[2], si->random_start);
  snprintf(MENU[3], sizeof(MENU[3]), MENU_FORMAT[3], si->random_stop);
  snprintf(MENU[4], sizeof(MENU[4]), MENU_FORMAT[4], si->start_threshold);
  snprintf(MENU[5], sizeof(MENU[5]), MENU_FORMAT[5], si->stop_threshold);
  snprintf(MENU[6], sizeof(MENU[6]), MENU_FORMAT[6], si->vertical_sort);
}

/**
 * formats data from sort info to load as text
 */
int SDL_LoadContextMenu(SDL_Context* context, sort_info* si) {
  SDL_ReadContextMenu(si);
  int offset = 0; /* keep track of each items offset */
  for (int i = 0; i < MENU_LENGTH; i++)
    if (!SDL_LoadContextTextTexture(context, i, &offset))
      return 1;
  return 0;
}

int SDL_LoadContext(SDL_Context* context, sort_info* si) {
  /* for each, load texture, set render, and unset update flag*/
  if (SDL_LoadContextScreenTexture(context)) return 1;
  else {
    context->render = 1;
    context->update &= ~SCREEN_UPDATE;
  }
  if (SDL_LoadContextMenu(context, si)) return 1;
  else {
    context->render = 1;
    context->update &= ~MENU_UPDATE;
  }
  return 0;
}

int SDL_InitFrom(SDL_Context* context, sort_info* si) {
  context->path = si->img_ref->path;
  if (SDL_InitContext(context))
    return 1;
  if (SDL_LoadContext(context, si))
    return 1;
  return 0;
}

int SDL_UpdateTextureFromSurface(SDL_Surface* surface, SDL_Texture** texture) {
  /* update pixel values */
  if (SDL_UpdateTexture(
        *texture,
        NULL,
        surface->pixels,
        surface->pitch
     ) < 0
  ) {
    fprintf(stderr, "ERROR > updating texture.\n%s\n", SDL_GetError());
    return 1;
  }

  return 0;
}
/**
 * update a contexts texture from its surface
 *
 * @param context with texture and surface components
 * @return exit code
 */
int SDL_UpdateContextScreenTexture(SDL_Context* context) {
  return SDL_UpdateTextureFromSurface(
           context->screen_surface,
           &(context->screen_texture)
         );
}

/**
 * update a contexts text from its surface
 *
 * @param context with text texture and surface components
 * @return exit code
 */
int SDL_UpdateContextTextTexture(SDL_Context* context, int menu_index, int* offset) {
  /* free and generate text to surface */
  SDL_FreeSurface(context->text_surfaces[menu_index]);
  if (!(context->text_surfaces[menu_index] =
          TTF_RenderUTF8_Solid(
            context->default_font,
            MENU[menu_index],
            (context->menu_selection == menu_index ? RED : WHITE)
          )
       )
  ) {
    fprintf(stderr, "ERROR > creating text surface.\n%s\n", TTF_GetError());
    return 1;
  }
  /* dont leak memory! replace w custom renderer soon */
  SDL_DestroyTexture(context->text_textures[menu_index]);
  if (!(context->text_textures[menu_index] = SDL_CreateTextureFromSurface(context->renderer, context->text_surfaces[menu_index]))) {
    fprintf(stderr, "ERROR > recreating text texture.\n%s\n", TTF_GetError());
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
  context->text_rects[menu_index].y = SCREEN_HEIGHT - context->text_rects[menu_index].h;
  if (menu_index % 2) {
    *offset += context->text_rects[menu_index].w;
  }
  if (! (menu_index % 2)) {
    context->text_rects[menu_index].y = SCREEN_HEIGHT - (context->text_rects[menu_index].h * 2);
  }
  return 0;

}

int SDL_UpdateContextMenu(SDL_Context* context, sort_info* si) {
  SDL_ReadContextMenu(si);
  int offset = 0; /* keep track of each items offset */
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
  /* skip if no flags set */
  if (!(context->update)) {
    return 0;
  }
  if (context->update & SCREEN_UPDATE) {
    SDL_UpdateContextScreenTexture(context);
    context->update &= ~SCREEN_UPDATE;
    context->render = 1;
  }
  if (context->update & MENU_UPDATE) {
    SDL_UpdateContextMenu(context, si);
    context->update &= ~MENU_UPDATE;
    context->render = 1;
  }

  return 0;
}

/**
 * handles inputs to adjust sort info
 * stores SDL event and and keyboard state in context
 */
int SDL_HandleContextInput(SDL_Context* context, sort_info* si) {
  /* check keyboard inputs */
  context->kbstate = SDL_GetKeyboardState(NULL);
  if (context->kbstate[SDL_SCANCODE_K]) {
    switch(context->menu_selection) {
      case 0:
        if (si->min_length < si->max_length) {
          si->min_length++;
          context->update |= SCREEN_UPDATE | MENU_UPDATE;
          context->render = 1;
        }
      break;
      case 1:
        si->max_length++;
        context->update |= SCREEN_UPDATE | MENU_UPDATE;
        context->render = 1;
      break;
      case 2:
        si->random_start++;
        context->update |= SCREEN_UPDATE | MENU_UPDATE;
        context->render = 1;
      break;
      case 3:
        si->random_stop++;
        context->update |= SCREEN_UPDATE | MENU_UPDATE;
        context->render = 1;
      break;
      case 4:
        si->start_threshold += 0.001;
        context->update |= SCREEN_UPDATE | MENU_UPDATE;
        context->render = 1;
      break;
      case 5:
        si->stop_threshold += 0.001;
        context->update |= SCREEN_UPDATE | MENU_UPDATE;
        context->render = 1;
      break;
      case 6:
        si->vertical_sort = si->vertical_sort ? 0 : 1;
        context->update |= SCREEN_UPDATE | MENU_UPDATE;
        context->render = 1;
      break;
    }
  }
  if (context->kbstate[SDL_SCANCODE_J]) {
    switch(context->menu_selection) {
      case 0:
        if (si->min_length) {
          si->min_length--;
          context->update |= SCREEN_UPDATE | MENU_UPDATE;
          context->render = 1;
        }
      break;
      case 1:
        if (si->max_length > si->min_length) {
          si->max_length--;
          context->update |= SCREEN_UPDATE | MENU_UPDATE;
          context->render = 1;
       }
      break;
      case 2:
        if (si->random_start) {
          si->random_start--;
          context->update |= SCREEN_UPDATE | MENU_UPDATE;
          context->render = 1;
        }
      break;
      case 3:
        if (si->random_stop) {
          si->random_stop--;
          context->update |= SCREEN_UPDATE | MENU_UPDATE;
          context->render = 1;
        }
      break;
      case 4:
        if (si->start_threshold) {
          si->start_threshold -= 0.001;
          context->update |= SCREEN_UPDATE | MENU_UPDATE;
          context->render = 1;
        }
      break;
      case 5:
        if (si->stop_threshold) {
          si->stop_threshold -= 0.001;
          context->update |= SCREEN_UPDATE | MENU_UPDATE;
          context->render = 1;
        }
      break;
      case 6:
        si->vertical_sort = si->vertical_sort ? 0 : 1;
        context->update |= SCREEN_UPDATE | MENU_UPDATE;
        context->render = 1;
      break;
    }
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
            if (context->menu_selection < MENU_LENGTH - 1) {
              context->menu_selection++;
              context->update |= MENU_UPDATE;
              context->render = 1;
            }
          break;
          case SDLK_h:
            if (context->menu_selection > 0) {
              context->menu_selection--;
              context->update |= MENU_UPDATE;
              context->render = 1;
            }
          break;
          case SDLK_v:
            si->vertical_sort = si->vertical_sort ? 0 : 1;
            context->update |= SCREEN_UPDATE | MENU_UPDATE;
            context->render = 1;
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
  if (!(context->render))
    return 0;

  /* clear and redraw screen */
  SDL_RenderClear(context->renderer);
  SDL_RenderCopy(context->renderer, context->screen_texture, NULL, NULL);

  /* add black under text */
  SDL_Rect menu_background = (SDL_Rect) {
    .x = context->text_rects[0].x,
    .y = context->text_rects[0].y,
    .w = SCREEN_WIDTH,
    .h = context->text_rects[0].h * 2
  };
  SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 255);
  SDL_RenderFillRect(context->renderer, &menu_background);
  /* render menu */
  for (int i = 0; i < MENU_LENGTH; i++) {
    SDL_RenderCopy(context->renderer, context->text_textures[i], NULL, &(context->text_rects[i]));
  }
  /* render and reset flags */
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
  SDL_FreeSurface(context->screen_surface);
  SDL_DestroyTexture(context->screen_texture);
  /* free text stuff  */
  for (int i = 0; i < MENU_LENGTH; i++) {
    SDL_FreeSurface(context->text_surfaces[i]);
    SDL_DestroyTexture(context->text_textures[i]);
  }
  /* quit libraries */
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}
