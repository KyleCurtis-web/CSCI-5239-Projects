// Inter-image processing
#version 120

uniform sampler2D img0;
uniform sampler2D img1;
uniform sampler2D img2;

vec4 keepPixel(vec4 a, vec4 b)
{
      if(all(equal(b,a)))
        return vec4(0,0,0,0);
      else
        return b;
}

void main()
{
   vec4 pix0 = texture2D(img0,gl_TexCoord[0].st);//no wally
   vec4 pix1 = texture2D(img1,gl_TexCoord[0].st);//wally
   vec4 pix2 = texture2D(img2,gl_TexCoord[0].st);//new image for wally

   vec4 difference = keepPixel(pix0, pix1);
   vec4 blank = vec4(0,0,0,0);

   //if not wally (black space) then just new image, otherwise add wally
   if(all(equal(difference, blank)))
        gl_FragColor = pix2;
   else
        gl_FragColor = difference;

}
