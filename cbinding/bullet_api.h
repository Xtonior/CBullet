#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "bullet_types.h"

// Structs
typedef struct BulletBody BulletBody;
typedef struct BulletContactResultCallback BulletContactResultCallback;
typedef struct BulletShape BulletShape;
typedef struct BulletWorld BulletWorld;

// --- AUTO-GENERATED ---

BulletWorld* Bullet_CreateWorld();
void Bullet_SetGravity(BulletWorld *world, float x, float y, float z);
void Bullet_DestroyWorld(BulletWorld* w);
void Bullet_WorldContactTest(BulletWorld* w, BulletBody *b, BulletContactResultCallback **outContactResultCallback);
void Bullet_GetContactPoint(BulletContactResultCallback *contactResultCallback, BulletContactPoint_t *outBulletContactPoint, int numContact);
int Bullet_BodyGetCountContacts(BulletContactResultCallback *contactResultCallback, int maxPoints);
void Bullet_DestroyContactResultCallback(BulletContactResultCallback* callback);
BulletBody* Bullet_CreateRigidBody(BulletWorld *w, BulletShape *shape, float mass, float x, float y, float z);
void Bullet_DestroyRigidBody(BulletWorld *w, BulletBody *b);
BulletShape* Bullet_CreateSphereShape(float radius);
BulletShape* Bullet_CreateBoxShape(float halfExtentX, float halfExtentY, float halfExtentZ);
BulletShape* Bullet_CreateCapsuleShape(float radius, float height);
void Bullet_DestroyShape(BulletShape *shape);
void Bullet_ApplyCentralImpulse(BulletBody *b, BulletVec3_t v);
void Bullet_ApplyImpulse(BulletBody *b, BulletVec3_t v, BulletVec3_t p);
void Bullet_ApplyCentralPushImpulse(BulletBody *b, BulletVec3_t v, BulletVec3_t p);
void Bullet_ApplyPushImpulse(BulletBody *b, BulletVec3_t v, BulletVec3_t p);
void Bullet_ApplyCentralForce(BulletBody *b, BulletVec3_t v);
void Bullet_ApplyForce(BulletBody *b, BulletVec3_t v, BulletVec3_t p);
void Bullet_ApplyTorque(BulletBody *b, BulletVec3_t v);
void Bullet_ApplyTorqueImpulse(BulletBody *b, BulletVec3_t v);
void Bullet_ApplyTorqueTurnImpulse(BulletBody *b, BulletVec3_t v);
void Bullet_StepSimulation(BulletWorld *w, float timeStep);
void Bullet_Activate(BulletBody* b, int forceActivation);
void Bullet_GetBodyPosition(BulletBody *b, BulletVec3_t *out);
void Bullet_GetBodyEulerZYX(BulletBody *b, BulletVec3_t *out);
void Bullet_GetBodyRotation(BulletBody *b, BulletQuat_t *out);
void Get_TotalTorque(BulletBody *b, BulletVec3_t *out);
void Get_TotalForce(BulletBody *b, BulletVec3_t *out);
float Get_AngularDamping(BulletBody *b);
void Get_AngularFactor(BulletBody *b, BulletVec3_t *out);
float Get_AngularSleepingThreshold(BulletBody *b);
void Get_AngularVelocity(BulletBody *b, BulletVec3_t *out);
float Get_LinearDamping(BulletBody *b);
void Get_LinearFactor(BulletBody *b, BulletVec3_t *out);
float Get_LinearSleepingThreshold(BulletBody *b);
void Get_LinearVelocity(BulletBody *b, BulletVec3_t *out);
void Get_InterpolationLinearVelocity(BulletBody *b, BulletVec3_t *out);
void Get_InterpolationAngularVelocity(BulletBody *b, BulletVec3_t *out);
void Get_PushVelocity(BulletBody *b, BulletVec3_t *out);
void Get_PushVelocityInLocalPoint(BulletBody *b, BulletVec3_t p, BulletVec3_t *out);
void Get_TurnVelocity(BulletBody *b, BulletVec3_t *out);
float Get_DeactivationTime(BulletBody *b);
float Get_Restitution(BulletBody *b);
float Get_Friction(BulletBody *b);
void Get_AnisotropicFriction(BulletBody *b, BulletVec3_t *out);
float Get_RollingFriction(BulletBody *b);
float Get_SpinningFriction(BulletBody *b);
float Get_Mass(BulletBody *b);
void Get_LocalInertia(BulletBody *b, BulletVec3_t *out);
void Bullet_SetPosition(BulletBody *b, float x, float y, float z);
void Bullet_SetRotation(BulletBody *b, const BulletQuat_t *rot);
void Bullet_SetLinearVelocity(BulletBody *b, const BulletVec3_t *v);
void Bullet_SetAngularVelocity(BulletBody *b, const BulletVec3_t *v);
void Set_AngularFactor(BulletBody *b, BulletVec3_t v);
void Set_AngularVelocity(BulletBody *b, BulletVec3_t v);
void Set_LinearFactor(BulletBody *b, BulletVec3_t v);
void Set_LinearVelocity(BulletBody *b, BulletVec3_t v);
void Set_PushVelocity(BulletBody *b, BulletVec3_t v);
void Set_TurnVelocity(BulletBody *b, BulletVec3_t v);
void Set_Damping(BulletBody *b, float lin_damping, float ang_damping);
void Set_Gravity(BulletBody *b, BulletVec3_t v);
void Set_SleepingThresholds(BulletBody *b, float linear, float angular);
void Set_InterpolationAngularVelocity(BulletBody *b, BulletVec3_t v);
void Set_InterpolationLinearVelocity(BulletBody *b, BulletVec3_t v);
void Set_Friction(BulletBody *b, float frict);
void Set_AnisotropicFriction(BulletBody *b, BulletVec3_t v);
void Set_RollingFriction(BulletBody *b, float frict);
void Set_SpinningFriction(BulletBody *b, float frict);
void Set_DeactivationTime(BulletBody *b, float time);

// --- END ---

#ifdef __cplusplus
}
#endif
