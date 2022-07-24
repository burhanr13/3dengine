#ifndef _3D_GRAPH_H
#define _3D_GRAPH_H

#include <SDL2/SDL.h>
#include "3dEngine.h"

typedef struct
{
    float umin, umax;
    int usteps;
    float vmin, vmax;
    int vsteps;
} paramBounds;

int makeMeshFromFunction(tri *mesh, int n, float (*f)(float, float), float size, int divisions, SDL_Color c);
int makeMeshFromParamFunction(tri *mesh, int n, vec3 (*f)(float, float), paramBounds b, SDL_Color c);

void writeMeshToObj(tri *mesh, int n, char *filename);

#endif