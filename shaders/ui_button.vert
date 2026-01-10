#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 fragUV;

layout(push_constant) uniform PushConstants {
    mat4 projection;
} pc;

void main() {
    gl_Position = pc.projection * vec4(inPosition, 0.0, 1.0);
    fragUV = inUV;
}
