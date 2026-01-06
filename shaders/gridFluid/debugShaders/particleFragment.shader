// fragment.glsl
#version 430 core
out vec4 FragColor;
uniform vec3 color;
void main() {
    
    // gl_PointCoord gives the fragment’s coordinate in [0,1] range
    vec2 uv = gl_PointCoord * 2.0 - 1.0; // center at (0,0)
    float dist = length(uv); // distance from center
    //
    //// Compute soft alpha falloff
    ////float alpha = smoothstep(1.0, 1.0 - softness, dist);
    //
    //// Kill fragments outside circle for clean edges
    if (dist > 1.0)
        discard;
//
    //// Final color
    if(dist+0.01 < 1.0)
    {
        FragColor = vec4(color, 1.0);
    }
    else
        FragColor = vec4(0.0,0.0,0.0,1.0);
}