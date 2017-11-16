#include "../include/load.h"

Game *loadGame()
{
  Game *game = malloc(sizeof(Game));
  game->screen = initScreen("They Come in Peace");
  game->perso = loadPerso(game);
  game->input = generateInput();
  game->wmap = 30;
  game->hmap = 20;
  game->map = malloc(game->wmap*sizeof(Bloc));
  if (!game->map)
  {
    printf("Unable to malloc map\n");
    exit(EXIT_FAILURE);
  }
  for (uint i=0; i<game->wmap; i++)
  {
    game->map[i] = malloc(game->hmap*sizeof(Bloc));
    if (!game->map[i])
    {
      printf("Unable to calloc map %d\n", i);
      exit(EXIT_FAILURE);
    }
  }
  for (int x=0; x<game->wmap; x++)
  {
    for (int y=0; y<game->hmap; y++)
    {
      game->map[x][y]->type = 0;
    }
  }
  game->map[0][0]->type = 1;
  game->map[1][4]->type = 1;
  game->map[0][3]->type = 1;
  game->map[10][10]->type = 1;
  game->map[8][6]->type = 1;

  return game;
}

Perso *loadPerso(Game *game)
{
  Perso *perso = malloc(sizeof(Perso));
  perso->image = loadTexture("../graphics/perso.png", game->screen->pRenderer);
  perso->w = 32;
  perso->h = 64;
  perso->x = 100;
  perso->y = 100;
  perso->hSpeed = 5;
  perso->vSpeed = 5;

  return perso;
}


SDL_Texture *loadTexture(char *image, SDL_Renderer *pRenderer)
{
  SDL_Surface *surface = IMG_Load(image);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(pRenderer, surface);
  SDL_FreeSurface(surface);

  return texture;
}
