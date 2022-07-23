#include "3dEngine.h"
#include <math.h>

int makeMeshFromFunction(tri *mesh, int n, float (*f)(float, float), float size, int divisions, SDL_Color c)
{
    float step = 2 * size / divisions;
    float x, y, x2, y2;
    vec3 v[4];
    int tcount = 0;
    float shade;
    vec3 lightDir = {-1, -1, -1};
    SDL_Color curCol;
    for (int i = 0; i < divisions; i++)
    {
        for (int j = 0; j < divisions; j++)
        {
            if(tcount>n-2)
                return tcount;
            x = i * step - size;
            y = j * step - size;
            x2 = x + step;
            y2 = y + step;
            v[0] = (vec3){x, f(x, y), y};
            v[1] = (vec3){x2, f(x2, y), y};
            v[2] = (vec3){x2, f(x2, y2), y2};
            v[3] = (vec3){x, f(x, y2), y2};

            if (fabsf(v[0].y) > size || fabsf(v[1].y) > size || 
                fabsf(v[2].y) > size || fabsf(v[3].y) > size)
                continue;

                mesh[tcount] = (tri){v[0], v[1], v[2]};
            mesh[tcount + 1] = (tri){v[0], v[2], v[3]};
            mesh[tcount].cull = 0;
            mesh[tcount + 1].cull = 0;

            shade = ((1 - dotProduct(calculateNormal(mesh[tcount]), normalize(lightDir))) * 0.375 + 0.25) * 
                    ((i % 2 == j % 2) ? 1 : 0.5);
            curCol = (SDL_Color){c.r * shade, c.g * shade, c.b * shade, 0xff};

            mesh[tcount].ac = curCol;
            mesh[tcount].bc = curCol;
            mesh[tcount].cc = curCol;
            mesh[tcount+1].ac = curCol;
            mesh[tcount+1].bc = curCol;
            mesh[tcount+1].cc = curCol;

            tcount += 2;
        }
    }
    return tcount;
}