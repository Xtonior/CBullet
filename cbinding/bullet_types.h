#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BulletVec3_t
{
    float x, y, z;
} BulletVec3_t;

typedef struct BulletQuat_t
{
    float x, y, z, w;
} BulletQuat_t;

typedef struct BulletContactPoint_t
{
    BulletVec3_t pointOnA;
    BulletVec3_t pointOnB;
    BulletVec3_t normalOnB;
    float impulse;
    float distance;
} BulletContactPoint_t;

#ifdef __cplusplus
}
#endif