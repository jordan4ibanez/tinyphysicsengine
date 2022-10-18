#include <stdio.h>
#include "tinyphysicsengine.h"

#define F TPE_FRACTIONS_PER_UNIT
  
#define TOLERANCE 10

int tolerance(TPE_Unit x, TPE_Unit expX)
{
  return (x <= (expX + TOLERANCE)) && (x >= (expX - TOLERANCE)); 
}

int testRotToQuat(
  TPE_Unit x, TPE_Unit y, TPE_Unit z, TPE_Unit angle,
  TPE_Unit expX, TPE_Unit expY, TPE_Unit expZ, TPE_Unit expW)
{
  printf("testing axis + rot -> quaternion ([%d,%d,%d] %d -> %d %d %d %d): ",
    x,y,z,angle,expX,expY,expZ,expW);

  TPE_Vec4 q, axis;

  TPE_vec4Set(&axis,x,y,z,0);
  TPE_rotationToQuaternion(axis,angle,&q);

  if (!tolerance(q.x,expX) ||
      !tolerance(q.y,expY) ||
      !tolerance(q.z,expZ) || 
      !tolerance(q.w,expW)) 
  {
    printf("bad (%d %d %d %d)\n",q.x,q.y,q.z,q.w);
    return 0;
  }
  
  puts("OK");
  return 1;
}

int ass(const char *what, int cond)
{
  printf("testing %s: %s\n",what,cond ? "OK" : "ERROR");
  return cond;
}

int testColl(const TPE_Body *b1, const TPE_Body *b2,
  TPE_Unit expRet, TPE_Unit expX, TPE_Unit expY, TPE_Unit expZ,
  TPE_Unit expNX, TPE_Unit expNY, TPE_Unit expNZ)
{
  printf("testing collision detection, %d %d: ",b1->shape,b2->shape);

  TPE_Vec4 p, n;

  TPE_vec4Set(&p,0,0,0,0);
  TPE_vec4Set(&n,0,0,0,0);

  TPE_Unit ret = TPE_bodyCollides(b1,b2,&p,&n);

#if 1
  printf("\nret: %d\n",ret);
  TPE_PRINTF_VEC4(p);
  TPE_PRINTF_VEC4(n);
  printf("\n");
#endif

  if (!tolerance(ret,expRet) ||
      ((ret != 0) &&
       (!tolerance(p.x,expX) ||
        !tolerance(p.y,expY) ||
        !tolerance(p.z,expZ) ||
        !tolerance(n.x,expNX) ||
        !tolerance(n.y,expNY) ||
        !tolerance(n.z,expNZ))))
    return 0;

  puts("OK");

  return 1;
}

int main(void)
{
  #define ASS(what) if (!what) { puts("ERROR"); return 0; } 

  {
    ASS(ass("shape ID",TPE_COLLISION_TYPE(TPE_SHAPE_SPHERE,TPE_SHAPE_CUBOID) == TPE_COLLISION_TYPE(TPE_SHAPE_CUBOID,TPE_SHAPE_SPHERE)))

    ASS(testRotToQuat(F,0,0,    0,    0,0,0,F));
    ASS(testRotToQuat(F,0,0,    F/4,  361,0,0,361));
    ASS(testRotToQuat(0,F,0,    F/4,  0,361,0,361));
    ASS(testRotToQuat(0,0,F,    F/2,  0,0,F,0));
    ASS(testRotToQuat(-F,F,F,   -F/8, 112,-112,-112,472));


    TPE_Vec4 p = TPE_vec4(10,200,100,0), p2, q;
  

    p2 = p;
    TPE_rotationToQuaternion(TPE_vec4(512,0,0,0),F/4,&q);
    TPE_rotatePoint(&p2,q);

    #define TEST_LINE_SEGMENT_CLOSE(ax,ay,az,bx,by,bz,px,py,pz,rx,ry,rz) \
      ASS(ass("line segment closest",TPE_vec3Dist(TPE_lineSegmentClosestPoint(\
        TPE_vec4(ax,ay,az,0),TPE_vec4(bx,by,bz,0),TPE_vec4(px,py,pz,0)),\
        TPE_vec4(rx,ry,rz,0)) < 10))

    TEST_LINE_SEGMENT_CLOSE(0,0,0, 100,0,0, 50,0,0, 50,0,0)
    TEST_LINE_SEGMENT_CLOSE(-100,-100,20, 1000,10000,20000, -3000,-5000,-1000, -100,-100,20)

return 0;
  }

  {
    puts("collisions:");

    TPE_Body sphere, cylinder;
    TPE_Vec4 collPoint, collNorm;
  
    TPE_bodyInit(&sphere);
    TPE_bodyInit(&cylinder);

    // sphere, sphere:

    sphere.shape = TPE_SHAPE_SPHERE;
    cylinder.shape = TPE_SHAPE_SPHERE;
    
    sphere.shapeParams[0] = TPE_FRACTIONS_PER_UNIT;
    cylinder.shapeParams[1] = TPE_FRACTIONS_PER_UNIT;

    sphere.position = TPE_vec4(F,F / 2,0,0);
    cylinder.position = TPE_vec4(F + F / 2,F / 2,0,0);
    
    ASS(testColl(&sphere,&cylinder,256,640,256,0,512,0,0));
    ASS(testColl(&cylinder,&sphere,256,640,256,0,-512,0,0));

    // sphere, cylinder:

    sphere.shape = TPE_SHAPE_SPHERE;
    cylinder.shape = TPE_SHAPE_CYLINDER;
    
    sphere.shapeParams[0] = F;
    cylinder.shapeParams[0] = F * 2;
    cylinder.shapeParams[1] = F * 3;

    sphere.position.y = 6 * F;
    ASS(testColl(&sphere,&cylinder,0,0,0,0,0,0,0)); // no collision

    sphere.position = TPE_vec4(F * 3 + F / 2,F,0,0);
    cylinder.position = TPE_vec4(F,0,0,0);
    ASS(testColl(&sphere,&cylinder,F / 2,3 * F,F,0,-1 * F,0,0)); // collision with cyl. body
    ASS(testColl(&cylinder,&sphere,F / 2,3 * F,F,0,F,0,0));

    sphere.position.x = F + F / 2;
    sphere.position.y = 2 * F;
    ASS(testColl(&sphere,&cylinder,F / 2,F + F / 2,F + F / 2,0,0,-1 * F,0)); // collision with cyl. body
    ASS(testColl(&cylinder,&sphere,F / 2,F + F / 2,F + F / 2,0,0,F,0));

    sphere.position.x = 3 * F + F / 2;
    sphere.position.y = 2 * F;
    ASS(testColl(&sphere,&cylinder,150,3 * F,F + F / 2,0,-362,-362,0)); // collision with cyl. edge
 


  }

  return 0;
}
