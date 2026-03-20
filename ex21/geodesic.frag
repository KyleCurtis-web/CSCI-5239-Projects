//  Geodesic Fragment Shader
#version 400

//  Colors
const vec3 AmbientMaterial = vec3(0.05,0.05,0.05);
const vec3 DiffuseMaterial = vec3(0.00,0.75,0.75);
const vec3 red = vec3(1.0,0.0,0.0);
//  Light direction
uniform vec3 LightDir;
//  Normal
in  vec3  gFacetNormal;
//  Distance to edge of patch and triange
in  vec3  gTriDistance;
in  vec3  gPatchDistance;
//  Output color
out vec4  FragColor;

void main()
{
   //  Diffuse only lighting
   vec3 N = normalize(gFacetNormal);
   vec3 L = normalize(LightDir);
   vec3 color = AmbientMaterial + max(dot(N,L),0.0) * DiffuseMaterial;

   //  Draw mesh in red, heavy for patches, light for triangles
   float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
   float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
   color = mix(red,color,step(0.02,d1) * step(0.01,d2));

   //  Pixel color
   FragColor = vec4(color, 1.0);
}
