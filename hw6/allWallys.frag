// Inter-image processing
#version 120

uniform sampler2D img0;
uniform sampler2D img1;
uniform sampler2D img2;
uniform sampler2D img3;

void main()
{
   vec4 pix0 = texture2D(img0,gl_TexCoord[0].st);
   vec4 pix1 = texture2D(img1,gl_TexCoord[0].st);
   vec4 pix2 = texture2D(img2,gl_TexCoord[0].st);
   vec4 pix3 = texture2D(img3,gl_TexCoord[0].st);

   //subtract each pair, then add them all together
   vec4 difference = abs(pix1-pix0);
   vec4 sumOfDifference = difference;
   difference = abs(pix3-pix2);
   sumOfDifference = abs(sumOfDifference + difference);


   gl_FragColor = sumOfDifference;

}
