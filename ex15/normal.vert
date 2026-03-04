//  Normal Map
#version 120

// Tangent attribute
attribute vec3 glTangent;

// Vectors sent to frag shader
varying vec3 View;
varying vec3 Light;
varying vec3 Normal;
varying vec3 Tangent;

void main()
{
   //  Vertex location in modelview coordinates
   vec4 P = gl_ModelViewMatrix * gl_Vertex;
   //  Light position
   Light  = gl_LightSource[0].position.xyz - P.xyz;
   //  Normal and Tangent
   Normal  = gl_NormalMatrix * gl_Normal;
   Tangent = gl_NormalMatrix * glTangent;
   //  Eye position
   View  = -P.xyz;
   //  Texture coordinates
   gl_TexCoord[0] = gl_MultiTexCoord0;
   //  Set vertex position
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
