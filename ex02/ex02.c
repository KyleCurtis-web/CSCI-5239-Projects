/*
 *  NDC2RGB shader
 *  Map NDC [-1,+1] to RGB [0,1]
 *
 *  Key bindings:
 *  m          Toggle shaders
 *  o          Change objects
 *  xXyYzZ     Change location
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int mode=0;    //  Shader
int th=0,ph=0; //  View angles
int fov=57;    //  Field of view (for perspective)
int tex=0;     //  Texture
int obj=0;     //  Object
float asp=1;   //  Aspect ratio
float dim=3;   //  Size of world
float X=0,Y=0,Z=0;         //  Location of Object
#define MODE 4
int shader[] = {0,0,0,0};  //  Shaders
const char* text[] = {"Fixed Pipeline","Vertex+Fragment","Vertex","Fragment"};

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

   //  Select shader
   glUseProgram(shader[mode]);
   //  Set dimensions for shader 3
   if (mode==3)
   {
      int id = glGetUniformLocation(shader[mode],"dim");
      int width,height;
      glfwGetFramebufferSize(window,&width,&height);
      glUniform3f(id,width,height,1);
   }
   //  Transform
   glPushMatrix();
   glTranslated(X,Y,Z);
   //  Draw scene
   if (obj)
      TexturedIcosahedron(tex);
   else
      TexturedCube(tex);
   //  Transform
   glPopMatrix();
   //  Revert to fixed pipeline
   glUseProgram(0);

   //  Display parameters
   SetColor(1,1,1);
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f Projection=%s Mode=%s",th,ph,dim,fov>0?"Perpective":"Orthogonal",text[mode]);
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glfwSwapBuffers(window);
}

//
//  Move to position
//
static void moveto(float x,float y, float z)
{
   X = x;
   Y = y;
   Z = z;
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
   //  Change location
   else if (key==GLFW_KEY_X)
      X += shift ? +0.05 : -0.05;
   else if (key==GLFW_KEY_Y)
      Y += shift ? +0.05 : -0.05;
   else if (key==GLFW_KEY_Z)
      Z += shift ? +0.05 : -0.05;
   //  Reset view angle and location
   else if (key==GLFW_KEY_0)
      X = Y = Z = th = ph = 0;
   //  Move to the corners
   else if (key==GLFW_KEY_1)
      moveto(-dim,-dim,-dim);
   else if (key==GLFW_KEY_2)
      moveto(+dim,-dim,-dim);
   else if (key==GLFW_KEY_3)
      moveto(-dim,+dim,-dim);
   else if (key==GLFW_KEY_4)
      moveto(+dim,+dim,-dim);
   else if (key==GLFW_KEY_5)
      moveto(-dim,-dim,+dim);
   else if (key==GLFW_KEY_6)
      moveto(+dim,-dim,+dim);
   else if (key==GLFW_KEY_7)
      moveto(-dim,+dim,+dim);
   else if (key==GLFW_KEY_8)
      moveto(+dim,+dim,+dim);
   //  Switch shaders
   else if (key==GLFW_KEY_M)
      mode = shift ? (mode+MODE-1)%MODE : (mode+1)%MODE;
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
   GLFWwindow* window = InitWindow("NDC2RGB",1,600,600,&reshape,&key);

   //  Load shaders
   shader[1] = CreateShaderProg("ndc2rgb.vert","ndc2rgb.frag");
   shader[2] = CreateShaderProg("ndc2rgb.vert",NULL);
   shader[3] = CreateShaderProg(NULL,"ndc2rgb2.frag");
   //  Load textures
   tex = LoadTexBMP("pi.bmp");

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
