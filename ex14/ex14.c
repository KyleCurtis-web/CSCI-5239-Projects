/*
 *  Conway's Game of Life
 *
 *
 *  Note that this is a finite, wrapped universe.
 *  Sometimes you need to hit '0' to reset the simulation if the sceen is blank
 *  Golly is a great source of RLE files
 *
 *  Key bindings:
 *  s/s    Start/stop computations
 *  0      Restart
 *  ESC    Exit
 */
#include "CSCIx239.h"
#include <ctype.h>
int move=1;         //  Run
unsigned int img=0; //  Image texture
int W=0,H=0;        //  Texture dimensions
int N=-5;           //  Generations (start neative to catch initial events)
int warn=0;         //  Warn if pattern does not fit
float dX,dY;        //  Image pixel offset
int shader;         //  Shader program
char* file;         //  RLE file name

//
//  Read until the first character of the next line and return it
//
char ReadLine(FILE* f)
{
   int eol=0;
   char ch = fgetc(f);
   while ((!eol || isspace(ch)) && ch!=EOF)
   {
      if (ch=='\n' || ch=='\r') eol = 1;
      ch = fgetc(f);
   }
   return ch;
}

//
//  Load pattern from RLE file
//
void LoadPattern(char* file)
{
   char  ch;        //  Read character
   int   x0=0;      //  Starting line position
   int   x,y;       //  Current position
   int   dx,dy;     //  Pattern size
   int   k=0;       //  Repeat count
   char  rule[256]; //  Rule string

   //  Open file
   FILE* f = fopen(file,"r");
   if (!f) Fatal("Cannot open %s\n",file);
   //  Skip header (starts with #)
   for (ch=fgetc(f) ; ch=='#' ; ch=ReadLine(f));
   ungetc(ch,f);
   //  Check header line
   if (fscanf(f," x = %d , y = %d , rule = %s",&dx,&dy,rule)!=3) Fatal("Error reading header line from %s\n",file);
   if (strcmp("B3/S23",rule)) Fatal("Only rule B3/S23 implemented (%s)\n",rule);
   warn = (dx>W || dy>H);
   //  Center pattern
   x0 = x = (dx<W) ? (W-dx)/2 : 0;
   y = H-1-(dy<H ? (H-dy)/2 : 0);
   //  Read pattern from file
   for (ch=ReadLine(f) ; ch!='!' && ch!=EOF ; ch=fgetc(f))
   {
      //  Live cell(s)
      if (ch=='o')
      {
         //  k==0 => one
         if (k<=0) k = 1;
         //  Draw k live cells in a row
         for (;k>0;k--)
         {
            GLubyte dot[] = {0xFF};
            glWindowPos2i(x++,y);
            glBitmap(1,1,0,0,0,0,dot);
         }
      }
      //  Dead cell(s)
      else if (ch=='b')
      {
         x += (k?k:1);
         k = 0;
      }
      //  End of Line
      else if (ch=='$')
      {
         y -= (k?k:1);
         x = x0;
         k = 0;
      }
      //  Repeat count
      else if (isdigit(ch))
         k = 10*k+(ch-'0');
   }
   // Done
   fclose(f);
}

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Wait for first reshape
   if (H==0) return;
   //  Set initial pattern
   if (N==0)
   {
      //  Clear screen and set color
      glClearColor(0,0,0,0);
      glClear(GL_COLOR_BUFFER_BIT);
      glColor4f(1,0,0,1);
      //  Draw pattern from file
      LoadPattern(file);
   }
   //
   //  Compute next generation
   //
   else
   {
      //  Set shader
      glUseProgram(shader);

      //  Set offsets
      int id = glGetUniformLocation(shader,"dX");
      glUniform1f(id,dX);
      id = glGetUniformLocation(shader,"dY");
      glUniform1f(id,dY);
      id = glGetUniformLocation(shader,"img");
      glUniform1i(id,0);

      //  Copy original scene to texture
      glBindTexture(GL_TEXTURE_2D,img);
      glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,0,0,W,H,0);

      //  Redraw the texture
      glClear(GL_COLOR_BUFFER_BIT);
      glEnable(GL_TEXTURE_2D);
      glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex2f(-1,-1);
      glTexCoord2f(0,1); glVertex2f(-1,+1);
      glTexCoord2f(1,1); glVertex2f(+1,+1);
      glTexCoord2f(1,0); glVertex2f(+1,-1);
      glEnd();
      glDisable(GL_TEXTURE_2D);

      //  Shader off
      glUseProgram(0);
   }

   //  Lock alpha to not interfere with game
   glColorMask(1,1,1,0);
   //  Display parameters
   glColor4f(1,1,0,0);
   glWindowPos2i(5,5);
   if (warn) Print("Pattern too large for screen ");
   if (move) Print("FPS=%d ",FramesPerSecond());
   Print("Generation=%d",N);
   glColorMask(1,1,1,1);
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glfwSwapBuffers(window);
   //  Increment generations
   N++;
}

//
//  Key pressed callback
//
void key(GLFWwindow* window,int key,int scancode,int action,int mods)
{
   //  Discard key releases (keeps PRESS and REPEAT)
   if (action==GLFW_RELEASE) return;

   //  Exit on ESC
   if (key == GLFW_KEY_ESCAPE)
      glfwSetWindowShouldClose(window,1);
   else if (key == GLFW_KEY_0)
      N = 0;
   else if (key == GLFW_KEY_S)
      move = 1-move;
}

//
//  Window resized callback
//
void reshape(GLFWwindow* window,int width,int height)
{
   //  Re-initialize
   if (N>0) N = 0;
   //  Get framebuffer dimensions (makes Apple work right)
   glfwGetFramebufferSize(window,&width,&height);
   //  Set identity projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glViewport(0,0, width,height);

   //  Maximum texture size
   int MaxTexSize;
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,&MaxTexSize);

   //  Set size of texture
   W = width;
   H = height;
   if (W>MaxTexSize || H>MaxTexSize) Fatal("Window bigger that maximum texture size %d\n",MaxTexSize);
   //  Set texture offsets for kernel
   dX = 1.0/W;
   dY = 1.0/H;
}


//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("Conway's Game of Life",0,800,600,&reshape,&key);

   //  Make sure a pattern file is specified
   if (argc!=2) Fatal("Usage: %s <RLE pattern-file>\n",argv[0]);
   file = argv[1];

   //  Background color
   glClearColor(0,0,0,0);
   //  Create Shader Programs
   shader = CreateShaderProg(NULL,"life.frag");
   //  Image texture
   glBindTexture(GL_TEXTURE_2D,img);
   //  Nearest returns exact cell values
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   //  Wrap to create circular universe
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

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
