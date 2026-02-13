/*
 *  Inter-Image processing
 *  Mostly ripped off from Pete Battaglia (Fall 2009)
 *
 *  Key bindings:
 *  m/M        Cycle through shaders (filters)
 *  +/-        Change fraction in mix mode
 *  a          Toggle crosshairs
 *  arrows     Pan location
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view
 *  ESC        Exit
 */
#include "CSCIx239.h"
int mode=0;    //  Filter to use
int axes=1;    //  Draw crosshairs
float asp=1;   //  Aspect ratio
float zoom=1;  //  Zoom factor
float frac=0;  //  Fraction
float X=0,Y=0; //  Initial position
int shader;    //  Shader program
#define MODE 5
const char* text[] = {"Image 0","Image 1","Mix","Difference","False Color"};

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT);
   //  Set projection and view
   Projection(0,asp,1);
   View(0,0,0,0);

      //  Set up for drawing
   glPushMatrix();
   glScaled(zoom,zoom,1);
   glTranslated(X,Y,0);
   SetColor(1,1,1);
   glUseProgram(shader);

   //  Set mode for shader
   int id = glGetUniformLocation(shader,"mode");
   glUniform1i(id,mode);
   //  Fraction used in some filters
   id = glGetUniformLocation(shader,"frac");
   if (id>=0) glUniform1f(id,frac);
   //  First image is on texture unit 0
   id = glGetUniformLocation(shader,"img0");
   glUniform1i(id,0);
   //  Second image is on texture unit 1
   id = glGetUniformLocation(shader,"img1");
   glUniform1i(id,1);

   //  Draw to a quad
   glEnable(GL_TEXTURE_2D);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex2f(-1,-1);
   glTexCoord2f(0,1); glVertex2f(-1,+1);
   glTexCoord2f(1,1); glVertex2f(+1,+1);
   glTexCoord2f(1,0); glVertex2f(+1,-1);
   glEnd();
   glDisable(GL_TEXTURE_2D);

   // Disable shader
   glUseProgram(0);
   glPopMatrix();

   //  Draw crosshairs
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex2f(-0.1,0);
      glVertex2f(+0.1,0);
      glVertex2f(0,-0.1);
      glVertex2f(0,+0.1);
      glEnd();
   }

   //  Display parameters
   SetColor(1,1,1);
   glWindowPos2i(5,5);
   Print("Zoom=%.1f Offset=%f,%f Mode=%s",zoom,X,Y,text[mode]);
   if (mode==2) Print(" Fraction=%.1f",frac);
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
   //  Fraction
   else if (key==GLFW_KEY_KP_ADD || key==GLFW_KEY_EQUAL)
      frac += 0.1;
   else if (key==GLFW_KEY_KP_SUBTRACT || key==GLFW_KEY_MINUS)
      frac -= 0.1;
   //  Reset view
   else if (key == GLFW_KEY_0)
      X = Y = 0;
   //  Cycle modes
   else if (key == GLFW_KEY_M)
      mode = shift ? (mode+MODE-1)%MODE : (mode+1)%MODE;
   //  Toggle axes
   else if (key == GLFW_KEY_A)
      axes = !axes;
      //  Right arrow key - increase angle by 5 degrees
   else if (key == GLFW_KEY_RIGHT)
      X -= 0.03/zoom;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLFW_KEY_LEFT)
      X += 0.03/zoom;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLFW_KEY_UP)
      Y -= 0.03/zoom;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLFW_KEY_DOWN)
      Y += 0.03/zoom;
   //  PageUp key - increase zoom
   else if (key == GLFW_KEY_PAGE_DOWN)
      zoom /= 1.1;
   //  PageDown key - decrease zoom
   else if (key == GLFW_KEY_PAGE_UP)
      zoom *= 1.1;

   //  Limit fractions
   if (frac>1)
      frac = 1;
   else if (frac<0)
      frac = 0;
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
   //  Get framebuffer dimensions (makes Apple work right)
   glfwGetFramebufferSize(window,&width,&height);
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("Image Processing",1,600,600,&reshape,&key);

   //  Load first image to texture unit 0
   glActiveTexture(GL_TEXTURE0);
   LoadTexBMP("20090602.bmp");
   //  Load second image to texture unit 1
   glActiveTexture(GL_TEXTURE1);
   LoadTexBMP("20090706.bmp");

   //  Load shaders
   shader = CreateShaderProg(NULL,"imgproc.frag");

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Display
      display(window);
      //  Wait for next event
      glfwWaitEvents();
   }
   //  Shut down GLFW
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
