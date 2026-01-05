#version 430 core
in vec2 uv;
out vec4 fragColor;
uniform sampler2D tex;
void main() {
    float color = texture(tex, uv).r;
    fragColor = vec4(color,0.0,-color,1.0);
}