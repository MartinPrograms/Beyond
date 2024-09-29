#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outUV;

struct Vertex {

    vec3 position;
    float uv_x;
    vec3 normal;
    float uv_y;
    vec4 color;
};

layout(buffer_reference, std430) readonly buffer VertexBuffer{
    Vertex vertices[];
};

//push constants block
layout( push_constant ) uniform constants
{
    mat4 model_matrix;
    mat4 view_matrix;
    mat4 projection_matrix;
    VertexBuffer vertexBuffer;
} PushConstants;

void main()
{
    Vertex vertex = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

    gl_Position = PushConstants.projection_matrix * PushConstants.view_matrix * PushConstants.model_matrix * vec4(vertex.position, 1.0);
    outColor = vertex.color.rgb;
    outUV = vec2(vertex.uv_x, vertex.uv_y);
}
