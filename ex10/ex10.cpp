/*
 *  Real Time Image processing
 *  Demonstrates post-processing of video images
 *
 *  Relies on OpenCV VideoIO module to snarf video
 *
 *  On Ubuntu install OpenCV VideoIO with
 *    apt-get install libopencv-videoio-dev
 *
 *  On MSYS2 install OpenCV with
 *    pacman -S mingw-w64-x86_64-opencv
 *
 *  Key bindings:
 *  m          Toggle shader
 *  o          Change objects
 *  arrows     Change location
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
#include <opencv2/videoio.hpp>
int mode=0;    //  Shader
int N=1;       //  Number of passes
float asp=1;   //  Aspect ratio
float zoom=1;  //  Zoom level
float X=0,Y=0; //  Initial position
//  Images
int width,height;          //  Original image size
unsigned char* rgba;       //  Working array
unsigned int img[2]={0,0}; //  Image textures
unsigned int framebuf[2];  //  Frame buffers
//  Shader programs
#define MODE 9
int shader[MODE] = {0};
const char* text[] = {"No Shader","Copy","Sharpen","Blur","Erosion","Dilation","Laplacian","Prewitt","Sobel"};
//  OpenCV camera
using namespace cv;
VideoCapture cam;

//
//  Capture images
//
int capture()
{
   //  Capture image
   Mat frame;
   if (!cam.read(frame)) return 0;
   //  If it is not BGR color don't know what to do
   int k = frame.type();
   if (k!=CV_8UC3) Fatal("Invalid frame type %d size %d %d channels %d\n",k,frame.cols,frame.rows,frame.channels());

   //  Extract data from frame
   int r=2,g=1,b=0;
   unsigned char* dst = rgba;
   for (int i=0;i<height;i++)
   {
      //  Frame origin is bottom left
      unsigned char* src = frame.data+frame.channels()*width*(height-1-i);
      for (int j=0;j<width;j++)
      {
         *dst++ = src[r];
         *dst++ = src[g];
         *dst++ = src[b];
         *dst++ = 0xFF;
         src += frame.channels();
      }
   }

   //  Copy image to texture 0
   glBindTexture(GL_TEXTURE_2D,img[0]);
   glTexImage2D(GL_TEXTURE_2D,0,4,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
   ErrCheck("Capture");

   //  Ping-Pong between framebuffers
   if (mode)
   {
      //  Enable textures and shader
      glEnable(GL_TEXTURE_2D);
      glUseProgram(shader[mode]);
      //  Resolution uniforms
      int id = glGetUniformLocation(shader[mode],"dX");
      glUniform1f(id,1.0/width);
      id = glGetUniformLocation(shader[mode],"dY");
      glUniform1f(id,1.0/height);
      //  Identity projection
      glViewport(0,0,width,height);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      //  Copy entire screen
      for (int i=0;i<N;i++)
      {
         //  Input image is from the last framebuffer
         glBindTexture(GL_TEXTURE_2D,img[i%2]);
         //  Output to alternate framebuffers
         glBindFramebuffer(GL_FRAMEBUFFER,framebuf[(i+1)%2]);
         //  Clear the screen
         glClear(GL_COLOR_BUFFER_BIT);
         //  Redraw the screen
         glBegin(GL_QUADS);
         glTexCoord2f(0,0); glVertex2f(-1,-1);
         glTexCoord2f(0,1); glVertex2f(-1,+1);
         glTexCoord2f(1,1); glVertex2f(+1,+1);
         glTexCoord2f(1,0); glVertex2f(+1,-1);
         glEnd();
      }
      //  Disable textures and shaders
      glDisable(GL_TEXTURE_2D);
      glUseProgram(0);
      //  Output to screen
      glBindFramebuffer(GL_FRAMEBUFFER,0);
   }
   ErrCheck("Process");

   return 1;
}

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Erase the window
   glClear(GL_COLOR_BUFFER_BIT);
   //  Window size
   int wid,hgt;
   glfwGetFramebufferSize(window,&wid,&hgt);
   //  Set projection and view
   glViewport(0,0,wid,hgt);
   Projection(0,asp,1);
   View(0,0,0,0);

   //  Input image is from the last framebuffer
   glBindTexture(GL_TEXTURE_2D,img[mode?N%2:0]);
   glClear(GL_COLOR_BUFFER_BIT);
   //  Redraw the screen
   float Casp = width/(float)height;
   glEnable(GL_TEXTURE_2D);
   glScaled(zoom,zoom,1);
   glTranslated(X,Y,0);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex2f(-Casp,-1);
   glTexCoord2f(0,1); glVertex2f(-Casp,+1);
   glTexCoord2f(1,1); glVertex2f(+Casp,+1);
   glTexCoord2f(1,0); glVertex2f(+Casp,-1);
   glEnd();
   glDisable(GL_TEXTURE_2D);

   //  Display parameters
   SetColor(1,1,1);
   glWindowPos2i(5,5);
   Print("Mode=%s Passes=%d",text[mode],N);
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glfwSwapBuffers(window);
}

//
//  Key pressed callback
//
void key(GLFWwindow* window,int key,int scancode,int action,int mods)
{
   //  Discard key releases (keeps PRESS and REPEAT)
   if (action==GLFW_RELEASE) return;

   //  Check for shift
   int shift = (mods & GLFW_MOD_SHIFT);

   //  Exit on ESC
   if (key == GLFW_KEY_ESCAPE)
     glfwSetWindowShouldClose(window,1);
   //  Reset zoom
   else if (key==GLFW_KEY_0)
      zoom = 0;
   //  Switch shaders
   else if (key==GLFW_KEY_M)
      mode = shift ? (mode+MODE-1)%MODE : (mode+1)%MODE;
   //  Number of passes
   else if ((key==GLFW_KEY_KP_SUBTRACT || key==GLFW_KEY_MINUS) && N>1)
      N --;
   else if (key==GLFW_KEY_KP_ADD || key==GLFW_KEY_EQUAL)
      N++;
   //  Change location
   else if (key==GLFW_KEY_RIGHT)
      X -= 0.03/zoom;
   else if (key==GLFW_KEY_LEFT)
      X += 0.03/zoom;
   else if (key==GLFW_KEY_UP)
      Y -= 0.03/zoom;
   else if (key==GLFW_KEY_DOWN)
      Y += 0.03/zoom;
   //  PageUp key - decrease zoom
   else if (key==GLFW_KEY_PAGE_DOWN)
      zoom /= 1.1;
   //  PageDown key - increase zoom
   else if (key==GLFW_KEY_PAGE_UP)
      zoom *= 1.1;

   //  Limit zoom
   if (zoom<1)
   {
      zoom = 1;
      X = Y = 0;
   }
}

//
//  Window resized callback
//
void reshape(GLFWwindow* window,int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("Image Processing",1,600,600,&reshape,&key);

   //  Allow camera to be selected
   int kam = (argc>1) ? strtod(argv[1],NULL) : 0;
   //  Initialize OpenCV
   if(!cam.open(kam)) Fatal("Could not initialize video source\n");

   //  Get image size
   width  = cam.get(CAP_PROP_FRAME_WIDTH);
   height = cam.get(CAP_PROP_FRAME_HEIGHT);
   fprintf(stderr,"Video stream %dx%d\n",width,height);
   //  Working array
   rgba = (unsigned char*)malloc(4*width*height);
   if (!rgba) Fatal("Cannot allocate %d bytes for image\n",4*width*height);
   //  Allocate frame buffers and textures
   glGenFramebuffers(2,framebuf);   
   glGenTextures(2,img);
   for (int k=0;k<2;k++)
   {
      //  Set texture parameters
      glBindTexture(GL_TEXTURE_2D,img[k]);
      glTexImage2D(GL_TEXTURE_2D,0,4,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
      //  Bind frame buffer to texture
      glBindFramebuffer(GL_FRAMEBUFFER,framebuf[k]);
      glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,img[k],0);
   }
   glBindFramebuffer(GL_FRAMEBUFFER,0);
   ErrCheck("Framebuffer");

   //  Load shaders
   shader[1] = CreateShaderProg(NULL,"copy.frag");
   shader[2] = CreateShaderProg(NULL,"sharpen.frag");
   shader[3] = CreateShaderProg(NULL,"blur.frag");
   shader[4] = CreateShaderProg(NULL,"erosion.frag");
   shader[5] = CreateShaderProg(NULL,"dilation.frag");
   shader[6] = CreateShaderProg(NULL,"laplacian.frag");
   shader[7] = CreateShaderProg(NULL,"prewitt.frag");
   shader[8] = CreateShaderProg(NULL,"sobel.frag");

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Capture image and display
      if (capture())
         display(window);
      //  Process any events
      glfwPollEvents();
   }
   //  Shut down GLFW
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
