//a shader that spins the texture around the center of the face

//  Phong lighting
#version 120

vec4 phong()
{
   //  P is the vertex coordinate on body
   vec3 P = vec3(gl_ModelViewMatrix * gl_Vertex);
   //  N is the object normal at P
   vec3 N = normalize(gl_NormalMatrix * gl_Normal);
   //  Light Position for light 0
   vec3 LightPos = vec3(gl_LightSource[0].position);
   //  L is the light vector
   vec3 L = normalize(LightPos - P);
   //  R is the reflected light vector R = 2(L.N)N - L
   vec3 R = reflect(-L, N);
   //  V is the view vector (eye at the origin)
   vec3 V = normalize(-P);

   //  Diffuse light intensity is cosine of light and normal vectors
   float Id = max(dot(L,N) , 0.0);
   //  Shininess intensity is cosine of light and reflection vectors to a power
   float Is = (Id>0.0) ? pow(max(dot(R,V) , 0.0) , gl_FrontMaterial.shininess) : 0.0;

   //  Vertex color
   return gl_FrontMaterial.emission                         // Emission color
     +    gl_LightModel.ambient*gl_FrontMaterial.ambient    // Global ambient
     +    gl_FrontLightProduct[0].ambient                   // Light[0] ambient
     + Id*gl_FrontLightProduct[0].diffuse                   // Light[0] diffuse
     + Is*gl_FrontLightProduct[0].specular;                 // Light[0] specular
}

uniform float time;

void main()
{
   //  Vertex color (using Phong lighting)
   gl_FrontColor = phong();
   //  Return fixed transform coordinates for this vertex
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

   //  Texture coordinates
   //rotate around the center of the texture
   float sCenter = gl_MultiTexCoord0.s - 0.5;
   float tCenter = gl_MultiTexCoord0.t - 0.5;

   //rotate and shift back
   float sNew = ((sCenter * cos(time)) - (tCenter * sin(time))) + 0.5;
   float tNew = ((sCenter * sin(time)) + (tCenter * cos(time))) + 0.5;

   //apply the texture
   gl_TexCoord[0] = vec4(sNew, tNew, gl_MultiTexCoord0.r,gl_MultiTexCoord0.q);

}
