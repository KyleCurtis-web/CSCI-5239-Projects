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

// Input: points from the vertex shader
layout (triangles) in;

layout(location=0) in vec3 NormIn[];  // Normal vector
layout(location=1) in vec3 LightIn[]; // Light vector
layout(location=2) in vec3 ViewIn[];  // Eye vector
layout(location=3) in vec3 colIn[];   // Color
layout(location=4) in vec2 t2dIn[];   // Texture

// Output: line strip with a maximum of 2 vertices
layout (triangle_strip, max_vertices = 3) out;

layout(location=0) out vec3 Norm;  // Normal vector
layout(location=1) out vec3 Light; // Light vector
layout(location=2) out vec3 View;  // Eye vector
layout(location=3) out vec3 col;   // Color
layout(location=4) out vec2 t2d;   // Texture

void main() {
    //pass values to frag shader
    for(int i = 0; i < gl_in.length();i++)
    {

    //pass the outputs
    Norm = NormIn[i];
    Light = LightIn[i];
    View = ViewIn[i];
    col = colIn[i];
    t2d = t2dIn[i];

    // Vertex
    gl_Position = gl_in[i].gl_Position;
    EmitVertex();

    }

    // Finalize the primitive
    EndPrimitive();


}

