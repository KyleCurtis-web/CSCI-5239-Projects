//
//  nBody geometry shader
//  Billboards point to quad
//

#version 330
layout(points) in;
layout(triangle_strip,max_vertices=4) out;
uniform float size;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

in vec4 vColor[1];
out vec2 Tex2D;
out vec4 fColor;

void billboard(float x,float y,int s,int t)
{
   //  Copy Color
   fColor  = vColor[0];
   //  Set texture coordinates
   Tex2D = vec2(s,t);
   //  Determine position
   vec4 p = gl_in[0].gl_Position;
   p.xyz += transpose(mat3(ModelViewMatrix))*vec3(x,y,0);
   gl_Position = ProjectionMatrix*ModelViewMatrix*p;
   //  Emit new vertex
   EmitVertex();
}

void main()
{
   billboard(-size,-size , 0,0);
   billboard(+size,-size , 1,0);
   billboard(-size,+size , 0,1);
   billboard(+size,+size , 1,1);
   EndPrimitive();
}
