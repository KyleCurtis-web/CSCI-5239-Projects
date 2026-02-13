// Conovolution
#version 120

uniform int   mode;
uniform int   dim;
uniform float dX;
uniform float dY;
uniform sampler2D img0;
uniform sampler2D img1;

void main()
{
   //  Show test image
   if (mode==0)
      gl_FragColor = texture2D(img0,gl_TexCoord[0].st);
   //  Show reference image
   else if (mode==1)
      gl_FragColor = texture2D(img1,gl_TexCoord[0].st);
   //  Difference reference and test images
   else
   {
      //  Convert dimension to float
      float Dim=float(dim);
      //  Offset in test image
      vec2 off = gl_TexCoord[0].st - 0.5*Dim*vec2(dX,dY);
      //  Loop over all pixels in the reference image and sum differences
      float sum = 0.0;
      for (float s=0.0;s<Dim;s+=1.0)
         for (float t=0.0;t<Dim;t+=1.0)
         {
            //  Sample reference image
            vec3 ref = texture2D(img1,vec2(s,t)/Dim).rgb;
            //  Sample test image
            vec3 img = texture2D(img0,off+vec2(s*dX,t*dY)).rgb;
            //  Add length of difference
            sum += length(img-ref);
         }
      //  Rescale to 0-1
      sum = 1.0 - sum/(Dim*Dim);

      //  Translate sum into a color
      if (sum>0.95)
         gl_FragColor = vec4(1.0,0.0,0.0,1.0);
      else if (sum>0.90)
         gl_FragColor = vec4(1.0,0.5,0.0,1.0);
      else if (sum>0.85)
         gl_FragColor = vec4(1.0,1.0,0.0,1.0);
      else if (sum>0.80)
         gl_FragColor = vec4(0.0,1.0,0.0,1.0);
      else if (sum>0.75)
         gl_FragColor = vec4(0.0,1.0,1.0,1.0);
      else if (sum>0.70)
         gl_FragColor = vec4(0.0,0.0,1.0,1.0);
      else
         discard;
   }
}
