#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BulletWorld BulletWorld;
typedef struct BulletBody  BulletBody;
typedef struct BulletShape BulletShape;

// --- AUTO-GENERATED ---

BulletWorld* Bullet_CreateWorld();
void Bullet_SetGravity(BulletWorld *world, float x, float y, float z);
void Bullet_DestroyWorld(BulletWorld* w);
BulletBody* Bullet_CreateRigidBody(BulletWorld *w, BulletShape *shape, float mass, float x, float y, float z);
void Bullet_DestroyRigidBody(BulletWorld *w, BulletBody *b);
BulletShape* Bullet_CreateSphereShape(float radius);
BulletShape* Bullet_CreateBoxShape(float halfExtentX, float halfExtentY, float halfExtentZ);
void Bullet_DestroyShape(BulletShape *shape);
void Bullet_ApplyCentralImpulse(BulletBody *b, float impulseX, float impulseY, float impulseZ);
void Bullet_StepSimulation(BulletWorld *w, float timeStep);
void Bullet_GetBodyPosition(BulletBody *b, float *outX, float *outY, float *outZ);

// --- END ---

#ifdef __cplusplus
}
#endif
