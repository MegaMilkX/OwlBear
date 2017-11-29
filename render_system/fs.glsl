R"(#version 330

uniform sampler2D Diffuse;
in vec3 fColorRGB;
in vec2 fUV;
out vec4 color;
void main()
{
    color = vec4(texture2D(Diffuse, fUV).xyz, 1.0);
}

)"