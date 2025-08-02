#pragma once

#include "bullet_api.h"
#include <GL/gl.h>
#include <cglm/types.h>

typedef struct Drawable
{
    GLuint vbo, vao;
    mat4 transform;
    BulletBody* body;
    float color[3];
} Drawable;

void drawable_init(Drawable *drawable, GLuint shader, float vertices[], int num_vertices);
void drawable_draw(Drawable *drawable, GLuint shader, int num_vertices);
void drawable_update_transform(Drawable *drawable, mat4 new_transform);