//  Set the fragment color
#version 120

uniform sampler2D tex;
uniform float time;
uniform float hologramSpec;

void main()
{
   float saveFragY = sin(gl_FragCoord.y + time);

   if(saveFragY > 0)
	discard;
   else
	gl_FragColor = gl_Color * texture2D(tex,gl_TexCoord[0].xy);
}
