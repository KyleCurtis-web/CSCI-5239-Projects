/*
 *  Object convolution
 *
 *  Demonstrate using a naive convolution of pixels to detect objects.
 *
 *  Key bindings:
 *  m/M        Cycle through modes
 *  o/O        Cycle through objects to detect
 *  +/-        Increase/Decrease object size
 *  ESC        Exit
 */
#include "CSCIx239.h"
int mode=0;             //  Shader mode
int obj=0;              //  Objects
int shader;             //  Shader programs
int width,height;       //  Width an height
int Dim=32;             //  Size of test image
unsigned int img0,img1; //  Textures
#define OBJ 5
const char* name[] = {"Triangle","Square","Diamond","Circle","Teapot"};
#define MODE 3
const char* text[] = {"Test Image","Reference Image","Convolution"};

//
//  Draw object
//
void DrawObj(int obj,float dx,float dy,int type)
{
   glPushMatrix();
   glTranslatef(dx,dy,0);
   SetColor(1,1,0);
   //  Triangle
   if (obj==0)
   {
      glBegin(type);
      glVertex2f(-1,-1);
      glVertex2f(+1,-1);
      glVertex2f(+0,+1);
      glEnd();
   }
   //  Square
   else if (obj==1)
   {
      glBegin(type);
      glVertex2f(-1,-1);
      glVertex2f(+1,-1);
      glVertex2f(+1,+1);
      glVertex2f(-1,+1);
      glEnd();
   }
   //  Diamond
   else if (obj==2)
   {
      glBegin(type);
      glVertex2f( 0,-1);
      glVertex2f(+1, 0);
      glVertex2f( 0,+1);
      glVertex2f(-1, 0);
      glEnd();
   }
   //  Circle
   else if (obj==3)
   {
      glBegin(type);
      int k;
      for (k=0;k<360;k+=10)
         glVertex2f(Cos(k),Sin(k));
      glEnd();
   }
   //  Teapot
   else
   {
      glPushMatrix();
      glPolygonMode(GL_FRONT_AND_BACK,(type==GL_POLYGON)?GL_FILL:GL_LINE);
      glScalef(0.6,0.6,0.6);
      SolidTeapot(2);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glPopMatrix();
   }
   glPopMatrix();
}

//
//  Draw all objects in the scene
//
void DrawScene(int type,int dim)
{
   //  Set dimensions so that the object size in pixels are correct
   double dx = (double)width/dim;
   double dy = (double)height/dim;

   //  Set projection and draw all the objects
   glPushMatrix();
   glOrtho(-dx,+dx,-dy,+dy,-1,1);
   DrawObj(0,-2,-2,type);
   DrawObj(1,+2,-2,type);
   DrawObj(2,-2,+2,type);
   DrawObj(3,+2,+2,type);
   DrawObj(4, 0, 0,type);
   glPopMatrix();
}

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Set reference image dimensions
   int dim = Dim;
   if (width <dim) dim = width;
   if (height<dim) dim = height;

   //  Make test image
   //  Draws the object
   //  Copy image to texture on unit 0
   glClear(GL_COLOR_BUFFER_BIT);
   glViewport(0,0, width,height);
   DrawScene(GL_POLYGON,dim);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,img0);
   glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,0,0,width,height,0);

   //  Make reference image
   //  Limit viewport to dim x dim
   //  Draw object to fill that size
   //  Copy image to texture on unit 1
   glClear(GL_COLOR_BUFFER_BIT);
   glViewport(0,0, dim,dim);
   DrawObj(obj,0,0,GL_POLYGON);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D,img1);
   glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,0,0,dim,dim,0);
   glActiveTexture(GL_TEXTURE0);

   //
   //  Process images using the shader
   //
   glClear(GL_COLOR_BUFFER_BIT);
   glViewport(0,0, width,height);
   if (mode==2) DrawScene(GL_LINE_LOOP,dim);

   glUseProgram(shader);

   //  Set offsets
   int id = glGetUniformLocation(shader,"mode");
   if (id>=0) glUniform1i(id,mode);
   id = glGetUniformLocation(shader,"img0");
   if (id>=0) glUniform1i(id,0);
   id = glGetUniformLocation(shader,"img1");
   if (id>=0) glUniform1i(id,1);
   id = glGetUniformLocation(shader,"dim");
   if (id>=0) glUniform1i(id,dim);
   id = glGetUniformLocation(shader,"dX");
   if (id>=0) glUniform1f(id,1.0/width);
   id = glGetUniformLocation(shader,"dY");
   if (id>=0) glUniform1f(id,1.0/height);

   //  Do the whole screen
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex2f(-1,-1);
   glTexCoord2f(0,1); glVertex2f(-1,+1);
   glTexCoord2f(1,1); glVertex2f(+1,+1);
   glTexCoord2f(1,0); glVertex2f(+1,-1);
   glEnd();

   glUseProgram(0);

   //  Display parameters
   SetColor(1,1,1);
   glWindowPos2i(5,5);
   Print("Mode=%s Obj=%s Dim=%d",text[mode],name[obj],dim);
   if (mode==MODE-1) Print(" Time=%f",Elapsed());
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
   //  Cycle modes
   else if (key == GLFW_KEY_M)
      mode = shift ? (mode+MODE-1)%MODE : (mode+1)%MODE;
   //  Cycle objects
   else if (key == GLFW_KEY_O)
      obj = shift ? (obj+OBJ-1)%OBJ : (obj+1)%OBJ;
   //  Change dimensions
   else if ((key==GLFW_KEY_KP_ADD || key==GLFW_KEY_EQUAL) && Dim<256)
      Dim *= 2;
   else if ((key==GLFW_KEY_KP_SUBTRACT || key==GLFW_KEY_MINUS) && Dim>8)
      Dim /= 2;
}

//
//  Window resized callback
//
void reshape(GLFWwindow* window,int w,int h)
{
   //  Get framebuffer dimensions (makes Apple work right)
   glfwGetFramebufferSize(window,&width,&height);
   //  Indentity projections
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("Find the Teapot",1,600,600,&reshape,&key);

   //  Create textures
   glGenTextures(1,&img0);
   glBindTexture(GL_TEXTURE_2D,img0);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   glGenTextures(1,&img1);
   glBindTexture(GL_TEXTURE_2D,img1);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   //  Create Shader Programs
   shader = CreateShaderProg(NULL,"convolution.frag");

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Display
      display(window);
      //  Wait for events
      glfwWaitEvents();
   }
   //  Shut down GLFW
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
