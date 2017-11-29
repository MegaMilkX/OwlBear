R"(#version 330

in vec3 Position;
in vec3 ColorRGB;
in vec2 UV;
out vec3 fColorRGB;
out vec2 fUV;
void main()
{
    fColorRGB = ColorRGB;
    fUV = UV;
    gl_Position = vec4(Position, 1.0);
}

)"