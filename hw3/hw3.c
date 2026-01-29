/*
 *
 *  Key bindings:
 *  m          Toggle shaders
 *  o          Change objects
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int mode=0;    //  Shader
int move=1;    //  Moving light
int cull=0;    //  Face cull
int zh=0;      //  Light position
int th=0,ph=0; //  View angles
int fov=57;    //  Field of view (for perspective)
int tex=0;     //  Texture
int obj=0;     //  Object
float YL=1.5;  //  Light elevation
float asp=1;   //  Aspect ratio
float dim=3;   //  Size of world
#define MODE 11
int shader[] = {0,0,0,0,0,0,0,0,0,0,0};  //  Shaders
const char* text[] = {"None","Stored","Float Color", "Integer Color", "If Color", "Crash and Burn", "Burn and Crash", "Transcendental Sin", "Spin", "In Line", "Function"};

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   if (cull) glEnable(GL_CULL_FACE);
   //  Set view
   View(th,ph,fov,dim);
   //  Enable lighting
   Lighting(3*Cos(zh),YL,3*Sin(zh) , 0.3,0.5,0.8);

   //  Select shader
   glUseProgram(shader[mode]);
   //  Draw scene
   //crash and burn, make a new object every second
   if (mode == 5)
   {
       //  Time in seconds
       int t = glfwGetTime();
       int x = 0, y = 0, z = 0;



       while (t > 0)
       {
           if (x - dim > dim)
           {
               x = 0;
               y++;
               if (y - dim > dim)
               {
                   y = 0;
                   z++;
                   if (z - dim > dim)
                   {
                       break;
                   }
               }
           }
           Cube(-dim + x, -dim + y, -dim + z, 1, 1, 1, 0, 0, tex);
           t--;
           x++;
       }
   }
   else if (mode == 6)
   {
       //  Time in seconds
       int dimSize = dim * 2;
       int cubeSpace = dimSize * dimSize * dimSize;
       int t = cubeSpace - glfwGetTime();
       int x = 0, y = 0, z = 0;



       while (t > 0)
       {
           if (x - dim > dim)
           {
               x = 0;
               y++;
               if (y - dim > dim)
               {
                   y = 0;
                   z++;
                   if (z - dim > dim)
                   {
                       break;
                   }
               }
           }
           Cube(-dim + x, -dim + y, -dim + z, 1, 1, 1, 0, 0, tex);
           t--;
           x++;
       }
   }
   else if (mode == 8)
   {
       float spin = glfwGetTime() * 10;
       if (obj)
           Icosahedron(0,0,0,1, spin,0, tex);
       else
           Cube(0,0,0,1,1,1, spin,0,tex);
   }
   else
   {
       if (obj)
           TexturedIcosahedron(tex);
       else
           TexturedCube(tex);
   }
   //  Revert to fixed pipeline
   glUseProgram(0);
   glDisable(GL_LIGHTING);
   if (cull) glDisable(GL_CULL_FACE);

   //  Display parameters
   SetColor(1,1,1);
   glWindowPos2i(5,5);
   Print("Angle=%d,%d FPS=%d Dim=%.1f Projection=%s Mode=%s Face Cull=%s",th,ph,FramesPerSecond(),dim,fov>0?"Perpective":"Orthogonal",text[mode],cull?"On":"Off");
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
   //  Reset view angle and location
   else if (key==GLFW_KEY_0)
      th = ph = 0;
   //  Switch shaders
   else if (key==GLFW_KEY_M)
      mode = shift ? (mode+MODE-1)%MODE : (mode+1)%MODE;
   //  Light movement
   else if (key==GLFW_KEY_S)
      move = 1-move;
   //  Light elevation
   else if (key==GLFW_KEY_KP_SUBTRACT || key==GLFW_KEY_MINUS)
      YL -= 0.05;
   else if (key==GLFW_KEY_KP_ADD || key==GLFW_KEY_EQUAL)
      YL += 0.05;
   //  Toggle face cull
   else if (key==GLFW_KEY_C)
      cull = 1-cull;
   //  Switch objects
   else if (key==GLFW_KEY_O)
      obj = 1-obj;
   //  Switch between perspective/orthogonal
   else if (key==GLFW_KEY_P)
      fov = fov ? 0 : 57;
   //  Increase/decrease asimuth
   else if (key==GLFW_KEY_RIGHT)
      th += 5;
   else if (key==GLFW_KEY_LEFT)
      th -= 5;
   //  Increase/decrease elevation
   else if (key==GLFW_KEY_UP)
      ph += 5;
   else if (key==GLFW_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key==GLFW_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key==GLFW_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
   //  Wrap angles
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
   GLFWwindow* window = InitWindow("HW3 Kyle Curtis",0,600,600,&reshape,&key);

   //  Load shaders
   shader[1] = CreateShaderProg("stored.vert", "stored.frag");
   shader[2] = CreateShaderProg("floatcolor.vert",NULL);
   shader[3] = CreateShaderProg("intcolor.vert", NULL);
   shader[4] = CreateShaderProg("ifcolor.vert", NULL);
   shader[5] = CreateShaderProg("stored.vert", "stored.frag");
   shader[6] = CreateShaderProg("stored.vert", "stored.frag");
   shader[7] = CreateShaderProg("transcendentalColor.vert", "stored.frag");
   shader[8] = CreateShaderProg("stored.vert", "stored.frag");
   shader[9] = CreateShaderProg("exponentialInLineColor.vert", "stored.frag");
   shader[10] = CreateShaderProg("exponentialFunctionColor.vert", "stored.frag");
   //  Load textures
   tex = LoadTexBMP("pi.bmp");

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
