#version 460 core      

layout(location = 0)  in vec3 inPosition;
layout(location = 1)  in vec3 normal;
layout(location = 3)  in vec3 tangent;
layout(location = 2)  in vec2 UVMap;


in vec3 in_Position;                                                         
in vec2 in_Texcoord;                                                         
out vec2 Texcoord;
out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;
out vec3 FragPos;
flat out uint drawID;
uniform mat4 trans;

struct command{
    uint test;
    uint test1;
    uint test2;
    int  test3;
    uint test4;
    uint materialID;
    uint modelmatrixID;
    //uint MARGIN;
};

layout (std140, binding = 0) uniform Commons {
    vec4 uPos;
    mat4 transform;
    float gamma;
};

layout(std430, binding = 1) buffer Commands {
    command commands[];
};

layout(std430, binding = 3) buffer ModelMats {
    mat4 modelmatrices[];
};

void main()                                                                   
{      
    drawID = gl_DrawID;

    mat4 modelmat = modelmatrices[ commands[drawID].modelmatrixID ];

//    if(commands[drawID].modelmatrixID % 2 == 0)
//    {
//        modelmat = mat4(1.0);
//    }

    Normal = normalize(transpose(inverse(mat3(modelmat))) * normal);

    Tangent = normalize(transpose(inverse(mat3(modelmat))) * tangent);
    Bitangent = cross(Normal, Tangent);


    Texcoord = vec2(UVMap.x, UVMap.y); 
    //Texcoord.y = 1 - Texcoord.y;
    FragPos =  mat3(modelmat) * in_Position.xyz;
    gl_Position = transform * (modelmat * vec4(in_Position.xyz, 1.0));                                
                                
}                                                                               