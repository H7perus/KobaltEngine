#version 460 core      

layout(location = 0)  in vec3 inPosition;
layout(location = 1)  in vec3 inColor;

out vec3 FragPos;
out vec3 Color;

layout (std140, binding = 0) uniform Commons {
    vec4 uPos;
    mat4 transform;
    float gamma;
};

layout(std430, binding = 3) buffer ModelMats {
    mat4 modelmatrices[];
};

void main()                                                                   
{      
    Color = inColor;
    gl_Position = transform * vec4(inPosition.xyz, 1.0);
}                                                                               