#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "bullet_api.h"

#include <cglm/cglm.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <cglm/vec3.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "shaders.h"

#define MAX_BODIES 128

typedef struct {
    BulletBody* body;
    float color[3];
} Drawable;

Drawable objects[MAX_BODIES];
int objectCount = 0;

float camYaw = -90.0f, camPitch = 0.0f;
vec3 camPos = {0.0f, 5.0f, 20.0f};
vec3 camFront = {0.0f, 0.0f, -1.0f};
vec3 camUp = {0.0f, 1.0f, 0.0f};

float lastX = 800/2.0f, lastY = 600/2.0f;
int firstMouse = 1;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = 0; }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camYaw += xoffset;
    camPitch += yoffset;
    if (camPitch > 89.0f) camPitch = 89.0f;
    if (camPitch < -89.0f) camPitch = -89.0f;

    vec3 front;
    front[0] = cosf(glm_rad(camYaw)) * cosf(glm_rad(camPitch));
    front[1] = sinf(glm_rad(camPitch));
    front[2] = sinf(glm_rad(camYaw)) * cosf(glm_rad(camPitch));
    glm_vec3_normalize_to(front, camFront);
}

void process_input(GLFWwindow* win, float delta) {
    float speed = 10.0f * delta;
    vec3 tmp;

    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) {
        glm_vec3_scale(camFront, speed, tmp);
        glm_vec3_add(camPos, tmp, camPos);
    }
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
        glm_vec3_scale(camFront, speed, tmp);
        glm_vec3_sub(camPos, tmp, camPos);
    }
    vec3 right;
    glm_vec3_cross(camFront, camUp, right);
    glm_vec3_normalize(right);

    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
        glm_vec3_scale(right, speed, tmp);
        glm_vec3_sub(camPos, tmp, camPos);
    }
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
        glm_vec3_scale(right, speed, tmp);
        glm_vec3_add(camPos, tmp, camPos);
    }
}

int main() {
    srand(time(NULL));
    if (!glfwInit()) return -1;

    GLFWwindow* win = glfwCreateWindow(800, 600, "Bullet OpenGL C", NULL, NULL);
    glfwMakeContextCurrent(win);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(win, mouse_callback);
    glewInit();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);

    GLuint shader = glCreateProgram();
    glAttachShader(shader, vs);
    glAttachShader(shader, fs);
    glLinkProgram(shader);

    GLuint vao, vbo;
    float cube[] = {
        -1,-1,-1,  1,-1,-1,  1, 1,-1, -1, 1,-1,
        -1,-1, 1,  1,-1, 1,  1, 1, 1, -1, 1, 1
    };
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    BulletWorld* world = Bullet_CreateWorld();
    BulletShape* groundShape = Bullet_CreateBoxShape(20, 1, 20);
    BulletBody* ground = Bullet_CreateRigidBody(world, groundShape, 0, 0, -1, 0);

    for (int i = 0; i < 30; ++i) {
        BulletShape* shape = Bullet_CreateBoxShape(1, 1, 1);
        BulletBody* body = Bullet_CreateRigidBody(world, shape, 1.0f,
            rand()%10 - 5, i*2 + 5, rand()%10 - 5);

        objects[objectCount].body = body;
        for (int j = 0; j < 3; ++j)
            objects[objectCount].color[j] = (float)(rand() % 100) / 100.0f;
        objectCount++;
    }

    glEnable(GL_DEPTH_TEST);

    float last = glfwGetTime();
    while (!glfwWindowShouldClose(win)) {
        float now = glfwGetTime();
        float dt = now - last;
        last = now;

        process_input(win, dt);
        Bullet_StepSimulation(world, dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader);

        mat4 view, proj;
        glm_lookat(camPos, (vec3){camPos[0]+camFront[0], camPos[1]+camFront[1], camPos[2]+camFront[2]}, camUp, view);
        glm_perspective(glm_rad(70.0f), 800.0f/600.0f, 0.1f, 100.0f, proj);

        GLint loc_view = glGetUniformLocation(shader, "view");
        GLint loc_proj = glGetUniformLocation(shader, "projection");
        glUniformMatrix4fv(loc_view, 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(loc_proj, 1, GL_FALSE, (float*)proj);

        for (int i = 0; i < objectCount; ++i) {
            float x, y, z;
            Bullet_GetBodyPosition(objects[i].body, &x, &y, &z);

            mat4 model;
            glm_mat4_identity(model);
            glm_translate(model, (vec3){x, y, z});

            GLint loc_model = glGetUniformLocation(shader, "model");
            glUniformMatrix4fv(loc_model, 1, GL_FALSE, (float*)model);

            GLint loc_color = glGetUniformLocation(shader, "color");
            glUniform3fv(loc_color, 1, objects[i].color);

            glBindVertexArray(vao);
            glDrawArrays(GL_QUADS, 0, 8);
        }

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
