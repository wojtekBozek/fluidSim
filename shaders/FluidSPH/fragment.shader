// fragment.glsl
#version 430 core
out vec4 FragColor;
uniform vec3 color;
uniform float softness; // how soft the edge is, 0.0–1.0 range

void main() {
    // gl_PointCoord gives the fragment’s coordinate in [0,1] range
    vec2 uv = gl_PointCoord * 2.0 - 1.0; // center at (0,0)
    float dist = length(uv); // distance from center
    
    // Compute soft alpha falloff
    float alpha = smoothstep(1.0, 1.0 - softness, dist);
    
    // Kill fragments outside circle for clean edges
    if (dist > 1.0)
        discard;

    // Final color
    FragColor = vec4(color, alpha);
}