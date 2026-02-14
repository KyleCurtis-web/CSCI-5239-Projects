// Inter-image processing
#version 120

uniform int       mode;
uniform float     frac;
uniform sampler2D img0;
uniform sampler2D img1;

void main()
{
   vec4 pix0 = texture2D(img0,gl_TexCoord[0].st);
   vec4 pix1 = texture2D(img1,gl_TexCoord[0].st);
   //  First image
   if (mode==0)
      gl_FragColor = pix0;
   //  Second image
   else if (mode==1)
      gl_FragColor = pix1;
   //  Transition
   else if (mode==2)
      gl_FragColor = mix(pix0,pix1,frac);
   //  Absolute difference
   else if (mode==3)
      gl_FragColor = abs(pix1-pix0);
   //  False color
   else if (mode==4)
      gl_FragColor = vec4(length(pix0.rgb),0,length(pix1.rgb),1);
   else
      discard;
}
