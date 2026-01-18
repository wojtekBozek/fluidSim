#version 430 core
in vec3 uv;
out vec4 fragColor;
uniform usampler3D tex;
void main() {
    uint color = texture(tex, uv).r;
    if(color == 0u)
        fragColor = vec4(0.0,0.0,1.0,1.0);
    else if(color == 1u)
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    else if(color == 2u)
        fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}