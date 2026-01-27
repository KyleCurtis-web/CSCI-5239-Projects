// Fragment shader for drawing the Mandelbrot set
// with per pixel phong lighting
#version 120

varying vec3 View;
varying vec3 Light;
varying vec3 Normal;
varying vec4 Ambient;


// Phong lighting calculation
vec4 phong()
{
   //  N is the object normal
   vec3 N = normalize(Normal);
   //  L is the light vector
   vec3 L = normalize(Light);

   //  Emission and ambient color
   vec4 color = Ambient;

   //  Diffuse light is cosine of light and normal vectors
   float Id = dot(L,N);
   if (Id>0.0)
   {
      //  Add diffuse
      color += Id*gl_FrontLightProduct[0].diffuse;
      //  R is the reflected light vector R = 2(L.N)N - L
      vec3 R = reflect(-L,N);
      //  V is the view vector (eye vector)
      vec3 V = normalize(View);
      //  Specular is cosine of reflected and view vectors
      float Is = dot(R,V);
      if (Is>0.0) color += pow(Is,gl_FrontMaterial.shininess)*gl_FrontLightProduct[0].specular;
   }

   //  Return sum of color components
   return color;
}

//  Mandelbrot set calculation
vec4 mandelbrot(vec2 c)
{
   // Maximum number of iterations
   const int   MaxIter = 100;
   // Beyond this the sequence always diverges
   const float MaxR2   = 4.0;
   // Colors
   const vec4  In   = vec4(0,0,0,1);
   const vec4  Out1 = vec4(1,0,0,1);
   const vec4  Out2 = vec4(0,1,0,1);

   //  Iterate z = z^2 + c;
   int   iter;
   vec2  z  = vec2(0,0);
   float r2 = 0.0;
   for (iter=0 ; iter<MaxIter && r2<MaxR2 ; iter++)
   {
       z  = vec2(z.x*z.x-z.y*z.y , 2.0*z.x*z.y) + c;
       r2 = z.x*z.x+z.y*z.y;
   }

   // Base the color on the number of iterations
   return (r2<MaxR2) ? In : mix(Out1,Out2,float(iter)/float(MaxIter));
}

void main()
{
   gl_FragColor = phong()*mandelbrot(2*(gl_TexCoord[0].xy-vec2(0.7,0.5)));
}
