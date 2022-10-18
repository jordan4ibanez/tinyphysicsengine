#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>

#define S3L_RESOLUTION_X 640
#define S3L_RESOLUTION_Y 480
#define S3L_PIXEL_FUNCTION drawPixel

#define S3L_FLAT 0
#define S3L_NEAR_CROSS_STRATEGY 1
#define S3L_PERSPECTIVE_CORRECTION 2
#define S3L_SORT 0
#define S3L_STENCIL_BUFFER 0
#define S3L_Z_BUFFER 1

#define FPS 30
#define MSPF (1000 / (FPS))

#include "small3dlib.h"

#include "tinyphysicsengine.h"

#define PIXELS_SIZE (S3L_RESOLUTION_X * S3L_RESOLUTION_Y * 4)

uint8_t pixels[PIXELS_SIZE];

void draw2DPoint(int x, int y, int r, int g, int b)
{
  if (x < 1 || x > S3L_RESOLUTION_X - 1 ||
      y < 1 || y > S3L_RESOLUTION_Y - 1)
    return;

  uint32_t index = ((y - 1) * S3L_RESOLUTION_X + x) * 4;

  #define d pixels[index] = 0; pixels[index + 1] = b; pixels[index + 2] = g; pixels[index + 3] = r;

  d
  index += S3L_RESOLUTION_X * 4 - 4;
  d
  index += 4;
  d
  index += 4;
  d
  index += S3L_RESOLUTION_X * 4 - 4;
  d

  #undef d
}

void drawPixel(S3L_PixelInfo *p)
{
  uint32_t index = (p->y * S3L_RESOLUTION_X + p->x) * 4;
  pixels[index + 1] = p->triangleIndex * 16;
  pixels[index + 2] = 255 - p->triangleIndex * 16;
}

S3L_Unit cubeVertices[] = { S3L_CUBE_VERTICES(S3L_FRACTIONS_PER_UNIT) };
S3L_Index cubeTriangles[] = { S3L_CUBE_TRIANGLES };

#define CYLINDER_VERTEX_COUNT 20
const S3L_Unit cylinderVertices[CYLINDER_VERTEX_COUNT * 3] = {
      0,  -256,   512,        // 0
      0,   256,   512,        // 3
    300,  -256,   414,        // 6
    300,   256,   414,        // 9
    486,  -256,   158,        // 12
    486,   256,   158,        // 15
    486,  -256,  -158,        // 18
    486,   256,  -158,        // 21
    300,  -256,  -414,        // 24
    300,   256,  -414,        // 27
      0,  -256,  -512,        // 30
      0,   256,  -512,        // 33
   -300,  -256,  -414,        // 36
   -300,   256,  -414,        // 39
   -486,  -256,  -158,        // 42
   -486,   256,  -158,        // 45
   -486,  -256,   158,        // 48
   -486,   256,   158,        // 51
   -300,  -256,   414,        // 54
   -300,   256,   414         // 57
}; // cylinderVertices

#define CYLINDER_TRIANGLE_COUNT 36
const S3L_Index cylinderTriangleIndices[CYLINDER_TRIANGLE_COUNT * 3] = {
      1,     2,     0,        // 0
      3,     4,     2,        // 3
      5,     6,     4,        // 6
      7,     8,     6,        // 9
      9,    10,     8,        // 12
     11,    12,    10,        // 15
     13,    14,    12,        // 18
     15,    16,    14,        // 21
     17,     7,     5,        // 24
     17,    18,    16,        // 27
     19,     0,    18,        // 30
      6,    14,    18,        // 33
      1,     3,     2,        // 36
      3,     5,     4,        // 39
      5,     7,     6,        // 42
      7,     9,     8,        // 45
      9,    11,    10,        // 48
     11,    13,    12,        // 51
     13,    15,    14,        // 54
     15,    17,    16,        // 57
      5,     3,    17,        // 60
      1,    19,    17,        // 63
     17,    15,    13,        // 66
     13,    11,    17,        // 69
      9,     7,    17,        // 72
      3,     1,    17,        // 75
     17,    11,     9,        // 78
     17,    19,    18,        // 81
     19,     1,     0,        // 84
     18,     0,     2,        // 87
      2,     4,     6,        // 90
      6,     8,    10,        // 93
     10,    12,    14,        // 96
     14,    16,    18,        // 99
     18,     2,     6,        // 102
      6,    10,    14         // 105
}; // cylinderTriangleIndices

#define SPHERE_VERTEX_COUNT 42
const S3L_Unit sphereVertices[SPHERE_VERTEX_COUNT * 3] = {
      0,  -512,     0,        // 0
    370,  -228,  -269,        // 3
   -141,  -228,  -435,        // 6
   -457,  -228,     0,        // 9
   -141,  -228,   435,        // 12
    370,  -228,   269,        // 15
    141,   228,  -435,        // 18
   -370,   228,  -269,        // 21
   -370,   228,   269,        // 24
    141,   228,   435,        // 27
    457,   228,     0,        // 30
      0,   512,     0,        // 33
    -83,  -435,  -255,        // 36
    217,  -435,  -158,        // 39
    134,  -269,  -414,        // 42
    435,  -269,     0,        // 45
    217,  -435,   158,        // 48
   -269,  -435,     0,        // 51
   -352,  -269,  -255,        // 54
    -83,  -435,   255,        // 57
   -352,  -269,   255,        // 60
    134,  -269,   414,        // 63
    486,     0,  -158,        // 66
    486,     0,   158,        // 69
      0,     0,  -512,        // 72
    300,     0,  -414,        // 75
   -486,     0,  -158,        // 78
   -300,     0,  -414,        // 81
   -300,     0,   414,        // 84
   -486,     0,   158,        // 87
    300,     0,   414,        // 90
      0,     0,   512,        // 93
    352,   269,  -255,        // 96
   -134,   269,  -414,        // 99
   -435,   269,     0,        // 102
   -134,   269,   414,        // 105
    352,   269,   255,        // 108
     83,   435,  -255,        // 111
    269,   435,     0,        // 114
   -217,   435,  -158,        // 117
   -217,   435,   158,        // 120
     83,   435,   255         // 123
}; // sphereVertices

#define SPHERE_TRIANGLE_COUNT 80
const S3L_Index sphereTriangleIndices[SPHERE_TRIANGLE_COUNT * 3] = {
      0,    13,    12,        // 0
      1,    13,    15,        // 3
      0,    12,    17,        // 6
      0,    17,    19,        // 9
      0,    19,    16,        // 12
      1,    15,    22,        // 15
      2,    14,    24,        // 18
      3,    18,    26,        // 21
      4,    20,    28,        // 24
      5,    21,    30,        // 27
      1,    22,    25,        // 30
      2,    24,    27,        // 33
      3,    26,    29,        // 36
      4,    28,    31,        // 39
      5,    30,    23,        // 42
      6,    32,    37,        // 45
      7,    33,    39,        // 48
      8,    34,    40,        // 51
      9,    35,    41,        // 54
     10,    36,    38,        // 57
     38,    41,    11,        // 60
     38,    36,    41,        // 63
     36,     9,    41,        // 66
     41,    40,    11,        // 69
     41,    35,    40,        // 72
     35,     8,    40,        // 75
     40,    39,    11,        // 78
     40,    34,    39,        // 81
     34,     7,    39,        // 84
     39,    37,    11,        // 87
     39,    33,    37,        // 90
     33,     6,    37,        // 93
     37,    38,    11,        // 96
     37,    32,    38,        // 99
     32,    10,    38,        // 102
     23,    36,    10,        // 105
     23,    30,    36,        // 108
     30,     9,    36,        // 111
     31,    35,     9,        // 114
     31,    28,    35,        // 117
     28,     8,    35,        // 120
     29,    34,     8,        // 123
     29,    26,    34,        // 126
     26,     7,    34,        // 129
     27,    33,     7,        // 132
     27,    24,    33,        // 135
     24,     6,    33,        // 138
     25,    32,     6,        // 141
     25,    22,    32,        // 144
     22,    10,    32,        // 147
     30,    31,     9,        // 150
     30,    21,    31,        // 153
     21,     4,    31,        // 156
     28,    29,     8,        // 159
     28,    20,    29,        // 162
     20,     3,    29,        // 165
     26,    27,     7,        // 168
     26,    18,    27,        // 171
     18,     2,    27,        // 174
     24,    25,     6,        // 177
     24,    14,    25,        // 180
     14,     1,    25,        // 183
     22,    23,    10,        // 186
     22,    15,    23,        // 189
     15,     5,    23,        // 192
     16,    21,     5,        // 195
     16,    19,    21,        // 198
     19,     4,    21,        // 201
     19,    20,     4,        // 204
     19,    17,    20,        // 207
     17,     3,    20,        // 210
     17,    18,     3,        // 213
     17,    12,    18,        // 216
     12,     2,    18,        // 219
     15,    16,     5,        // 222
     15,    13,    16,        // 225
     13,     0,    16,        // 228
     12,    14,     2,        // 231
     12,    13,    14,        // 234
     13,     1,    14         // 237
}; // sphereTriangleIndices

typedef struct
{
  S3L_Mat4 matrix;
  S3L_Mat4 scaleMatrix;
} BodyModelExtra;

int bodyCount = 0;

S3L_Scene scene;
S3L_Model3D models[1024];

TPE_World world;
TPE_Body bodies[1024];

BodyModelExtra extra[1024];

void addBody(uint8_t shape, TPE_Unit mass, TPE_Unit param1, TPE_Unit param2, TPE_Unit param3)
{
  BodyModelExtra e;
  TPE_Body b;
  S3L_Model3D m;

  TPE_bodyInit(&b);

  b.shape = shape;
  b.mass = mass;
  b.shapeParams[0] = param1;
  b.shapeParams[1] = param2;
  b.shapeParams[2] = param3;

  TPE_bodyRecomputeBounds(&b);

  const S3L_Unit *v;
  const S3L_Index *t;

  S3L_Index vc, tc;
  S3L_Unit sx = S3L_FRACTIONS_PER_UNIT, sy = S3L_FRACTIONS_PER_UNIT, sz = S3L_FRACTIONS_PER_UNIT;

  switch (shape)
  {
    case TPE_SHAPE_CYLINDER:
    case TPE_SHAPE_CAPSULE:
      v = cylinderVertices;
      t = cylinderTriangleIndices;
      vc = CYLINDER_VERTEX_COUNT;
      tc = CYLINDER_TRIANGLE_COUNT;
      sx = param1; sy = param2; sz = param1;
      break;

    case TPE_SHAPE_SPHERE:
    default:
      v = sphereVertices;
      t = sphereTriangleIndices;
      vc = SPHERE_VERTEX_COUNT;
      tc = SPHERE_TRIANGLE_COUNT;
      sx = param1; sy = param1; sz = param1;
      break;

    case TPE_SHAPE_CUBOID:
      v = cubeVertices;
      t = cubeTriangles;
      vc = S3L_CUBE_VERTEX_COUNT;
      tc = S3L_CUBE_TRIANGLE_COUNT;
      sx = param1; sy = param2; sz = param3;
      break;
  }

  S3L_initModel3D(v,vc,t,tc,&m);
  S3L_makeScaleMatrix(sx,sy,sz,e.scaleMatrix);

  S3L_initMat4(e.matrix);
  extra[bodyCount] = e;

  m.customTransformMatrix = &(extra[bodyCount].matrix);

  bodies[bodyCount] = b;
  models[bodyCount] = m;

  scene.modelCount++;
  world.bodyCount++;

  bodyCount++;
}

void updateBodies()
{
  for (int i = 0; i < bodyCount; ++i)
  {
    BodyModelExtra *e = &(extra[i]);

    S3L_Mat4 m;

    TPE_bodyGetTransformMatrix(&(bodies[i]),m);
    S3L_copyMat4(e->scaleMatrix,e->matrix);
    S3L_mat4Xmat4(e->matrix,m);
  }
}

int main()
{
  SDL_Window *window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, S3L_RESOLUTION_X, S3L_RESOLUTION_Y, SDL_WINDOW_SHOWN); 
  SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);
  SDL_Texture *textureSDL = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STATIC, S3L_RESOLUTION_X, S3L_RESOLUTION_Y);
  SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
  SDL_Event event;

  int running = 1;

  S3L_initScene(models,0,&scene);
  TPE_worldInit(&world);
  world.bodies = bodies;
  
  TPE_Unit frame = 0;

  //-------

  scene.camera.transform.translation.z = -8 * S3L_FRACTIONS_PER_UNIT;
  scene.camera.transform.translation.y = 2 * S3L_FRACTIONS_PER_UNIT;

  addBody(TPE_SHAPE_CUBOID,4024,1000,2000,3000);
  addBody(TPE_SHAPE_CUBOID,TPE_INFINITY,5000,1000,5000);
  addBody(TPE_SHAPE_CUBOID,4024,1000,1000,1500);
 
  bodies[0].position = TPE_vec4(0,3000,0,0);
  bodies[1].position = TPE_vec4(0,-1000,0,0);
  bodies[0].velocity = TPE_vec4(0,0,0,0);
  bodies[2].position.x = 2000;
  bodies[2].position.y = 2000;

  TPE_Vec4 qqq;
  TPE_rotationToQuaternion(TPE_vec4(0,100,255,0),50,&qqq);

  TPE_bodySetOrientation(&(bodies[0]),qqq);

  int collided = 0;
  int time;

  while (running)
  {
    time = SDL_GetTicks();

//    TPE_worldApplyGravityCenter(&world,TPE_vec4(0,0,0,0),4);
    TPE_worldApplyGravityDown(&world,4);

    for (uint32_t i = 0; i < PIXELS_SIZE; ++i)
      pixels[i] = 0;

    S3L_newFrame();

    TPE_worldStepBodies(&world);

    updateBodies();

    S3L_drawScene(scene);

    TPE_Vec4 p, n;

    TPE_worldResolveCollisionNaive(&world);

    SDL_UpdateTexture(textureSDL,NULL,pixels,S3L_RESOLUTION_X * sizeof(uint32_t));

    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
        running = 0;
      else if (event.type == SDL_KEYDOWN)
      {
        if (event.key.keysym.scancode == SDL_SCANCODE_Q || event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
          running = 0;
      }
    }

    const uint8_t *state = SDL_GetKeyboardState(NULL);

    S3L_Vec4 camF, camR;
 
#define SHIFT_STEP 50
#define ROT_STEP 5

    S3L_rotationToDirections(scene.camera.transform.rotation,SHIFT_STEP,&camF,&camR,0);

    if (state[SDL_SCANCODE_LSHIFT])
    {
      if (state[SDL_SCANCODE_UP])
        S3L_vec3Add(&scene.camera.transform.translation,camF);
      else if (state[SDL_SCANCODE_DOWN])
        S3L_vec3Sub(&scene.camera.transform.translation,camF);
      else if (state[SDL_SCANCODE_LEFT])
        S3L_vec3Sub(&scene.camera.transform.translation,camR);
      else if (state[SDL_SCANCODE_RIGHT])
        S3L_vec3Add(&scene.camera.transform.translation,camR);
    }
    else
    {
      if (state[SDL_SCANCODE_UP])
        scene.camera.transform.rotation.x += ROT_STEP;
      else if (state[SDL_SCANCODE_DOWN])
        scene.camera.transform.rotation.x -= ROT_STEP;
      else if (state[SDL_SCANCODE_LEFT])
        scene.camera.transform.rotation.y += ROT_STEP;
      else if (state[SDL_SCANCODE_RIGHT])
        scene.camera.transform.rotation.y -= ROT_STEP;
    }

#define SHIFT_STEP 50

    if (state[SDL_SCANCODE_L])
      bodies[1].position.x += SHIFT_STEP;
    else if (state[SDL_SCANCODE_J])
      bodies[1].position.x -= SHIFT_STEP;
    else if (state[SDL_SCANCODE_I])
      bodies[1].position.z += SHIFT_STEP;
    else if (state[SDL_SCANCODE_K])
      bodies[1].position.z -= SHIFT_STEP;
    else if (state[SDL_SCANCODE_N])
      bodies[1].position.y += SHIFT_STEP;
    else if (state[SDL_SCANCODE_M])
      bodies[1].position.y -= SHIFT_STEP;
  
    if (state[SDL_SCANCODE_P])
      scene.camera.transform.translation.y += SHIFT_STEP;
    else if (state[SDL_SCANCODE_O])
      scene.camera.transform.translation.y -= SHIFT_STEP;

#undef SHIFT_STEP

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,textureSDL,NULL,NULL);
    SDL_RenderPresent(renderer);

    time = time + MSPF - SDL_GetTicks();

    if (time > 1)
      usleep(time * 1000);

    frame++;
  }

  return 0;
}

