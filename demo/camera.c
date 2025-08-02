#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

float camYaw = -90.0f, camPitch = 0.0f;
vec3 camPos = {0.0f, 5.0f, 20.0f};
vec3 camFront = {0.0f, 0.0f, -1.0f};
vec3 camUp = {0.0f, 1.0f, 0.0f};

float lastX = 800/2.0f, lastY = 600/2.0f;
int firstMouse = 1;

float *get_camera_front()
{
    return camFront;
}

float *get_camera_pos()
{
    return camPos;
}

void camera_update_mouse(float xpos, float ypos)
{
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

void camera_update_movement(GLFWwindow* win, float delta)
{
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

void camera_draw(GLuint shader)
{
    glUseProgram(shader);

    mat4 view, proj;
    glm_lookat(camPos, (vec3){camPos[0]+camFront[0], camPos[1]+camFront[1], camPos[2]+camFront[2]}, camUp, view);
    glm_perspective(glm_rad(70.0f), 800.0f/600.0f, 0.1f, 100.0f, proj);

    GLint loc_view = glGetUniformLocation(shader, "view");
    GLint loc_proj = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(loc_view, 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(loc_proj, 1, GL_FALSE, (float*)proj);
}