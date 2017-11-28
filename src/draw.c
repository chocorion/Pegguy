#include "../include/draw.h"

void drawGame(Game *game)
{
  clearScreen(game->screen);
  drawMap(game);
  drawHUD(game);
  /*dialogue(game, "Bonjour, je s'appelle Groot\n"
                  "Je suis ici pour vous guider\n"
                  "Etes-vous d'accord ?\\3"
                    "Et puis quoi encore ?\n"
                    "Hey ! Listen !\n"
                    "Ca me va, je suis un assiste\\"
                  "Bien, votre choix est fait\n"
                  "Enfin, il me semble ?\\2"
                    "Oui\n"
                    "Non\0", DEP_DIALOGUE_X, DEP_DIALOGUE_Y);*/
  SDL_RenderPresent(game->screen->pRenderer);
}


void calcul_dep(int *dep_x, int *dep_y, Game *game){
    if (game->perso->x > WINDOW_W/2){   //WINDOW_W/2 -> Début du scroll horizontal
        if (game->perso->x > game->wmap * 32 - WINDOW_W/2){//Fin du scroll
            *dep_x = game->wmap * 32 - WINDOW_W;
        } else {
            *dep_x = game->perso->x - WINDOW_W/2;
        }
    } else {
        *dep_x = 0;
    }

    //gestion du scroll vertical
    if (game->perso->y  > WINDOW_H/2){
        *dep_y = game->perso->y - WINDOW_H/2;
    } else {
        *dep_y = 0;
    }
}

//Affichage de ce que l'on voit à l'écran seulement

void drawMap(Game *game){
    int dep_x = 0;  //Nombre de pixel à passer sur la gauche
    int dep_y = 0;  //Nombre de pixel à passer sur le haut

    calcul_dep(&dep_x, &dep_y, game);

    //Gestion du scroll horizontal
    //Background
    drawImage(game->background, -dep_x/SCROLLING_BACKGROUND_X, -dep_y/SCROLLING_BACKGROUND_Y, game->screen->pRenderer);

    //On dessine la map
    for (int x = 0; x < WINDOW_W/32 + 1; x++){
        if (x + dep_x/32 >= game->wmap || x + dep_x/32 < 0)    break;   //Ne pas toucher

        for (int y = 0; y < WINDOW_H/32 + 1; y++){
            if (y + dep_y/32>= game->hmap)    break;                    //Ne pas toucher
            switch (game->map[x + dep_x/32][y + dep_y/32]->type){
                case GROUND:
                    drawImage(game->map[x + dep_x/32][y + dep_y/32]->image,
                                x*32 - dep_x%32,
                                y*32 - dep_y%32,
                                game->screen->pRenderer
                    );
                  break;
                case GROUND_2:
                    drawImage(game->map[x + dep_x/32][y + dep_y/32]->image,
                                x*32 - dep_x%32,
                                y*32 - dep_y%32,
                                game->screen->pRenderer
                    );
                  break;

            }
        }
    }
    /*
    *   Faudra afficher tous les objets dans cette fonction,
    *   ou alors renvoyer les valeurs de dep_x et dep_y
    *   et n'afficher que si c'est sur l'écran actuel
    */
    //Dessin de tous les objets dynamiques
    int nb_obj = game->nbDynObj;
    for (int i = 0; i < nb_obj; i++){
      if (game->mapObj[i]->active){
        drawImage(game->mapObj[i]->image,
                                game->mapObj[i]->x - dep_x,
                                game->mapObj[i]->y - dep_y,
                                game->screen->pRenderer
                    );
        }
    }
    bool cooldown = false;
    if(game->perso->invincible > 0){
        game->perso->invincible--;
        cooldown = true;
    }
    
    if (!cooldown || game->perso->invincible % 8 == 0){
        //Dessin des animations
        int frame_index = 0;
        if (game->perso->vSpeed !=0){                       //Image quand le personnage saute
            frame_index = 0;
        } else if (game->perso->hSpeed == 0){
            frame_index = 0;                                //perso immobile
        } else {
            //20 peut être diminué pour augmente la vitesse d'animation, et réciproquement
            frame_index = abs((game->perso->x/40)%(game->perso->nb_frame));
        }
        //Gestion du tems d'invincibilité du personnage
        

        drawImage(game->perso->image[game->perso->direction][frame_index],
                    game->perso->x - dep_x,
                    game->perso->y-dep_y,
                    game->screen->pRenderer
        );
        //Dessine ce que le personnage tient dans la main.
        //Il faudra appliquer le principe des frames
        if (game->perso->hand)
        {
        drawImage(game->perso->hand->image,
                    game->perso->hand->x - dep_x,
                    game->perso->hand->y-dep_y,
                    game->screen->pRenderer);
        }
    }
    drawDialogueNPCs(game, dep_x, dep_y);
    drawProjectiles(game, dep_x, dep_y);

    //Gérer les frames !
    mob *p_mob = game->first_mob;
    while (p_mob){
        draw_mob(game, p_mob);
        p_mob = p_mob->mob_next;
    }


    if (DEBUG){
        //Console de debug
        consol_d(game, dep_x, dep_y);
  }
}

void drawProjectiles(Game *game, int dep_x, int dep_y)
{
  Projectile *projectile2 = game->projectiles;
  while (projectile2)
  {
    drawImage(projectile2->dynObj->image,
                            projectile2->dynObj->x - dep_x,
                            projectile2->dynObj->y - dep_y,
                            game->screen->pRenderer);
    projectile2 = projectile2->following;
  }
}

void drawHUD(Game *game)
{
  SDL_Color color_text = {255, 255, 255, 255};

  //affiche le nombre de billes
  char text[3];
  sprintf(text, "%d", game->hud->nbBalls);
  SDL_Surface *text_surface = TTF_RenderText_Solid(game->font, text, color_text);
  if (!text_surface)
  {
      fprintf(stderr, "drawHUD error: Can't create surface\n");
      return;
  }
  SDL_Texture *text_texture = SDL_CreateTextureFromSurface(game->screen->pRenderer, text_surface);
  if (!text_texture)
  {
      fprintf(stderr, "drawHUD error: Can't create texure\n");
      return;
  }
  drawImage(text_texture, game->hud->xBall + 40, game->hud->yBall - 2, game->screen->pRenderer);
  drawImage(game->hud->ball, game->hud->xBall, game->hud->yBall, game->screen->pRenderer);

  drawImage(game->hud->hearts, game->hud->xHearts, game->hud->yHearts, game->screen->pRenderer);

  SDL_FreeSurface(text_surface);
  SDL_DestroyTexture(text_texture);
}

void consol_d(Game *game, int dep_x, int dep_y){
    char debug_text[50];
    int y = 0, x = 250;

    static int count = 0;
    static double save_t = 0;
    double t = SDL_GetTicks() - save_t;
    count ++;
    sprintf(debug_text, "   fps : %lf", count/(t/1000.0));
    if ((int)t%1000 == 0){
        count = 0;
        save_t = SDL_GetTicks();
    }
    print_line(game, x, y+=20, debug_text);

    sprintf(debug_text, "   dep_x : %d", dep_x);
    print_line(game, x, y+=20, debug_text);
    sprintf(debug_text, "   dep_y : %d", dep_y);
    print_line(game, x, y+=20, debug_text);
    sprintf(debug_text, "   x : %d", game->perso->x);
    print_line(game, x, y+=20, debug_text);
    sprintf(debug_text, "   y : %d", game->perso->y);
    print_line(game, x, y+=20, debug_text);
    sprintf(debug_text, "   x_screen : %d", game->perso->x - dep_x);
    print_line(game, x, y+=20, debug_text);
    sprintf(debug_text, "   y_screen : %d", game->perso->y - dep_y);
    print_line(game, x, y+=20, debug_text);
    sprintf(debug_text, "   life : %d", game->perso->hp);
    print_line(game, x, y+=20, debug_text);

}

void print_line(Game *game, int x, int y, char *debug_text){
    SDL_Color color_text = {0, 0, 0, 255};
    SDL_Surface *text_surface = TTF_RenderText_Solid(game->font, debug_text, color_text);
    if (!text_surface){
        fprintf(stderr, "consol_d error: Can't create surface\n");
        return;
    }
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(game->screen->pRenderer, text_surface);
    if (!text_texture){
        fprintf(stderr, "consol_d error: Can't create texure\n");
        return;
    }
    drawImage(text_texture, x, y, game->screen->pRenderer);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

void clearScreen(Screen *screen)
{
  SDL_SetRenderDrawColor(screen->pRenderer, 0, 0, 0, 255);
  SDL_RenderClear(screen->pRenderer);
}

void drawImage(SDL_Texture *texture, int x, int y, SDL_Renderer *pRenderer)
{
  SDL_Rect dest;
  dest.x = x;
  dest.y = y;
  SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
  SDL_RenderCopy(pRenderer, texture, NULL, &dest);
}
