#pragma once

const char* vertexShaderSource = "#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main() {\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\n";

const char* fragmentShaderSource = "#version 330 core\n"
"uniform vec3 color;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"   FragColor = vec4(color, 1.0);\n"
"}\n";
