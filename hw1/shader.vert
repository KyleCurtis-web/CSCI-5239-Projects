//   vertex shader
#version 120

varying vec4 color;
varying vec2 tex;

void main()
{
   //  Set vertex coordinates
   vec4 pos = gl_Vertex;
   //  Scroll texture coordinates
   tex  = gl_MultiTexCoord0.xy;
   //  Transform
   gl_Position = gl_ModelViewProjectionMatrix * pos;


   //  color is direct map of NDC to RGB
   //convert from clip space to NDC, then convert from NDC to RGB
   //calculate NDC in range -1,1 then shift to 0,2 then normalize to get 0,1
   color = (gl_Position / gl_Position.w);
   color += 1;
   color = normalize(color);

   //color = gl_Color;
   //color = pos;
   //color = gl_Position;
}
