#include <GL/glew.h>
#include <cglm/mat4.h>
#include <cglm/types.h>

#include "drawable.h"

void drawable_init(Drawable *drawable, GLuint shader, float vertices[], int num_vertices)
{
    glGenVertexArrays(1, &drawable->vao);
    glBindVertexArray(drawable->vao);
    glGenBuffers(1, &drawable->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, drawable->vbo);
    glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
}

void drawable_draw(Drawable *drawable, GLuint shader, int num_vertices)
{
    GLint loc_model = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(loc_model, 1, GL_FALSE, (float*)drawable->transform);

    GLint loc_color = glGetUniformLocation(shader, "color");
    glUniform3fv(loc_color, 1, drawable->color);

    glBindVertexArray(drawable->vao);
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);
}

void drawable_update_transform(Drawable *drawable, mat4 new_transform)
{
    glm_mat4_make(*new_transform, drawable->transform);
}