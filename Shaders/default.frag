#version 450

//shader input
layout (location = 0) in vec2 uv;
layout (location = 1) in vec3 normal;

//output write
layout (location = 0) out vec4 outFragColor;

void main()
{
    // Dra wnormal as color
    outFragColor = vec4(normal * 0.5 + 0.5, 1);
}
