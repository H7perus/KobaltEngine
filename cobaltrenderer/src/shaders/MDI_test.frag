#version 460 core
#extension GL_ARB_bindless_texture : require

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

struct PBRMat
{
    vec4 tint;
    vec4 emission_color;
    sampler2D albedo;
    sampler2D emission;

    sampler2D normal;
    sampler2D metallicity;
};


layout (std140, binding = 0) uniform Commons {
    vec4 uPos;
    mat4 transform;
    float gamma;
};

layout(std430, binding = 1) buffer Commands {
    command commands[];
};

layout(std430, binding = 2) buffer Materials {
    PBRMat materials[];
};

layout(std430, binding = 3) buffer ModelMats {
    mat4 modelmatrices[];
};


in vec2 Texcoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 FragPos;
flat in uint drawID;                                                           
out vec4 out_Color;                                                                                                                   
uniform sampler2D lightmap;    
uniform bool debugbool;
uniform float normalmix;


vec4 sRGBToLinear(vec4 color)
{
    return pow(color, vec4(2.2));
}


vec3 KhrPBRneutral(vec3 inputColor)
{
    float x = min(inputColor.x, min(inputColor.y, inputColor.z));

    float F90 = 0.04;
    float Ks = 0.8 - F90;
    float Kd = 0.15;
    float f = x - x * x / (4 * F90);
    if(x > 2 * F90)
    {
            f = F90;
    }

    float p = max(inputColor.r - f, max(inputColor.g - f, inputColor.b - f) );

    float pn = 1 - ( pow(1 - Ks, 2) / (p + 1 - 2 * Ks) );

    float g = 1 / (Kd * (p - pn) + 1);

    vec3 outColor = inputColor - f;

    if(p > Ks)
    {
        outColor = outColor  * (pn / p) * g  + vec3(pn) * (1 - g);
    }


    return outColor;

}

void main()                                                                   
{   
    out_Color.rgb = Texcoord.xyx;
    //return;


    uint local_objectID = commands[drawID].materialID;

    //out_Color = vec4(0.8, 0.0, 0.0, 1.0); //gl_FragCoord.xyxy / 500.f;

    out_Color = vec4(Normal + 1, 2.0) / 2;
    //return;

    PBRMat mat = materials[local_objectID];

    sampler2D cat = mat.albedo;

    if(mat.emission_color.x > 0 || mat.emission_color.y > 0 || mat.emission_color.z > 0)
    {
      out_Color = mat.emission_color; 
      return;
    }




    vec2 TriPlanar = vec2(0.5);

    if( abs(Normal.z) > abs(Normal.x) && abs(Normal.z) > abs(Normal.y))
    {
        TriPlanar.xy = FragPos.xy;
    }
    else if(abs(Normal.y) > abs(Normal.z))
    {
        TriPlanar.xy =  FragPos.xz;
    }
    else
    {
        TriPlanar.xy = FragPos.yz;
    }

    vec3 normalMap = (texture(mat.normal, Texcoord).rgb - 0.5) * 2;


    //out_Color.rgb = normalMap;
    //return;


    vec3 finNormal = Normal; //normalize(Tangent * normalMap.x  + Bitangent * normalMap.y + Normal * normalMap.z);

    //vec4 texColor = texture(cat, TriPlanar.xy);
    //texColor = textureLod(lightmap, vec2(Texcoord.x, 1 - Texcoord.y), 0);
    //out_Color = textureLod(lightmap, vec2(Texcoord.x, 1 - Texcoord.y), 0) * sRGBToLinear(texColor * mat.tint) * 1;

    out_Color = texture(cat, Texcoord); // * texture(lightmap, Texcoord);

    //out_Color.rgb = vec3(Texcoord.xx, 0);
    //return;
    if(!debugbool)
        finNormal = Normal;

    out_Color = mix(out_Color * 0.5, vec4(finNormal + 1, 2.0) / 2, normalmix);
    out_Color.rgb = KhrPBRneutral(vec3(out_Color));
    //if(texture(mat.metallicity, vec2(0)).g > 0)
    //    out_Color = texture(mat.metallicity, vec2(0)).gggg;

    out_Color = pow(out_Color, vec4(1 / 2.2));
} 


