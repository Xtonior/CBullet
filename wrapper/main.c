#include <stdio.h>

#include "bullet_api.h"

int main() 
{
    BulletWorld* w = Bullet_CreateWorld();
    printf("Hello\n");
    Bullet_DestroyWorld(w);
    return 0;
}