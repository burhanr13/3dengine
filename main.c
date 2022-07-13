#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "3dEngine.h"

void init();
void close();
void handleEvent(SDL_Event *e);
void updateCamera();

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

// tri mesh[] = {{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}}, {{1, 0, 0}, {1, 1, 0}, {0, 1, 0}}, {{0, 0, 0}, {0, 0, 1}, {1, 0, 0}}, {{0, 0, 1}, {1, 0, 1}, {1, 0, 0}}, {{0, 0, 0}, {0, 1, 0}, {0, 0, 1}}, {{0, 1, 0}, {0, 1, 1}, {0, 0, 1}}, {{0, 0, 1}, {0, 1, 1}, {1, 0, 1}}, {{0, 1, 1}, {1, 1, 1}, {1, 0, 1}}, {{0, 1, 0}, {1, 1, 0}, {0, 1, 1}}, {{1, 1, 0}, {1, 1, 1}, {0, 1, 1}}, {{1, 0, 0}, {1, 0, 1}, {1, 1, 0}}, {{1, 0, 1}, {1, 1, 1}, {1, 1, 0}}};

tri mesh[10000];
int nTris;

Camera cam = {{0, 0, -10}, {0, 0, 0}, 2};

int quit = 0;

int main(int argc, char *argv[])
{
    init();

    SDL_Event e;

    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            handleEvent(&e);
        }

        updateCamera();

        SDL_RenderClear(renderer);

        renderMesh(mesh, nTris, cam);

        SDL_RenderPresent(renderer);
    }

    close();

    return 0;
}

void init()
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    window = SDL_CreateWindow("Template", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    nTris = loadObj("teapot.obj", mesh, 10000);
}

void close()
{
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_Quit();
    IMG_Quit();
}

void handleEvent(SDL_Event *e)
{
    switch (e->type)
    {
    case SDL_QUIT:
        quit = 1;
        break;
    }
}

void updateCamera()
{
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    cam.pos = vectorAdd(cam.pos, 
                        vecScalarMult((vec3){((keys[SDL_SCANCODE_W] - keys[SDL_SCANCODE_S]) * sinf(cam.rot.y) +
                                         (keys[SDL_SCANCODE_D] - keys[SDL_SCANCODE_A]) * cosf(cam.rot.y)),
                                        (keys[SDL_SCANCODE_APOSTROPHE] - keys[SDL_SCANCODE_SLASH]),
                                        ((keys[SDL_SCANCODE_W] - keys[SDL_SCANCODE_S]) * cosf(cam.rot.y) +
                                         (keys[SDL_SCANCODE_D] - keys[SDL_SCANCODE_A]) * -sinf(cam.rot.y))},0.01));
    cam.rot = vectorAdd(cam.rot,    
                        vecScalarMult((vec3){(keys[SDL_SCANCODE_UP] - keys[SDL_SCANCODE_DOWN]), 
                                             (keys[SDL_SCANCODE_RIGHT] - keys[SDL_SCANCODE_LEFT]),0}, 0.001));
}