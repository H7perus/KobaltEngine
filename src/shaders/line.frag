#version 460 core
#extension GL_ARB_bindless_texture : require


in vec3 FragPos;
in vec3 Color;
                   
out vec4 out_Color;                                                                                                                   



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
    out_Color = vec4(Color, 1);
} 


