#version 430

layout(local_size_x=16, local_size_y=16) in;
layout(r32f ,binding = 0) uniform image2D vTex;

layout(binding = 1) uniform sampler2D divergence;
layout(binding = 2) uniform usampler2D cellType;

uniform float dt;
uniform float dx;
uniform ivec2 gridSize;
uniform float density = 997.0;

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;


uint typeAt(int i, int j){if(i<0 || i>= gridSize.x || j <0 || j>= gridSize.y) return SOLID; return texelFetch(cellType, ivec2(i, j), 0).r;}

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);

    if(id.x < gridSize.x && id.y < gridSize.y + 1)
    {
        int i = id.x;
        int j = id.y;
        
        float val = imageLoad(vTex,ivec2(i,j)).r;
        if((i%2 == 0 && j%2 == 1) || (i%2 == 1 && j%2 == 0))
        {            
            if(typeAt(i,j)!= FLUID) return;
            int count = 0;
            float div = texelFetch(divergence,ivec2(i,j),0).r;
            //left
            if(typeAt(i,j-1) == SOLID) { imageStore(vTex, ivec2(i,j), vec4(0.0)); return; }  else {count++;}// ścianka dla badanenj prędkości
            // Right
            if(typeAt(i+1,j) != SOLID) { count++; } 
            // Bottom
            if(typeAt(i-1,j) != SOLID) { count++; } 
            // Top
            if(typeAt(i,j+1) != SOLID) { count++; }

            if(count > 0)
            {
                val += div/float(count);                
            }
        }
        else
        {
            if(typeAt(i,j-1)!= FLUID) return;
            float div = texelFetch(divergence,ivec2(i,j-1),0).r;
            int count = 0;
            if(typeAt(i,j) == SOLID){ imageStore(vTex, ivec2(i,j), vec4(0.0)); return; }  else{count++;}// ścianka dla badanenj prędkości
            // Right
            if(typeAt(i+1,j-1) != SOLID){ count++; } 
            // Bottom
            if(typeAt(i-1,j-1) != SOLID){ count++; } 
            // Top
            if(typeAt(i,j-2) != SOLID){ count++; }

            if(count > 0)
            {
                val -= div/float(count);                
            }
        }
        imageStore(vTex, ivec2(i,j), vec4(val));
    }
}