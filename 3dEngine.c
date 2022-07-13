#include <SDL2/SDL.h>

#include "3dEngine.h"

extern SDL_Renderer *renderer;

const float ASPECT_RATIO = (float)SCREEN_HEIGHT / SCREEN_WIDTH;

vec3 vectorAdd(vec3 a, vec3 b)
{
    return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3 vectorSubtract(vec3 a, vec3 b)
{
    return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3 vectorNegate(vec3 a)
{
    return (vec3){-a.x, -a.y, -a.z};
}

vec3 vecScalarMult(vec3 a, float b)
{
    return (vec3){b * a.x, b * a.y, b * a.z};
}

float dotProduct(vec3 a, vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 crossProduct(vec3 a, vec3 b)
{
    return (vec3){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

vec3 normalize(vec3 a)
{
    return vecScalarMult(a, 1 / sqrtf(a.x * a.x + a.y * a.y + a.z * a.z));
}

vec3 vectorRotate(vec3 a, vec3 rot)
{
    vec3 out;
    float sina = sinf(rot.y);
    float cosa = cosf(rot.y);
    out = (vec3){a.x * cosa + a.z * sina, a.y, a.z * cosa - a.x * sina};
    sina = sinf(rot.x);
    cosa = cosf(rot.x);
    out = (vec3){out.x, out.y * cosa + out.z * sina, out.z * cosa - out.y * sina};
    sina = sinf(rot.z);
    cosa = cosf(rot.z);
    out = (vec3){out.x * cosa + out.y * sina, out.y * cosa - out.x * sina, out.z};
    return out;
}

vec3 transformVector(vec3 in, Camera c)
{
    vec3 out = vectorSubtract(in, c.pos);
    return vectorRotate(out, vectorNegate(c.rot));
}

tri transformTriangle(tri t, Camera c)
{
    return (tri){transformVector(t.a, c), transformVector(t.b, c), transformVector(t.c, c)};
}

vec3 projectVector(vec3 in, float a, float f)
{
    if (in.z == 0)
        return (vec3){0, 0, 0};
    return (vec3){a * f * in.x / in.z, f * in.y / in.z, in.z};
}

tri projectTriangle(tri in, float a, float f)
{
    return (tri){projectVector(in.a, a, f), projectVector(in.b, a, f), projectVector(in.c, a, f)};
}

vec3 calculateNormal(tri t)
{
    return normalize(crossProduct(vectorSubtract(t.b, t.a), vectorSubtract(t.c, t.a)));
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
    SDL_Point pts[4] = {{x1, y1},
                        {x2, y2},
                        {x3, y3},
                        {x1, y1}};
    SDL_RenderDrawLines(renderer, pts, 4);
}

int compTri(const void *a, const void *b)
{
    tri m = *(tri *)a;
    tri n = *(tri *)b;
    float mz = m.a.z + m.b.z + m.c.z;
    float nz = n.a.z + n.b.z + n.c.z;
    return (int)(nz - mz);
}

void renderMesh(tri *mesh, int n, Camera c)
{

    tri triProj;
    vec3 norm;
    vec3 lightDir = normalize((vec3){-1, 0, -0.5});
    float shade;

    SDL_FPoint pts[3];
    SDL_Vertex vts[3];

    tri toDraw[n];
    int drawCount = 0;

    for (int i = 0; i < n; i++)
    {

        triProj = projectTriangle(transformTriangle(mesh[i], c), ASPECT_RATIO, c.f);

        norm = calculateNormal(triProj);
        if (norm.z > 0 || triProj.a.z < 0 || triProj.b.z < 0 || triProj.c.z < 0)
            continue;

        shade = (dotProduct(calculateNormal(mesh[i]), lightDir) + 1) / 2;
        triProj.color = (SDL_Color){30 * shade, 100 * shade, 255 * shade, 0xff};
        toDraw[drawCount] = triProj;
        drawCount++;
    }

    qsort(toDraw, drawCount, sizeof(tri), compTri);

    for (int i = 0; i < drawCount; i++)
    {

        pts[0] = (SDL_FPoint){(toDraw[i].a.x + 1) * SCREEN_WIDTH / 2, (1 - toDraw[i].a.y) * SCREEN_HEIGHT / 2};
        pts[1] = (SDL_FPoint){(toDraw[i].b.x + 1) * SCREEN_WIDTH / 2, (1 - toDraw[i].b.y) * SCREEN_HEIGHT / 2};
        pts[2] = (SDL_FPoint){(toDraw[i].c.x + 1) * SCREEN_WIDTH / 2, (1 - toDraw[i].c.y) * SCREEN_HEIGHT / 2};

        for (int j = 0; j < 3; j++)
        {
            vts[j] = (SDL_Vertex){pts[j], toDraw[i].color, {}};
        }

        SDL_RenderGeometry(renderer, NULL, vts, 3, NULL, 0);

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        // drawTriangle(pts[0].x, pts[0].y, pts[1].x, pts[1].y, pts[2].x, pts[2].y);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
}

int loadObj(char *filename, tri *mesh, int n)
{
    FILE *f = fopen(filename, "r");

    vec3 vts[n];
    int vcount = 0;
    int tcount = 0;

    int inda, indb, indc;

    char buf[255];
    char w[5];
    while (fgets(buf, 255, f) && vcount < n && tcount < n)
    {
        sscanf(buf, "%s", w);
        if (!strcmp(w, "v"))
        {
            sscanf(buf, "v %f %f %f", &vts[vcount].x, &vts[vcount].y, &vts[vcount].z);
            vcount++;
        }
        else if (!strcmp(w, "f"))
        {
            sscanf(buf, "f %d %d %d", &inda, &indb, &indc);
            mesh[tcount] = (tri){vts[inda - 1], vts[indb - 1], vts[indc - 1]};
            tcount++;
        }
    }

    fclose(f);

    return tcount;
}
