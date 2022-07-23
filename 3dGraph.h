#ifndef _3D_GRAPH_H
#define _3D_GRAPH_H

#include <SDL2/SDL.h>
#include "3dEngine.h"

int makeMeshFromFunction(tri *mesh, int n, float (*f)(float, float), float size, int divisions, SDL_Color c);


#endif