// Inter-image processing
#version 120

uniform sampler2D img0;
uniform sampler2D img1;
uniform sampler2D img2;
uniform sampler2D img3;

vec4 keepPixel(vec4 a, vec4 b)
{
      if(all(equal(b,a)))
        return vec4(0,0,0,0);
      else
        return b;
}

void main()
{
   vec4 pix0 = texture2D(img0,gl_TexCoord[0].st);
   vec4 pix1 = texture2D(img1,gl_TexCoord[0].st);
   vec4 pix2 = texture2D(img2,gl_TexCoord[0].st);
   vec4 pix3 = texture2D(img3,gl_TexCoord[0].st);

   //subtract each pair, then add them all together
   //first pair
   vec4 difference = keepPixel(pix0, pix1);
   vec4 sumOfDifference = difference;

   //second pair
   difference = keepPixel(pix2, pix3);
   sumOfDifference = abs(sumOfDifference + difference);



   //final color
   gl_FragColor = sumOfDifference;

}
