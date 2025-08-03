#include <cglm/affine-mat.h>
#include <cglm/euler.h>
#include <cglm/quat.h>
#include <sys/types.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <cglm/vec3.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "bullet_api.h"
#include "bullet_types.h"

#include "camera.h"
#include "shaders.h"
#include "drawable.h"
#include "meshes.h"
#include "meshgen.h"

#define MAX_BODIES 128

GLFWwindow* win;

Drawable objects[MAX_BODIES];
Drawable ground_object;
Drawable sphere_object;
int num_sphere_verts;
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

// https://github.com/glfw/glfw/issues/310
void glfwSetWindowCenter(GLFWwindow* window) 
{
    // Get window position and size
    int window_x, window_y;
    glfwGetWindowPos(window, &window_x, &window_y);

    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

    // Halve the window size and use it to adjust the window position to the center of the window
    window_width *= 0.5;
    window_height *= 0.5;

    window_x += window_width;
    window_y += window_height;

    // Get the list of monitors
    int monitors_length;
    GLFWmonitor **monitors = glfwGetMonitors(&monitors_length);

    if(monitors == NULL) 
    {
        // Got no monitors back
        return;
    }

    // Figure out which monitor the window is in
    GLFWmonitor *owner = NULL;
    int owner_x, owner_y, owner_width, owner_height;

    for(int i = 0; i < monitors_length; i++) 
    {
        // Get the monitor position
        int monitor_x, monitor_y;
        glfwGetMonitorPos(monitors[i], &monitor_x, &monitor_y);

        // Get the monitor size from its video mode
        int monitor_width, monitor_height;
        GLFWvidmode *monitor_vidmode = (GLFWvidmode*) glfwGetVideoMode(monitors[i]);

        if(monitor_vidmode == NULL) 
        {
            // Video mode is required for width and height, so skip this monitor
            continue;

        } else 
        {
            monitor_width = monitor_vidmode->width;
            monitor_height = monitor_vidmode->height;
        }

        // Set the owner to this monitor if the center of the window is within its bounding box
        if((window_x > monitor_x && window_x < (monitor_x + monitor_width)) && (window_y > monitor_y && window_y < (monitor_y + monitor_height)))
        {
            owner = monitors[i];

            owner_x = monitor_x;
            owner_y = monitor_y;

            owner_width = monitor_width;
            owner_height = monitor_height;
        }
    }

    if(owner != NULL) 
    {
        // Set the window position to the center of the owner monitor
        glfwSetWindowPos(window, owner_x + (owner_width * 0.5) - window_width, owner_y + (owner_height * 0.5) - window_height);
    }
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
    BulletShape* groundShape = Bullet_CreateBoxShape(20, 0.5f, 20);
    BulletBody* ground = Bullet_CreateRigidBody(world, groundShape, 0, 0, -1, 0);

    ground_object.body = ground;
    for (int j = 0; j < 3; ++j)
        ground_object.color[j] = (float)(rand() % 100) / 100.0f;

    float *sphere_verts = mesh_gen_sphere(16, 16, 0.5f, &num_sphere_verts);

    drawable_init(&sphere_object, shader, sphere_verts, num_sphere_verts * 3);
    BulletShape* sphereShape = Bullet_CreateSphereShape(0.5f);
    BulletBody* sphere = Bullet_CreateRigidBody(world, sphereShape, 1, 0, 10, 0);

    sphere_object.body = sphere;
    for (int j = 0; j < 3; ++j)
        sphere_object.color[j] = (float)(rand() % 100) / 100.0f;

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

    objects[objectCount++] = sphere_object;

    return 1;
}

void update_bodies()
{
    for (int i = 0; i < objectCount; ++i) 
    {
        if (glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS)
        {
            Bullet_Activate(objects[i].body, 0);
            BulletVec3_t impulse = {0.0f, 1.0f, 0.0f};
            Bullet_ApplyCentralImpulse(objects[i].body, impulse);
        }

        BulletVec3_t pos;
        Bullet_GetBodyPosition(objects[i].body, &pos);

        BulletQuat_t rot;
        Bullet_GetBodyRotation(objects[i].body, &rot);
        
        versor q;
        glm_quat_identity(q);
        glm_quat_init(q, rot.x, rot.y, rot.z, rot.w);
        
        mat4 rotation;
        glm_quat_mat4(q, rotation);
        
        mat4 translation;
        glm_translate_make(translation, (vec3){pos.x, pos.y, pos.z});

        mat4 model;
        glm_mat4_mul(translation, rotation, model);

        drawable_update_transform(&objects[i], model);
        drawable_draw(&objects[i], shader, num_cube_vertices);
    }
}

void update_ground()
{
    BulletVec3_t pos;
    Bullet_GetBodyPosition(ground_object.body, &pos);

    BulletQuat_t rot;
    Bullet_GetBodyRotation(ground_object.body, &rot);
    
    versor q;
    glm_quat_identity(q);
    glm_quat_init(q, rot.x, rot.y, rot.z, rot.w);
    
    mat4 rotation;
    glm_quat_mat4(q, rotation);
    
    mat4 translation;
    glm_translate_make(translation, (vec3){pos.x, pos.y + 0.5f, pos.z});

    mat4 model;
    glm_mat4_mul(translation, rotation, model);

    drawable_update_transform(&ground_object, model);
    drawable_draw(&ground_object, shader, num_ground_vertices);
}

void update_sphere()
{
    BulletVec3_t pos;
    Bullet_GetBodyPosition(sphere_object.body, &pos);

    BulletQuat_t rot;
    Bullet_GetBodyRotation(sphere_object.body, &rot);
    
    versor q;
    glm_quat_identity(q);
    glm_quat_init(q, rot.x, rot.y, rot.z, rot.w);
    
    mat4 rotation;
    glm_quat_mat4(q, rotation);
    
    mat4 translation;
    glm_translate_make(translation, (vec3){pos.x, pos.y, pos.z});

    mat4 model;
    glm_mat4_mul(translation, rotation, model);

    drawable_update_transform(&sphere_object, model);
    drawable_draw(&sphere_object, shader, num_sphere_verts);
}

void throw_sphere()
{
    Bullet_Activate(sphere_object.body, 0);

    Bullet_SetPosition(sphere_object.body, get_camera_pos()[0], get_camera_pos()[1], get_camera_pos()[2]);

    BulletVec3_t zero_vec = {0.0f, 0.0f, 0.0f};
    Bullet_SetLinearVelocity(sphere_object.body, &zero_vec);
    Bullet_SetAngularVelocity(sphere_object.body, &zero_vec);
        
    BulletVec3_t impulse = 
    {
        get_camera_front()[0] * 50.0f, 
        get_camera_front()[1] * 50.0f, 
        get_camera_front()[2] * 50.0f
    };

    Bullet_ApplyCentralImpulse(sphere_object.body, impulse);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        throw_sphere();
    }
}

int main() 
{
    srand(time(NULL));
    
    init_window();
    init_graphics();
    init_physics();

    glfwSetWindowCenter(win);
    glfwSetKeyCallback(win, key_callback);

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
        update_sphere();

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
