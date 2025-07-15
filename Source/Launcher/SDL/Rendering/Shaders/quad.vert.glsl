#version 460

layout (location = 0) in vec2 a_position;
layout (location = 0) out vec4 v_color;

void main()
{
    gl_Position = vec4(a_position, 0.0f, 1.0f);
    v_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
