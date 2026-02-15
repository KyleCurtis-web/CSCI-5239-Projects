/*
 * HW6  
 Inter-Image processing
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
int shaders[2];    //  Shader program
int shaderNum = 0;  // which shader is being used
int image1 = 0;
int image2 = 1;
int imageSet = 0;
#define MODE 5
#define IMAGESETS 2
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
   glUseProgram(shaders[shaderNum]);

   if (shaderNum == 0)
   {
       //  Set mode for shader
       int id = glGetUniformLocation(shaders[0], "mode");
       glUniform1i(id, mode);
       //  Fraction used in some filters
       id = glGetUniformLocation(shaders[0], "frac");
       if (id >= 0) glUniform1f(id, frac);
       //  First image is on texture unit 0
       id = glGetUniformLocation(shaders[0], "img0");
       glUniform1i(id, image1);
       //  Second image is on texture unit 1
       id = glGetUniformLocation(shaders[0], "img1");
       glUniform1i(id, image2);
   }
   else if (shaderNum == 1)
   {
       int id = 0;
       char imgString[20];
       char* initialString = "img";
       //go though and add each image
       for (int i = 0; i < IMAGESETS * 2; i++)
       {
           sprintf(imgString, "%s%d", initialString, i);
           id = glGetUniformLocation(shaders[1], imgString);
           glUniform1i(id, i);
       }
   }
   else
   {
       //somehow there is no shader
   }

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

   glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
   glBegin(GL_QUADS);
   glVertex2f(-5, -5);
   glVertex2f(-5, -.93);
   glVertex2f(5, -.93);
   glVertex2f(5, -5);
   glEnd();

   //  Display parameters
   SetColor(1,1,1);
   glWindowPos2i(5,5);
   Print("Zoom=%.1f Offset=%f,%f Mode=%s Image Set=%d",zoom,X,Y,text[mode], imageSet + 1);
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
   //  Cycle modes forward
   else if (key == GLFW_KEY_M)
      mode = shift ? (mode+MODE-1)%MODE : (mode+1)%MODE;
   //  Cycle modes backward
   else if (key == GLFW_KEY_N)
       mode = (mode == 0) ? MODE - 1 : (mode - 1) % MODE;
   //  Cycle images forward
   else if (key == GLFW_KEY_K)
   {
       if (imageSet + 1 == IMAGESETS)
       {
           imageSet = 0;
       }
       else
       {
           imageSet += 1;
       }
       image1 = imageSet * 2;
       image2 = image1 + 1;
   }
   //  Cycle images backward
   else if (key == GLFW_KEY_J)
   {
       if (imageSet - 1 < 0)
       {
           imageSet = IMAGESETS - 1;
       }
       else
       {
           imageSet -= 1;
       }
       image1 = imageSet * 2;
       image2 = image1 + 1;
   }
   //  Cycle modes forward
   else if (key == GLFW_KEY_I)
       shaderNum = shaderNum ? 0 : 1;
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
   GLFWwindow* window = InitWindow("Kyle Curtis: HW6",1,600,600,&reshape,&key);

   //  Load first image to texture unit 0
   glActiveTexture(GL_TEXTURE0);
   LoadTexBMP("wallysGone/wallysGone1.bmp");
   //  Load second image to texture unit 1
   glActiveTexture(GL_TEXTURE1);
   LoadTexBMP("wallys/wallys1.bmp");

   glActiveTexture(GL_TEXTURE2);
   LoadTexBMP("wallysGone/wallysGone2.bmp");

   glActiveTexture(GL_TEXTURE3);
   LoadTexBMP("wallys/wallys2.bmp");

   //  Load shaders
   shaders[0] = CreateShaderProg(NULL, "imgproc.frag");

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
