#pragma once

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

void camera_update_mouse(float xpos, float ypos);
void camera_update_movement(GLFWwindow* win, float delta);
void camera_draw(GLuint shader);