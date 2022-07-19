#ifndef ENGINE_H
#define ENGINE_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 500

typedef struct{
    float x, y, z;
} vec3;

typedef struct{
    float u, v;
} vec2;

typedef struct{
    vec3 a, b, c;
    vec2 at, bt, ct;
    vec3 an, bn, cn;
    SDL_Color ac, bc, cc;
} tri;

typedef struct{
    vec3 pos;
    vec3 rot;
    float f;
} Camera;

vec3 vectorAdd(vec3 a, vec3 b);
vec3 vectorSubtract(vec3 a, vec3 b);
vec3 vecScalarMult(vec3 a, float b);

tri rotateTriangle(tri t, vec3 rot);

tri transformTriangle(tri t, Camera c);
tri projectTriangle(tri in, float a, float f);
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);

void renderMesh(tri *mesh, int n, Camera c, SDL_Texture *tex);

int loadObj(char *filename, tri *mesh, int n);

#endif