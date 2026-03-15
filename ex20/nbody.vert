//  nBody Vertex shader
#version 330

in vec4 Vertex;
in vec4 Color;
out vec4 vColor;

void main()
{
   //  Remember the color
   vColor = Color;
   //  Defer all transformations to geometry shader
   gl_Position = Vertex;
}
