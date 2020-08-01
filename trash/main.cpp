#include<cstdio>

#define SDL_MAIN_HANDLED
#include<SDL2/SDL.h>

int main(){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        printf("error initializing SDL: %s\n", SDL_GetError()); 
    SDL_Window* win = SDL_CreateWindow("GAME",  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 1000, 0); 
    while(true){

    }
    SDL_DestroyWindow(win); 
}
