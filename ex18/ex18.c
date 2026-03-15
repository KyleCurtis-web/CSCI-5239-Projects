/*
 *  Particle Shaders
 *
 *  Demonstrate a small particle shader using the Orange Book confetti cannon
 *  and a large particle shader using the bambi fire.
 *
 *  Key bindings:
 *  m/M        Toggle between particle shaders
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int axes=1;       //  Display axes
int mode=0;       //  Shader mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int n;            //  Particle count
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world
#define MODE 2
int shader[MODE] = {0,0}; //  Shader programs
const char* text[] = {"Confetti Cannon","Fire"};

//  Point arrays
#define N 100
float Vert[3*N*N];
float Color[3*N*N];
float Vel[3*N*N];
float Start[N*N];

//
//  Random numbers with range and offset
//
static float frand(float rng,float off)
{
   return rand()*rng/RAND_MAX+off;
}

//
//  Initialize particles
//
void InitPart(void)
{
   //  Array Pointers
   float* vert  = Vert;
   float* color = Color;
   float* vel   = Vel;
   float* start = Start;
   //  Loop over NxN patch
   n = mode ? 15 : N;
   for (int i=0;i<n;i++)
      for (int j=0;j<n;j++)
      {
         //  Location x,y,z
         *vert++ = (i+0.5)/n-0.75;
         *vert++ = 0;
         *vert++ = (j+0.5)/n-0.75;
         //  Color r,g,b (0.5-1.0)
         *color++ = frand(0.5,0.5);
         *color++ = frand(0.5,0.5);
         *color++ = frand(0.5,0.5);
         //  Velocity
         *vel++ = frand( 1.0,3.0);
         *vel++ = frand(10.0,0.0);
         *vel++ = frand( 1.0,3.0);
         //  Launch time
         *start++ = frand(2.0,0.0);
      }
}

//
//  Draw particles
//
void DrawPart(void)
{
   //  Set particle size
   glPointSize(mode?50:2);
   //  Point vertex location to local array Vert
   glVertexPointer(3,GL_FLOAT,0,Vert);
   //  Point color array to local array Color
   glColorPointer(3,GL_FLOAT,0,Color);
   //  Point attribute arrays to local arrays
   int locvel = glGetAttribLocation(shader[mode],"Vel");
   if (locvel>=0) glVertexAttribPointer(locvel,3,GL_FLOAT,GL_FALSE,0,Vel);
   int locstr = glGetAttribLocation(shader[mode],"Start");
   if (locstr>=0) glVertexAttribPointer(locstr,1,GL_FLOAT,GL_FALSE,0,Start);
   //  Enable arrays used by DrawArrays
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);
   if (locvel>=0) glEnableVertexAttribArray(locvel);
   if (locstr>=0) glEnableVertexAttribArray(locstr);
   //  Set transparent large particles
   if (mode)
   {
      glEnable(GL_POINT_SPRITE);
      glTexEnvi(GL_POINT_SPRITE,GL_COORD_REPLACE,GL_TRUE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE);
      glDepthMask(0);
   }
   //  Draw arrays
   glDrawArrays(GL_POINTS,0,n*n);
   //  Reset
   if (mode)
   {
      glDisable(GL_POINT_SPRITE);
      glDisable(GL_BLEND);
      glDepthMask(1);
   }
   //  Disable arrays
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   if (locvel>=0) glDisableVertexAttribArray(locvel);
   if (locstr>=0) glDisableVertexAttribArray(locstr);
}

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   View(th,ph,0,dim);

   //
   //  Draw scene
   //
   //  Select shader (0 => no shader)
   glUseProgram(shader[mode]);
   //  Set time
   int id = glGetUniformLocation(shader[mode],"time");
   glUniform1f(id,glfwGetTime());
   id = glGetUniformLocation(shader[mode],"Noise3D");
   glUniform1i(id,1);
   id = glGetUniformLocation(shader[mode],"img");
   glUniform1i(id,0);

   //  Draw the particles
   DrawPart();

   //  No shader for what follows
   glUseProgram(0);

   //  Draw axes - no lighting from here on
   if (axes) Axes(2);
   //  Display parameters
   glWindowPos2i(5,5);
   Print("FPS=%d Dim=%.1f Mode=%s",
     FramesPerSecond(),dim,text[mode]);
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
   //  Reset view angle
   else if (key == GLFW_KEY_0)
      th = ph = 0;
   //  Toggle axes
   else if (key == GLFW_KEY_A)
      axes = 1-axes;
   //  Cycle modes
   else if (key == GLFW_KEY_M)
   {
      mode = shift ? (mode+MODE-1)%MODE : (mode+1)%MODE;
      InitPart();
   }
   //  Right arrow key - increase angle by 5 degrees
   else if (key == GLFW_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLFW_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLFW_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLFW_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLFW_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLFW_KEY_PAGE_UP && dim>1)
      dim -= 0.1;

   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Projection(0,asp,dim);
}

//
//  Window resized callback
//
void reshape(GLFWwindow* window,int width,int height)
{
   //  Get framebuffer dimensions (makes Apple work right)
   glfwGetFramebufferSize(window,&width,&height);
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Projection(0,asp,dim);
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("Particle Shaders",1,800,600,&reshape,&key);

   //  Confetti Cannon needs no fragment shader, but adds Vel and Start
   shader[0] = CreateShaderProg("confetti.vert",NULL);
   shader[1] = CreateShaderProg("fire.vert","fire.frag");
   //  Load random texture
   CreateNoise3D(GL_TEXTURE1);
   //  Load smoke particle
   LoadTexBMP("particle.bmp");
   //  Initialize particles
   InitPart();

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Display
      display(window);
      //  Process any events
      glfwPollEvents();
   }
   //  Shut down GLFW
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
