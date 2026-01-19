//  Set the fragment color
#version 120

uniform vec3 dim;

void main()
{
   gl_FragColor = vec4(gl_FragCoord.xyz/dim,1.0);
}
