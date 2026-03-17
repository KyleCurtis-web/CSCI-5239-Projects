//  Set the fragment color
#version 120

uniform sampler2D tex;
uniform sampler2D wally;

varying float instanceIdentifier;

void main()
{
	const float epsilon = 0.1;
	if(abs(instanceIdentifier - 5) < epsilon)
	{
		gl_FragColor = gl_Color * texture2D(wally,gl_TexCoord[0].xy);
	}
	else
	{
	    gl_FragColor = gl_Color * texture2D(tex,gl_TexCoord[0].xy);
	}
}
