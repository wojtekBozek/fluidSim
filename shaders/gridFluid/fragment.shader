#version 430 core
in vec2 uv;
out vec4 fragColor;
uniform usampler2D tex;
void main() {
    uint color = texture(tex, uv).r;
    if(color == 0)
        fragColor = vec4(0.0,0.0,1.0,0.0);
    else
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}