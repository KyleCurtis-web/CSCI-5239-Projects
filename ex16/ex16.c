/*
 *  Perlin Noise
 *
 *  Demonstrate using perlin noise to make marble, clouds, and similar
 *  noise-based procedural textures.
 *
 *  Key bindings:
 *  m/M        Cycle through shaders
 *  o/O        Cycle through objects
 *  s/S        Start/stop light
 *  p/P        Toggle between orthogonal & perspective projection
 *  -/+        Change light elevation
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int axes=1;       //  Display axes
int mode=0;       //  Shader mode
int move=1;       //  Move light
int obj=0;        //  Object
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=57;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world
int zh=90;        //  Light azimuth
float Ylight=2;   //  Light elevation
int model;        //  Object model
#define MODE 5
int shader[MODE] = {0,0,0,0,0}; //  Shader programs
const char* text[] = {"No Shader","Cloud","Sun","Marble","Wood"};

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Set view
   View(th,ph,fov,dim);
   //  Enable light
   Lighting(2*Cos(zh),Ylight,2*Sin(zh) , 0.3,1.0,1.0);

   //
   //  Draw scene
   //
   if (mode>0)
   {
      glUseProgram(shader[mode]);
      int id = glGetUniformLocation(shader[mode],"Noise3D");
      glUniform1i(id,1);
   }

   //  Draw the model, teapot or cube
   SetColor(1,1,0);
   if (obj==2)
      glCallList(model);
   else if (obj==1)
      SolidTeapot(12);
   else
      SolidCube();

   //  No shader for what follows - no lighting from here on
   glUseProgram(0);
   glDisable(GL_LIGHTING);

   //  Draw axes
   Axes(2);
   //  Display parameters
   glWindowPos2i(5,5);
   Print("FPS=%d  Dim=%.1f Projection=%s Mode=%s",
     FramesPerSecond(),dim,fov>0?"Perpective":"Orthogonal",text[mode]);
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
   //  Toggle projection type
   else if (key == GLFW_KEY_P)
      fov = fov ? 0 : 57;
   //  Toggle light movement
   else if (key == GLFW_KEY_S)
      move = 1-move;
   //  Toggle objects
   else if (key == GLFW_KEY_O)
      obj = (obj+1)%3;
   //  Cycle modes
   else if (key == GLFW_KEY_M)
      mode = shift ? (mode+MODE-1)%MODE : (mode+1)%MODE;
   //  Light elevation
   else if (key==GLFW_KEY_KP_ADD || key==GLFW_KEY_EQUAL)
      Ylight += 0.1;
   else if (key==GLFW_KEY_KP_SUBTRACT || key==GLFW_KEY_MINUS)
      Ylight -= 0.1;
   //  Light position
   else if (key == GLFW_KEY_LEFT_BRACKET)
      zh--;
   else if (key == GLFW_KEY_RIGHT_BRACKET)
      zh++;
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
   Projection(fov,asp,dim);
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
   Projection(fov,asp,dim);
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("Perlin Noise",1,600,600,&reshape,&key);

   //  Stanford Bunny
   model = LoadOBJ("bunny.obj");
   //  Load random texture
   CreateNoise3D(GL_TEXTURE1);
   //  Create Shader Programs
   shader[1] = CreateShaderProg("noise.vert","cloud.frag");
   shader[2] = CreateShaderProg("noise.vert","sun.frag");
   shader[3] = CreateShaderProg("noise.vert","marble.frag");
   shader[4] = CreateShaderProg("noise.vert","wood.frag");

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Light animation
      if (move) zh = fmod(90*glfwGetTime(),360);
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
