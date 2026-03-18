//  Phong lighting
#version 120
#extension GL_EXT_draw_instanced : enable
//#version 140

uniform vec3 xyz[125];
uniform   float time;  //  Time
uniform   float heightCap;

varying float instanceIdentifier;

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

void main()
{
   //ID the instance
   instanceIdentifier = float(gl_InstanceID);

   //F center, the center of a letter F
   vec4 center = 3 * vec4(0.5,-0.75,-0.15,0.3);

   //position
   vec4 P = gl_Vertex;

        //  Offset
        vec4 offset = vec4(3*xyz[int(mod(gl_InstanceID, 25))],1);
        center += offset;
        P += offset;

        float verticalDif = center.y - P.y;

        //fall
        float fall = 4.9 * time;
        center.y = mod(center.y - fall, heightCap + 1.5);
        P.y = center.y - verticalDif;

   //  Vertex color (using Phong lighting)
   gl_FrontColor = phong();

   //  Texture coordinates
   gl_TexCoord[0] = gl_MultiTexCoord0;
   //  Return fixed transform coordinates for this vertex
   gl_Position = gl_ModelViewProjectionMatrix * P;

}
