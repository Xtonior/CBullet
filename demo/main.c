#include <cglm/affine-mat.h>
#include <cglm/euler.h>
#include <cglm/quat.h>
#include <sys/types.h>
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

#include "camera.h"
#include "shaders.h"
#include "drawable.h"
#include "meshes.h"

#define MAX_BODIES 128

GLFWwindow* win;

Drawable objects[MAX_BODIES];
Drawable ground_object;
int objectCount = 0;
GLuint shader;

BulletWorld* world;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
    camera_update_mouse(xpos, ypos);
}

void process_input(GLFWwindow* win, float delta) 
{
    camera_update_movement(win, delta);
}

int init_window()
{
    if (!glfwInit()) return -1;

    win = glfwCreateWindow(800, 600, "Bullet OpenGL C", NULL, NULL);
    glfwMakeContextCurrent(win);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(win, mouse_callback);
    glewInit();

    return 1;
}

int init_graphics()
{
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);

    shader = glCreateProgram();
    glAttachShader(shader, vs);
    glAttachShader(shader, fs);
    glLinkProgram(shader);

    return 1;
}

int init_physics()
{
    world = Bullet_CreateWorld();

    drawable_init(&ground_object, shader, mesh_ground_vertices, num_ground_vertices * 3);
    BulletShape* groundShape = Bullet_CreateBoxShape(20, 1, 20);
    BulletBody* ground = Bullet_CreateRigidBody(world, groundShape, 0, 0, -1, 0);

    ground_object.body = ground;
    for (int j = 0; j < 3; ++j)
        ground_object.color[j] = (float)(rand() % 100) / 100.0f;

    for (int i = 0; i < 120; ++i) 
    {
        drawable_init(&objects[i], shader, mesh_cube_vertices, num_cube_vertices * 3);
        BulletShape* shape = Bullet_CreateBoxShape(0.5f, 0.5f, 0.5f);
        BulletBody* body = Bullet_CreateRigidBody(world, shape, 1.0f,
            rand()%10 - 5, i*2 + 2, rand()%10 - 5);

        objects[i].body = body;
        for (int j = 0; j < 3; ++j)
            objects[i].color[j] = (float)(rand() % 100) / 100.0f;
        
        objectCount = i;
    }

    return 1;
}

void update_bodies()
{
    for (int i = 0; i < objectCount; ++i) 
    {
        if (glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS)
        {
            Bullet_ApplyCentralImpulse(objects[i].body, 0.0f, 1.0f, 0.0f);
        }

        float x, y, z;
        Bullet_GetBodyPosition(objects[i].body, &x, &y, &z);

        float rx, ry, rz, rw;
        Bullet_GetBodyRotation(objects[i].body, &rx, &ry, &rz, &rw);
        
        versor q;
        glm_quat_identity(q);
        glm_quat_init(q, rx, ry, rz, rw);
        
        mat4 rotation;
        glm_quat_mat4(q, rotation);
        
        mat4 translation;
        glm_translate_make(translation, (vec3){x, y, z});

        mat4 model;
        glm_mat4_mul(translation, rotation, model);

        drawable_update_transform(&objects[i], model);
        drawable_draw(&objects[i], shader, num_cube_vertices);
    }
}

void update_ground()
{
    float x, y, z;
    Bullet_GetBodyPosition(ground_object.body, &x, &y, &z);
    
    float rx, ry, rz, rw;
    Bullet_GetBodyRotation(ground_object.body, &rx, &ry, &rz, &rw);
    
    versor q;
    glm_quat_identity(q);
    glm_quat_init(q, rx, ry, rz, rw);
    
    mat4 rotation;
    glm_quat_mat4(q, rotation);
    
    mat4 translation;
    glm_translate_make(translation, (vec3){x, y, z});

    mat4 model;
    glm_mat4_mul(translation, rotation, model);

    drawable_update_transform(&ground_object, model);
    drawable_draw(&ground_object, shader, num_ground_vertices);
}

int main() 
{
    srand(time(NULL));
    
    init_window();
    init_graphics();
    init_physics();

    glEnable(GL_DEPTH_TEST);

    float last = glfwGetTime();
    while (!glfwWindowShouldClose(win)) 
    {
        float now = glfwGetTime();
        float dt = now - last;
        last = now;

        process_input(win, dt);
        Bullet_StepSimulation(world, dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera_draw(shader);

        glUseProgram(shader);
        update_ground();
        update_bodies();

        glfwSwapBuffers(win);
        glfwPollEvents();

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) 
        {
            printf("OpenGL error: %d\n", err);
        }
    }

    glfwTerminate();
    return 0;
}
