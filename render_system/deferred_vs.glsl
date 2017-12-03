R"(#version 330

in vec3 Position;
in vec2 UV;
out vec2 fUV;
void main()
{
    fUV = UV;
    gl_Position = vec4(Position, 1.0);
}

)"