//   vertex shader
#version 120

varying vec4 color;
varying vec2 tex;

void main()
{
   // texture
   tex  = gl_MultiTexCoord0.xy;
   //  Transform
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;


   //  color is direct map of NDC to RGB
   //convert from clip space to NDC, then convert from NDC to RGB
   //calculate NDC in range -1,1 then shift to 0,2 then normalize to get 0,1
   //also since we divide w/w w=1 so alpha will be 1 at the end
   color = (gl_Position / gl_Position.w); //convert from clip space by perspective division(divide xyz by w) into NDC
   color += 1.0; // shift the -1,1 range to 0,2
   color = normalize(color); // normalize the 0,2 range to 0,1

   //color = gl_Color;
   //color = pos;
   //color = gl_Position;
}
