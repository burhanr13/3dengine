#include "3dEngine.h"
#include "3dGraph.h"
#include <math.h>
#include <stdio.h>

int makeMeshFromFunction(tri *mesh, int n, float (*f)(float, float), float size, int divisions, SDL_Color c)
{
    float step = 2 * size / divisions;
    float x, y, x2, y2;
    vec3 v[4];
    int tcount = 0;
    vec3 norm;
    float shade;
    vec3 lightDir = {-1, -1, -1};
    SDL_Color curCol;
    for (int i = 0; i < divisions; i++)
    {
        for (int j = 0; j < divisions; j++)
        {
            if (tcount > n - 2)
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

            mesh[tcount] = (tri){v[2], v[1], v[0]};
            mesh[tcount + 1] = (tri){v[3], v[2], v[0]};
            mesh[tcount].cull = 0;
            mesh[tcount + 1].cull = 0;

            norm = calculateNormal(mesh[tcount]);
            norm = isVectorZero(norm) ? calculateNormal(mesh[tcount + 1]) : norm;

            shade = ((1 - dotProduct(norm, normalize(lightDir))) * 0.375 + 0.25) *
                    ((i % 2 == j % 2) ? 1 : 0.5);
            curCol = (SDL_Color){c.r * shade, c.g * shade, c.b * shade, 0xff};

            mesh[tcount].ac = curCol;
            mesh[tcount].bc = curCol;
            mesh[tcount].cc = curCol;
            mesh[tcount + 1].ac = curCol;
            mesh[tcount + 1].bc = curCol;
            mesh[tcount + 1].cc = curCol;

            tcount += 2;
        }
    }
    return tcount;
}

int makeMeshFromParamFunction(tri *mesh, int n, vec3 (*f)(float, float), paramBounds b, SDL_Color c)
{
    float istep = (b.umax - b.umin) / b.usteps;
    float jstep = (b.vmax - b.vmin) / b.vsteps;
    float u, v, u2, v2;
    vec3 pts[4];
    int tcount = 0;
    float shade;
    vec3 lightDir = {-1, -1, -1};
    vec3 norm;
    SDL_Color curCol;
    for (int i = 0; i < b.usteps; i++)
    {
        for (int j = 0; j < b.vsteps; j++)
        {
            if (tcount > n - 2)
                return tcount;
            u = i * istep + b.umin;
            v = j * jstep + b.vmin;
            u2 = u + istep;
            v2 = v + jstep;
            pts[0] = f(u, v);
            pts[1] = f(u2, v);
            pts[2] = f(u2, v2);
            pts[3] = f(u, v2);

            for (int m = 0; m < 4; m++)
                pts[m] = vecSwapYZ(pts[m]);

            mesh[tcount] = (tri){pts[0], pts[1], pts[2]};
            mesh[tcount + 1] = (tri){pts[0], pts[2], pts[3]};
            mesh[tcount].cull = 0;
            mesh[tcount + 1].cull = 0;

            norm = calculateNormal(mesh[tcount]);
            norm = isVectorZero(norm) ? calculateNormal(mesh[tcount + 1]) : norm;

            shade = ((1 + dotProduct(norm, normalize(lightDir))) * 0.375 + 0.25) *
                    ((i % 2 == j % 2) ? 1 : 0.5);
            curCol = (SDL_Color){c.r * shade, c.g * shade, c.b * shade, 0xff};

            mesh[tcount].ac = curCol;
            mesh[tcount].bc = curCol;
            mesh[tcount].cc = curCol;
            mesh[tcount + 1].ac = curCol;
            mesh[tcount + 1].bc = curCol;
            mesh[tcount + 1].cc = curCol;

            tcount += 2;
        }
    }
    return tcount;
}

void writeMeshToObj(tri *mesh, int n, char *filename)
{
    FILE *f = fopen(filename, "w");
    for (int i = 0; i < n;i++){
        fprintf(f, "v %f %f %f\n", mesh[i].a.x, mesh[i].a.y, mesh[i].a.z);
        fprintf(f, "v %f %f %f\n", mesh[i].b.x, mesh[i].b.y, mesh[i].b.z);
        fprintf(f, "v %f %f %f\n", mesh[i].c.x, mesh[i].c.y, mesh[i].c.z);
        fprintf(f, "f %d %d %d\n", 3 * i + 1, 3 * i + 2, 3 * i + 3);
    }
}