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

layout (points) in  xyz;

layout (points, max_vertices = 4) out location;


void main()
{
	location = xyz;
}


#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Define inputs from vertex shader
in VS_OUT {
    vec2 texCoord;
    vec3 normal;
} gs_in[];

// Define outputs to fragment shader
out vec2 v_texCoord;
out vec3 v_normal;

void main() {
    for (int i = 0; i < 3; i++) {
        // Pass through position
        gl_Position = gl_in[i].gl_Position;
        
        // Pass through custom attributes
        v_texCoord = gs_in[i].texCoord;
        v_normal = gs_in[i].normal;
        
        EmitVertex();
    }
    EndPrimitive();
}
