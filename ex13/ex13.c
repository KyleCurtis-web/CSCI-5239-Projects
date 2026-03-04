/*
 *  Earth in Stored Textures
 *
 *  Based on the examples in Orange Book Chapter 10
 *  The textures are NASA's Blue Marble Next Generation Plain
 *
 *  Key bindings:
 *  m/M        Cycle modes (season/diurnal/clouds)
 *  s/S        Start/stop light
 *  []         Move light
 *  +/-        Change day
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int mode=0;           //  Mode
int move=1;           //  Move light
int th=0;             //  Azimuth of view angle
int ph=0;             //  Elevation of view angle
int zh=0;             //  Light azimuth
double asp=1;         //  Aspect ratio
double dim=3.0;       //  Size of world
int shader;           //  Shader programs
int day[12];          //  Day textures
const int DT=365*360;
#define MODE 3
const char* text[] = {"Seasonal","Diurnal","Clouds"};

/*
 *  Call this routine to display the scene
 */
void display(GLFWwindow* window)
{
   int ndm[] = {31,28,31,30,31,30,31,31,30,31,30,31};
   int doy = zh/360;
   int mo,dy,hr,mn;
   int id;
   //  Sun angle
   float fh = doy*360.0/365.0;

   //  Time of day
   id = (zh+(int)fh)%360;
   hr = (id/15)%24;
   mn = 4*(id%15);
   //  Compute month and day
   dy = doy+1;
   for (mo=0;dy>ndm[mo];mo++)
      dy -= ndm[mo];
   fh = (dy-1)/(float)ndm[mo];
   mo++;

   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Set tranformation
   View(th,ph,0,dim);
   //  Lighting
   Lighting(1000*Cos(fh),0.0,1000*Sin(fh), 0,0.3,1.0);

   //  Texture for this month
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,day[mo-1]);
   //  Texture for next month
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D,day[mo%12]);

   //  Enable shader
   glUseProgram(shader);
   //  Set textures
   id = glGetUniformLocation(shader,"DayTex0");
   glUniform1i(id,0);
   id = glGetUniformLocation(shader,"DayTex1");
   glUniform1i(id,1);
   id = glGetUniformLocation(shader,"NightTex");
   glUniform1i(id,2);
   id = glGetUniformLocation(shader,"CGETex");
   glUniform1i(id,3);
   id = glGetUniformLocation(shader,"mode");
   glUniform1i(id,mode);
   id = glGetUniformLocation(shader,"frac");
   glUniform1f(id,fh);

   //  Draw Planet
   SetColor(1,1,1);
   glEnable(GL_TEXTURE_2D);
   //  Rotate Z up and inclined 23.5 degrees, spinning around axis
   glRotated(-90,1,0,0);
   glRotated(-23.5,0,1,0);
   glRotated(zh,0,0,1);
   //  Solid Sphere
   glScaled(2,2,2);
   SolidSphere(36);
   //  Shader off
   glDisable(GL_TEXTURE_2D);
   glUseProgram(0);

   //  No lighting from here on
   glDisable(GL_LIGHTING);

   //  Axes
   glBegin(GL_LINES);
   glVertex3f(0,0,+2.5);
   glVertex3f(0,0,-2.5);
   glEnd();

   //  Display parameters
   SetColor(1,1,1);
   glWindowPos2i(5,5);
   Print("FPS=%d Dim=%.1f %d/%.2d %.2d:%.2d UTC %s", 
      FramesPerSecond(),dim,mo,dy,hr,mn,text[mode]);
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glfwSwapBuffers(window);
}

/*
 *  Call this routine when an arrow key is pressed
 */
void key(GLFWwindow* window,int key,int scancode,int action,int mods)
{
   //  Discard key releases (keeps PRESS and REPEAT)
   if (action==GLFW_RELEASE) return;

   //  Check for shift
   int shift = (mods & GLFW_MOD_SHIFT);

   int dt = (mode==0) ? 360 : 1;

   //  Exit on ESC
   if (key == GLFW_KEY_ESCAPE)
      glfwSetWindowShouldClose(window,1);
   //  Reset view angle
   else if (key == GLFW_KEY_0)
      th = ph = 0;
   //  Toggle light movement
   else if (key == GLFW_KEY_S)
      move = 1-move;
   //  Toggle mode
   else if (key == GLFW_KEY_M)
      mode = shift ? (mode+MODE-1)%MODE : (mode+1)%MODE;
   //  Day position
   else if (key==GLFW_KEY_KP_SUBTRACT || key==GLFW_KEY_MINUS)
      zh -= 360;
   else if (key==GLFW_KEY_KP_ADD || key==GLFW_KEY_EQUAL)
      zh += 360;
   //  Light position
   else if (key == GLFW_KEY_LEFT_BRACKET)
      zh -= dt;
   else if (key == GLFW_KEY_RIGHT_BRACKET)
      zh += dt;
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

   //  Adjust zh into range
   while (zh<0)
     zh += DT;
   while (zh>=DT)
     zh -= DT;

   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Set projection
   Projection(0,asp,dim);
}

/*
 *  Call this routine when the window is resized
 */
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

/*
 *  Start up GLFW and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("Blue Marble",1,600,600,&reshape,&key);

   //  Make sure enough texture units are available
   int n;
   glGetIntegerv(GL_MAX_TEXTURE_UNITS,&n);
   if (n<4) Fatal("Insufficient texture Units %d\n",n);
   //  Load daytime textures
   day[0]  = LoadTexBMP("day01.bmp");
   day[1]  = LoadTexBMP("day02.bmp");
   day[2]  = LoadTexBMP("day03.bmp");
   day[3]  = LoadTexBMP("day04.bmp");
   day[4]  = LoadTexBMP("day05.bmp");
   day[5]  = LoadTexBMP("day06.bmp");
   day[6]  = LoadTexBMP("day07.bmp");
   day[7]  = LoadTexBMP("day08.bmp");
   day[8]  = LoadTexBMP("day09.bmp");
   day[9]  = LoadTexBMP("day10.bmp");
   day[10] = LoadTexBMP("day11.bmp");
   day[11] = LoadTexBMP("day12.bmp");
   //  Load nightime texture to texture unit 2
   glActiveTexture(GL_TEXTURE2);
   LoadTexBMP("night.bmp");
   //  Load cloud, gloss and elevation texture to texture unit 3
   glActiveTexture(GL_TEXTURE3);
   LoadTexBMP("cge.bmp");
   //  Load shaders
   shader = CreateShaderProg("earth.vert","earth.frag");

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Light animation
      if (move) zh = fmod(30*glfwGetTime(),360);
      //  Speed up seasons in mode 0
      if (!mode) zh *= 360;
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
