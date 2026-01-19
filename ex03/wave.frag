//  Wave Shader
#version 120

const float pi4 = 4.0*3.145927;

void main()
{
   //  Generate spatial coordinate from texture coordinates
   float r = length(pi4*(gl_TexCoord[0].xy-0.5));
   //  Turn r into wavey color
   vec3 color  = vec3(cos(r)*cos(r),sin(r)*sin(r),abs(cos(2.0*r)*sin(2.0*r)));
   //  Modulate for lighting
   color *= gl_Color.rgb;
   //  Pad color with alpha
   gl_FragColor = vec4(color,1.0);
}
