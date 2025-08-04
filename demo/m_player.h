#pragma once

#include <GLFW/glfw3.h>
#include <cglm/types.h>

#include "bullet_api.h"

float player_vec3_len(vec3 *v);
void player_init(vec3 pos, BulletBody *b);
void player_update(GLFWwindow* win, float dt);