#include <GLFW/glfw3.h>
#include <cglm/types.h>
#include <cglm/cglm.h>
#include <cglm/vec3.h>
#include <math.h>

#include "bullet_api.h"
#include "bullet_types.h"
#include "camera.h"
#include "m_player.h"

BulletBody *body;
vec3 player_pos;
vec3 player_vel;
BulletVec3_t bt_player_vel;
BulletVec3_t bt_player_pos;
float accel = 50.0f;
float max_speed = 10.0f;

float player_vec3_len(vec3 *v)
{
    return sqrt(*v[0] * *v[0] * *v[1] * *v[1] * *v[2] * *v[2]);
}

void player_init(vec3 pos, BulletBody *b)
{
    body = b;
}

void player_update(GLFWwindow* win, float dt)
{
    vec3 input_x, input_y, target_vel, input;
    vec3 tmp;

    vec3 *camFront = (vec3 *)get_camera_front();
    vec3 *camUp = (vec3 *)get_camera_up();

    vec3 up = {0.0f, 1.0f, 0.0f};
    vec3 front;

    glm_vec3_cross(*camFront, up, tmp);
    glm_vec3_normalize(tmp);
    glm_vec3_cross(up, tmp, front);
    glm_vec3_normalize(front);

    vec3 right;
    glm_vec3_cross(front, up, right);
    glm_vec3_normalize(right);

    float curr_vel = player_vec3_len(&player_vel);
    float target_speed = (max_speed * accel) - curr_vel;
    float add_speed = (target_speed - curr_vel) * dt;

    if (target_speed < 0.0f) target_speed = 0.0f;
    if (add_speed < 0.0f) add_speed = 0.0f;

    float vel = curr_vel + add_speed;
    float inputX, inputY;

    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) 
    {
        inputY = 1.0f;
    }
    else if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) 
    {
        inputY = -1.0f;
    }
    else
    {
        inputY = 0.0f;
    } 

    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) 
    {
        inputX = -1.0f;
    }
    else if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) 
    {
        inputX = 1.0f;
        
    }
    else
    {
        inputX = 0.0f;
    }

    glm_vec3_scale(front, inputY, input_y);
    glm_vec3_scale(right, inputX, input_x);
    glm_vec3_add(input_x, input_y, input);
    glm_vec3_normalize(input);

    glm_vec3_scale(input, vel, player_vel);

    Get_LinearVelocity(body, &bt_player_vel);
    bt_player_vel.x = player_vel[0];
    bt_player_vel.z = player_vel[2];

    Bullet_Activate(body, 1);
    Bullet_SetLinearVelocity(body, &bt_player_vel);
    Bullet_GetBodyPosition(body, &bt_player_pos);

    player_pos[0] = bt_player_pos.x;
    player_pos[1] = bt_player_pos.y + 0.8f;
    player_pos[2] = bt_player_pos.z;
    set_camera_position(player_pos);
}