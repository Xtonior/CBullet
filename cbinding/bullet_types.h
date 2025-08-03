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

#ifdef __cplusplus
}
#endif