// Inter-image processing
#version 120

uniform sampler2D img0;
uniform sampler2D img1;
uniform sampler2D img2;
uniform sampler2D img3;
uniform sampler2D img4;
uniform sampler2D img5;
uniform sampler2D img6;
uniform sampler2D img7;

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
   vec4 pix4 = texture2D(img4,gl_TexCoord[0].st);
   vec4 pix5 = texture2D(img5,gl_TexCoord[0].st);
   vec4 pix6 = texture2D(img6,gl_TexCoord[0].st);
   vec4 pix7 = texture2D(img7,gl_TexCoord[0].st);

   //subtract each pair, then add them all together
   //first pair
   vec4 difference = keepPixel(pix0, pix1);
   vec4 sumOfDifference = difference;

   //second pair
   difference = keepPixel(pix2, pix3);
   sumOfDifference = abs(sumOfDifference + difference);

   //third pair
   difference = keepPixel(pix4, pix5);
   sumOfDifference = abs(sumOfDifference + difference);

   //fourth pair
   difference = keepPixel(pix6, pix7);
   sumOfDifference = abs(sumOfDifference + difference);

   //final color
   gl_FragColor = sumOfDifference;

}
