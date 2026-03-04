//  Earth Fragment shader
#version 120

varying vec3 View;            //  Eye position
varying vec3 Light;           //  Light direction
varying vec3 Normal;          //  Normal vector
uniform sampler2D DayTex0;    //  Day time texture 0
uniform sampler2D DayTex1;    //  Day time texture 1
uniform sampler2D NightTex;   //  Night time texture
uniform sampler2D CGETex;     //  Cloud, Gloss and Elevation texture
uniform int mode;             //  Mode
uniform float frac;           //  Month fraction
const float abit=0.1;        //  Start of twilight

void main()
{
   //  Mix day textures
   vec4 day = mix(texture2D(DayTex0,gl_TexCoord[0].xy) , texture2D(DayTex1,gl_TexCoord[0].xy) , frac);
   //  Get night texture
   vec4 night = texture2D(NightTex,gl_TexCoord[0].xy);
   //  Get cloud, gloss and elevation from texture
   vec4 cge = texture2D(CGETex,gl_TexCoord[0].xy);
   float cloud = (mode>1) ? cge.r : 0.0;
   float gloss = cge.g;
   float elev  = cge.b;

   //  N is the object normal
   vec3 N = normalize(Normal);
   //  L is the light direction
   vec3 L = normalize(Light);

   //  Diffuse light is cosine of light and normal vectors
   float Id = dot(L,N);

   //  Adjust day for specular
   if (mode>0 && Id>0.0)
   {
      //  R is the reflected light vector R = 2(L.N)N - L
      vec3 R = reflect(L,N);
      //  V is the view vector (eye vector)
      vec3 V = normalize(View);
      //  Specular is cosine of reflected and view vectors
      float Is = dot(R,V);
      //  Adjust brightness for gloss
      if (Is>0.0 && gloss>0.5) Id += pow(Is,32.0);
   }

   if (gloss>0.5) day += vec4(0.0,0.1,0.3,0.0);

   //  Bump intensity for twilight zone (beyond the horizon)
   Id += abit;

   //  Adjust day for clouds (reflection)
   day = vec4(cloud)+(1.0-cloud)*day;

   //  Adjust night for non-linear brighness and clouds
   night = (1.0-cloud)*night*night;

   //  Seasonal (daytime only)
   if (mode==0)
      gl_FragColor = day;
   //  Daylight
   else if (Id>abit)
      gl_FragColor = Id*day;
   //  Night
   else if (Id<0.0)
      gl_FragColor = night;
   //  Twilight
   else
      gl_FragColor = mix(night,abit*day,Id/abit);
}
