//  Geodesic Vertex Shader
#version 400

//  Coordinates in and out
in  vec4 Position;
out vec3 vPosition;

void main()
{
   //  Coordinate passthrough
   vPosition = Position.xyz;
}
