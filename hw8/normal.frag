//  Normal Map
#version 120

//  Light Vectors
varying vec3 View;
varying vec3 Light;
varying vec3 Normal;
varying vec3 Tangent;

//  Mode 0=flat, 1=Normal map in tangent space, 2=Normal map in model space
uniform int mode;
//  Textures and normal map
uniform sampler2D tex;
uniform sampler2D nml;
uniform sampler2D hgt;

void main()
{
   vec2 texCoords = gl_TexCoord[0].xy;

   //  Normalize after interpolation
   vec3 N = normalize(Normal);
   vec3 T = normalize(Tangent);
   vec3 L = normalize(Light);
   vec3 V = normalize(View);
   //  Calculate Bitangent as Normal x Tangent
   vec3 B = cross(N,T);
   mat3 TBN = mat3(T,B,N);

   //  Transform L and V to tangent space by inverting TBN
   if (mode==1)
   {
      N = normalize(vec3(2*texture2D(nml,gl_TexCoord[0].xy)-1));
      TBN = transpose(TBN);
      L = TBN*L;
      V = TBN*V;
   }
   //  Transform normal map to model space
   else if (mode==2)
      N = TBN*normalize(vec3(2*texture2D(nml,gl_TexCoord[0].xy)-1));
   //hgt map
   else if(mode==3)
   {
      //start by transforming L and V to tangent space
      N = normalize(vec3(2*texture2D(nml,gl_TexCoord[0].xy)-1));
      TBN = transpose(TBN);
      L = TBN*L;
      V = TBN*V;

      float height = texture2D(hgt, gl_TexCoord[0].xy).r;
      vec2 p = (V.xy / V.z) * (height * 0.1); //the 0.1 is the height scale
      texCoords = texCoords - p;

      N = vec3(texture2D(nml, texCoords));
      N = normalize(N * 2.0 - 1.0);

   }

   //  Diffuse light is cosine of light and normal vectors
   float Id = max(dot(L,N) , 0.0);
   //  Specular is cosine of reflected and view vectors
   float Is = (Id>0.0) ? pow(max(dot(reflect(-L,N),V),0.0) , gl_FrontMaterial.shininess) : 0.0;

   //  Sum color types
   vec4 color = gl_FrontLightProduct[0].ambient
              + Id*gl_FrontLightProduct[0].diffuse
              + Is*gl_FrontLightProduct[0].specular;

   //  Pixel color
   gl_FragColor = color*texture2D(tex,texCoords);
}
