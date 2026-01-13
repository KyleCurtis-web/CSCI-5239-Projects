//   fragment shader
#version 120

varying vec4 color;
varying vec2 tex;
uniform sampler2D img;

void main()
{
   //  Set color
   gl_FragColor = color*texture2D(img,tex);
}
