/*
 *  Compute Shaders
 *
 *  Demonstrate a compute shader with particle
 *  Derived from Mike Bailey's compute shader example
 *
 *  NOTE:  I use glDispatchComputeGroupSizeARB so that I can
 *         change the size of a group at run time.
 *         This is not always well supported so you may need
 *         to change this to glDispatchCompute and specify only
 *         the number of groups and then set the group size in 
 *         in the shader.
 *
 *  Key bindings:
 *  r          Reset
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int axes=1;          //  Display axes
int th=0;            //  Azimuth of view angle
int ph=0;            //  Elevation of view angle
int zh=30;           //  Light angle
int nw,ng,n;         //  Work group size and count
double asp=1;        //  Aspect ratio
double dim=1000;     //  Size of world
int shader;          //  Shader program
unsigned int posbuf; //  Position buffer
unsigned int velbuf; //  Velocity buffer
unsigned int colbuf; //  Color buffer

typedef struct 
{
  union {float x;float r;};
  union {float y;float g;};
  union {float z;float b;};
  union {float w;float a;};
} vec4;

//
//  Random numbers from min to max
//
static float frand(float min,float max)
{
   return rand()*(max-min)/RAND_MAX+min;
}

//
//  Reset particles
//
void ResetPart( )
{
   vec4 *pos,*vel,*col;
   //  Reset position
   glBindBuffer(GL_SHADER_STORAGE_BUFFER,posbuf);
   pos = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,n*sizeof(vec4),GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT);
   for (int i=0;i<n;i++)
   {
      pos[i].x = frand(0,100);
      pos[i].y = frand(+400,+600);
      pos[i].z = frand(-50,+50);
      pos[i].w = 1;
   }
   glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

   //  Reset velocities
   glBindBuffer(GL_SHADER_STORAGE_BUFFER,velbuf);
   vel = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,n*sizeof(vec4),GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT);
   for (int i=0;i<n;i++ )
   {
      vel[i].x = frand(-10,+10);
      vel[i].y = frand(-10,+10);
      vel[i].z = frand(-10,+10);
      vel[i].w = 0;
   }
   glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

   //  Reset colors
   glBindBuffer(GL_SHADER_STORAGE_BUFFER,colbuf);
   col = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,n*sizeof(vec4),GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT);
   for (int i=0;i<n;i++)
   {
      col[i].r = frand(0.1,1.0);
      col[i].g = frand(0.1,1.0);
      col[i].b = frand(0.1,1.0);
      col[i].a = 1.;
   }
   glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

   //  Set buffer base
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER,4,posbuf);
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER,5,velbuf);
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER,6,colbuf);

   //  Unset buffer
   glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
}

//
//  Initialize particles
//
void InitPart(void)
{
   //  Get max workgroup size and count
   glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,0,&ng);
   glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0,&nw);
   if (ng>8192) ng = 8192;
   n = nw*ng;

   //  Initialize position buffer
   glGenBuffers(1,&posbuf);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER,posbuf);
   glBufferData(GL_SHADER_STORAGE_BUFFER,n*sizeof(vec4),NULL,GL_STATIC_DRAW);

   //  Initialize velocity buffer
   glGenBuffers(1, &velbuf);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER,velbuf);
   glBufferData(GL_SHADER_STORAGE_BUFFER,n*sizeof(vec4),NULL,GL_STATIC_DRAW);

   //  Initialize color buffer
   glGenBuffers(1, &colbuf);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER,colbuf);
   glBufferData(GL_SHADER_STORAGE_BUFFER,n*sizeof(vec4),NULL,GL_STATIC_DRAW);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER,0 );

   // Reset buffer positions
   ResetPart();
}


//
//  Draw particles
//
void DrawPart(void)
{
   //  Set particle size
   glPointSize(1);
   //  Vertex array
   glBindBuffer(GL_ARRAY_BUFFER,posbuf);
   glVertexPointer(4,GL_FLOAT,0,(void*)0);
   //  Color array
   glBindBuffer(GL_ARRAY_BUFFER,colbuf);
   glColorPointer(4,GL_FLOAT,0,(void*)0);
   //  Enable arrays used by DrawArrays
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);
   //  Draw arrays
   glDrawArrays(GL_POINTS,0,n);
   //  Disable arrays
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   //  Reset buffer
   glBindBuffer(GL_ARRAY_BUFFER,0);
}

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   const float SphereY = -500;
   const float SphereR = +600;

   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   //  Set eye position
   View(th,ph,55,dim);
   //  Enable lighting
   Lighting(dim*Cos(zh),dim,dim*Sin(zh) , 0.3,0.5,0.5);

   //  Draw sphere
   SetColor(0.8,0.8,0);
   glPushMatrix();
   glTranslatef(0,SphereY,0);
   glRotatef(-90,1,0,0);
   glScaled(SphereR,SphereR,SphereR);
   SolidSphere(32);
   glPopMatrix();

   //  Disable lighting before particles
   glDisable(GL_LIGHTING);

   //  Launch compute shader
   glUseProgram(shader);
   int id = glGetUniformLocation(shader,"xyz");
   glUniform3f(id,0,SphereY,0);
   id = glGetUniformLocation(shader,"dim");
   glUniform1f(id,SphereR);
   glDispatchComputeGroupSizeARB(n/nw,1,1,nw,1,1);
   glUseProgram(0);

   //  Wait for compute shader
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

   //  Draw the particles
   DrawPart();

   //  Draw Axes
   Axes(500);

   //  Display parameters
   glDisable(GL_DEPTH_TEST);
   glWindowPos2i(5,5);
   Print("%d,%d FPS=%d Dim=%.1f Size=%d Count=%d N=%d",th,ph,FramesPerSecond(),dim,nw,ng,n);
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

   //  Exit on ESC
   if (key == GLFW_KEY_ESCAPE)
      glfwSetWindowShouldClose(window,1);
   //  Reset view angle
   else if (key == GLFW_KEY_0)
      th = ph = 0;
   //  Toggle axes
   else if (key == GLFW_KEY_A)
      axes = 1-axes;
   //  Reset particles
   else if (key == GLFW_KEY_R)
      ResetPart();
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
      dim += 10;
   //  PageDown key - decrease dim
   else if (key == GLFW_KEY_PAGE_UP && dim>300)
      dim -= 10;

   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Projection(55,asp,dim);
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
   Projection(55,asp,dim);
}

//
//  Create Shader Program with Location Names
//
int CreateShaderProgCompute(char* file)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile compute shader
   CreateShader(prog,GL_COMPUTE_SHADER,file);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("HW10: Kyle Curtis",0,600,600,&reshape,&key);

   //  Compute shader
   shader = CreateShaderProgCompute("particle.cs");
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
