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

vec3 vecScalarDiv(vec3 a, float b)
{
    return (vec3){a.x / b, a.y / b, a.z / b};
}

float dotProduct(vec3 a, vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 crossProduct(vec3 a, vec3 b)
{
    return (vec3){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

int isVectorZero(vec3 a)
{
    return a.x == 0.0f && a.y == 0.0f && a.z == 0.0f;
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

tri rotateTriangle(tri t, vec3 rot)
{
    tri out = t;
    out.a = vectorRotate(t.a, rot);
    out.an = vectorRotate(t.an, rot);
    out.b = vectorRotate(t.b, rot);
    out.bn = vectorRotate(t.bn, rot);
    out.c = vectorRotate(t.c, rot);
    out.cn = vectorRotate(t.cn, rot);
    return out;
}

vec3 transformVector(vec3 in, Camera c)
{
    vec3 out = vectorSubtract(in, c.pos);
    return vectorRotate(out, vectorNegate(c.rot));
}

tri transformTriangle(tri t, Camera c)
{
    tri out = t;
    out.a = transformVector(t.a, c);
    out.b = transformVector(t.b, c);
    out.c = transformVector(t.c, c);
    return out;
}

vec3 projectVector(vec3 in, float a, float f)
{
    if (in.z == 0)
        return (vec3){0, 0, 0};
    return (vec3){a * f * in.x / in.z, f * in.y / in.z, in.z};
}

tri projectTriangle(tri in, float a, float f)
{
    tri out = in;
    out.a = projectVector(in.a, a, f);
    out.b = projectVector(in.b, a, f);
    out.c = projectVector(in.c, a, f);
    return out;
}

vec3 calculateNormal(tri t)
{
    return normalize(crossProduct(vectorSubtract(t.c, t.a), vectorSubtract(t.b, t.a)));
}

float signed2dArea(tri t)
{
    vec3 a = vectorSubtract(t.c, t.a);
    vec3 b = vectorSubtract(t.b, t.a);
    return a.x * b.y - a.y * b.x;
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

void sort3(float x[4]){
    if (x[0] > x[1])
    {
        x[3] = x[0];
        x[0] = x[1];
        x[1] = x[3];
    }
    if (x[1] > x[2])
    {
        x[3] = x[1];
        x[1] = x[2];
        x[2] = x[3];
    }
    if (x[0] > x[1])
    {
        x[3] = x[0];
        x[0] = x[1];
        x[1] = x[3];
    }
}

float calculateLighting(vec3 p, vec3 n, vec3 l, tri *mesh, int nTris)
{

    float dp = dotProduct(n, l);
    //if(dp>=0)
        return (1 - dp) / 2;

    vec3 tn;

    int k = 0;

    float t;
    vec3 pTri;
    float x[4];

    for (int i = 0; i < nTris; i++)
    {
        tn = calculateNormal(mesh[i]);
        t = -dotProduct(vectorSubtract(p, mesh[i].a), tn) / dotProduct(vectorNegate(l), tn);
        if (t <= 0)
            continue;

        pTri = vectorAdd(p, vecScalarMult(l, -t));

        x[0] = mesh[i].a.x;
        x[1] = mesh[i].b.x;
        x[2] = mesh[i].c.x;
        sort3(x);

        if (x[0] > pTri.x || pTri.x > x[2])
            continue;
        
        x[0] = mesh[i].a.y;
        x[1] = mesh[i].b.y;
        x[2] = mesh[i].c.y;
        sort3(x);

        if (x[0] > pTri.y || pTri.y > x[2])
            continue;

        x[0] = mesh[i].a.z;
        x[1] = mesh[i].b.z;
        x[2] = mesh[i].c.z;
        sort3(x);

        if (x[0] > pTri.z || pTri.z > x[2])
            continue;

        return 0;
    }

    return (1 - dp) / 2;
}

void calculateVertexColors(tri *mesh, int n, vec3 lightDir)
{

    lightDir = normalize(lightDir);
    float shade;

    for (int i = 0; i < n; i++)
    {

        if (!isVectorZero(mesh[i].an))
        {
            shade = calculateLighting(mesh[i].a, mesh[i].an, lightDir, mesh, n);
            mesh[i].ac = (SDL_Color){255 * shade, 255 * shade, 255 * shade, 0xff};
            shade = calculateLighting(mesh[i].b, mesh[i].bn, lightDir, mesh, n);
            mesh[i].bc = (SDL_Color){255 * shade, 255 * shade, 255 * shade, 0xff};
            shade = calculateLighting(mesh[i].c, mesh[i].cn, lightDir, mesh, n);
            mesh[i].cc = (SDL_Color){255 * shade, 255 * shade, 255 * shade, 0xff};
        }
        else
        {
            shade = calculateLighting(vecScalarDiv(vectorAdd(mesh[i].a, vectorAdd(mesh[i].b, mesh[i].c)), 3), calculateNormal(mesh[i]), lightDir, mesh, n);
            mesh[i].ac = (SDL_Color){255 * shade, 255 * shade, 255 * shade, 0xff};
            mesh[i].bc = (SDL_Color){255 * shade, 255 * shade, 255 * shade, 0xff};
            mesh[i].cc = (SDL_Color){255 * shade, 255 * shade, 255 * shade, 0xff};
        }
    }
}

void renderMesh(tri *mesh, int n, Camera c, SDL_Texture *tex)
{

    tri triProj;
    float area;

    SDL_FPoint pts[3];
    SDL_Vertex vts[3];

    tri toDraw[n];
    int drawCount = 0;

    for (int i = 0; i < n; i++)
    {

        triProj = projectTriangle(transformTriangle(mesh[i], c), ASPECT_RATIO, c.f);

        area = signed2dArea(triProj);
        if (area < 0 && triProj.cull || triProj.a.z < 0 || triProj.b.z < 0 || triProj.c.z < 0)
            continue;

        toDraw[drawCount] = triProj;
        drawCount++;
    }

    qsort(toDraw, drawCount, sizeof(tri), compTri);

    for (int i = 0; i < drawCount; i++)
    {

        pts[0] = (SDL_FPoint){(toDraw[i].a.x + 1) * SCREEN_WIDTH / 2, (1 - toDraw[i].a.y) * SCREEN_HEIGHT / 2};
        pts[1] = (SDL_FPoint){(toDraw[i].b.x + 1) * SCREEN_WIDTH / 2, (1 - toDraw[i].b.y) * SCREEN_HEIGHT / 2};
        pts[2] = (SDL_FPoint){(toDraw[i].c.x + 1) * SCREEN_WIDTH / 2, (1 - toDraw[i].c.y) * SCREEN_HEIGHT / 2};

        vts[0] = (SDL_Vertex){pts[0], toDraw[i].ac, {toDraw[i].at.u, toDraw[i].at.v}};
        vts[1] = (SDL_Vertex){pts[1], toDraw[i].bc, {toDraw[i].bt.u, toDraw[i].bt.v}};
        vts[2] = (SDL_Vertex){pts[2], toDraw[i].cc, {toDraw[i].ct.u, toDraw[i].ct.v}};

        SDL_RenderGeometry(renderer, tex, vts, 3, NULL, 0);

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        // drawTriangle(pts[0].x, pts[0].y, pts[1].x, pts[1].y, pts[2].x, pts[2].y);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
}

int loadObj(char *filename, tri *mesh, int n)
{
    FILE *f = fopen(filename, "r");

    vec3 v[n];
    vec2 vt[n];
    vec3 vn[n];
    int vcount = 0;
    int vtcount = 0;
    int vncount = 0;
    int tcount = 0;

    int inda = 0, indb = 0, indc = 0, indd = 0;
    int indta = 0, indtb = 0, indtc = 0, indtd = 0;
    int indna = 0, indnb = 0, indnc = 0, indnd = 0;
    int nverts;

    char buf[255];
    char w[5];
    while (fgets(buf, 255, f) && vcount < n && vtcount < n && vncount < n && tcount < n)
    {
        sscanf(buf, "%s", w);
        if (!strcmp(w, "v"))
        {
            sscanf(buf, "v %f %f %f", &v[vcount].x, &v[vcount].y, &v[vcount].z);
            vcount++;
        }
        else if (!strcmp(w, "vt"))
        {
            sscanf(buf, "vt %f %f", &vt[vtcount].u, &vt[vtcount].v);
            // vt[vtcount].u = fmodf(fmodf(vt[vtcount].u, 1) + 1, 1);
            // vt[vtcount].v = fmodf(fmodf(vt[vtcount].v, 1) + 1, 1);
            vtcount++;
        }
        else if (!strcmp(w, "vn"))
        {
            sscanf(buf, "vn %f %f %f", &vn[vncount].x, &vn[vncount].y, &vn[vncount].z);
            vn[vncount] = normalize(vn[vncount]);
            vncount++;
        }
        else if (!strcmp(w, "f"))
        {
            if (vtcount > 0 && vncount > 0)
            {
                nverts = sscanf(buf, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &inda, &indta, &indna,
                                &indb, &indtb, &indnb,
                                &indc, &indtc, &indnc,
                                &indd, &indtd, &indnd);
                nverts /= 3;
            }
            else if (vncount > 0)
            {
                nverts = sscanf(buf, "f %d//%d %d//%d %d//%d %d//%d", &inda, &indna,
                                &indb, &indnb,
                                &indc, &indnc,
                                &indd, &indnd);
                nverts /= 2;
            }
            else if (vtcount > 0)
            {
                nverts = sscanf(buf, "f %d/%d%*s %d/%d%*s %d/%d%*s %d/%d%*s", &inda, &indta,
                                &indb, &indtb,
                                &indc, &indtc,
                                &indd, &indtd);
                nverts /= 2;
            }
            else
            {
                nverts = sscanf(buf, "f %d %d %d %d", &inda, &indb, &indc, &indd);
            }
            mesh[tcount] = (tri){v[inda - 1], v[indb - 1], v[indc - 1],
                                 vt[indta - 1], vt[indtb - 1], vt[indtc - 1],
                                 vn[indna - 1], vn[indnb - 1], vn[indnc - 1], 1};
            tcount++;

            if (nverts > 3)
            {
                mesh[tcount] = (tri){v[inda - 1], v[indc - 1], v[indd - 1],
                                     vt[indta - 1], vt[indtc - 1], vt[indtd - 1],
                                     vn[indna - 1], vn[indnc - 1], vn[indnd - 1], 1};
                tcount++;
            }
        }
    }

    fclose(f);

    return tcount;
}