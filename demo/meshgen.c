#include <math.h>
#include <stdlib.h>

#include "meshgen.h"

float *mesh_gen_sphere(int slices, int stacks, float radius, int* out_vertex_count)
{
    int num_vertices = slices * stacks * 6;
    float* vertices = malloc(sizeof(float) * 3 * num_vertices);
    
    int index = 0;

    for (int i = 0; i < stacks; ++i) 
    {
        float lat0 = M_PI * (-0.5f + (float)i / stacks);
        float lat1 = M_PI * (-0.5f + (float)(i + 1) / stacks);
        float z0 = sinf(lat0);
        float zr0 = cosf(lat0);
        float z1 = sinf(lat1);
        float zr1 = cosf(lat1);

        for (int j = 0; j < slices; ++j) 
        {
            float lng0 = 2 * M_PI * (float)j / slices;
            float lng1 = 2 * M_PI * (float)(j + 1) / slices;

            float x0 = cosf(lng0), y0 = sinf(lng0);
            float x1 = cosf(lng1), y1 = sinf(lng1);

            vertices[index++] = radius * x0 * zr0;
            vertices[index++] = radius * y0 * zr0;
            vertices[index++] = radius * z0;

            vertices[index++] = radius * x0 * zr1;
            vertices[index++] = radius * y0 * zr1;
            vertices[index++] = radius * z1;

            vertices[index++] = radius * x1 * zr1;
            vertices[index++] = radius * y1 * zr1;
            vertices[index++] = radius * z1;

            vertices[index++] = radius * x0 * zr0;
            vertices[index++] = radius * y0 * zr0;
            vertices[index++] = radius * z0;

            vertices[index++] = radius * x1 * zr1;
            vertices[index++] = radius * y1 * zr1;
            vertices[index++] = radius * z1;

            vertices[index++] = radius * x1 * zr0;
            vertices[index++] = radius * y1 * zr0;
            vertices[index++] = radius * z0;
        }
    }

    if (out_vertex_count) *out_vertex_count = num_vertices;
    return vertices;
}