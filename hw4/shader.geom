#version 450

layout(binding=0) uniform UniformBufferObject {
   mat4 model;  // Model matrix
   mat4 view;   // View matrix
   mat4 proj;   // Projection matrix
   mat4 norm;   // Normal matrix (passed as 4x4 to facilitate alignment)
   vec4 pos;    // Light position
   vec4 Ca;     // Light ambient
   vec4 Cd;     // Light diffuse
   vec4 Cs;     // Light specular
   vec4 Ks;     // Material specular
   float Ns;    // Material shininess
   } ubo;

layout (points) in;

layout (points, max_vertices = 4) out;


void main()
{
}


