#version 450

layout(binding = 0) uniform ColorUniformBlock
{
    vec3 color;
} u_ubo;

layout(location = 0) out vec4 fragColor;
void main() {
    fragColor = vec4(u_ubo.color, 1.0);
}