//  Geodesic Tesselation Evalution Shader

#version 400

//  Equilateral triangles in (CW order)
layout(triangles, equal_spacing, cw) in;
//  Modelview and projection matrices
uniform mat4 Projection;
uniform mat4 Modelview;
//  Coordinates in and out
in  vec3 tcPosition[];
out vec3 tePosition;
out vec3 tePatchDistance;

void main()
{
   //  Distance to edge of patch for interpolation
   tePatchDistance = gl_TessCoord;
   //  Calculate vertex as weighted average
   vec3 p = (gl_TessCoord.x * tcPosition[0]
          +  gl_TessCoord.y * tcPosition[1]
          +  gl_TessCoord.z * tcPosition[2]);
   //  Lengthen to unit radius
   tePosition = normalize(p);
   //  Project vertex
   gl_Position = Projection * Modelview * vec4(tePosition, 1);
}
