/*
 *  nBody Simulator
 *
 *  For Apple compile this with GL3.2
 *  Others use GL3.3
 *
 *  Demonstartes a geometry shader.
 *  OpenMP is used to accelerate computations.
 *
 *  Key bindings:
 *  m/M        Cycle through modes
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int N=1024;       //  Number of bodies
int src=0;        //  Offset of first star in source
int dst=0;        //  Offset of first star in destination
int axes=0;       //  Display axes
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
double dim=10;    //  Size of universe
double vel=0.1;   //  Relative speed
int mode=0;       //  Solver mode
int shader=0;     //  Shader
const char* text[] = {"Sequential","OpenMP","OpenMP+Geometry Shader"};

//  Star
typedef struct
{
   float x,y,z;  //  Position
   float u,v,w;  //  Velocity
   float r,g,b;  //  Color
}  Star;
Star* stars=NULL;

//
//  Advance time one time step for star k
//
void Move(int k)
{
   int k0 = k+src;
   int k1 = k+dst;
   float dt = 1e-3;
   //  Calculate force components
   double a=0;
   double b=0;
   double c=0;
   for (int i=src;i<src+N;i++)
   {
      double dx = stars[i].x-stars[k0].x;
      double dy = stars[i].y-stars[k0].y;
      double dz = stars[i].z-stars[k0].z;
      double d = sqrt(dx*dx+dy*dy+dz*dz)+1;  // Add 1 to d to dampen movement
      double f = 1/(d*d*d);                  // Normalize and scale to 1/r^2
      a += f*dx;
      b += f*dy;
      c += f*dz;
   }
   //  Update velocity
   stars[k1].u = stars[k0].u + dt*a;
   stars[k1].v = stars[k0].v + dt*b;
   stars[k1].w = stars[k0].w + dt*c;
   //  Update position
   stars[k1].x = stars[k0].x + dt*stars[k1].u;
   stars[k1].y = stars[k0].y + dt*stars[k1].v;
   stars[k1].z = stars[k0].z + dt*stars[k1].w;
}

//
//  Advance time one time step
//
void Step()
{
   //  Switch source and destination
   src = src?0:N;
   dst = dst?0:N;
   //  OpenMP
   if (mode)
      #pragma omp parallel for
      for (int k=0;k<N;k++)
         Move(k);
   //  Sequential
   else
      for (int k=0;k<N;k++)
         Move(k);
}

//
//  Scaled random value
//
void rand3(float Sx,float Sy,float Sz,float* X,float* Y,float* Z)
{
   float x = 0;
   float y = 0;
   float z = 0;
   float d = 2;
   while (d>1)
   {
      x = rand()/(0.5*RAND_MAX)-1;
      y = rand()/(0.5*RAND_MAX)-1;
      z = rand()/(0.5*RAND_MAX)-1;
      d = x*x+y*y+z*z;
   }
   *X = Sx*x;
   *Y = Sy*y;
   *Z = Sz*z;
}

//
//  Initialize nBody problem
//
void InitLoc()
{
   //  Allocate room for twice as many bodies to facilitate ping-pong
   if (!stars) stars = malloc(2*N*sizeof(Star));
   if (!stars) Fatal("Error allocating memory for %d stars\n",N);
   src = N;
   dst = 0;
   //  Assign random locations
   for (int k=0;k<N;k++)
   {
      rand3(dim/2,dim/2,dim/3,&stars[k].x,&stars[k].y,&stars[k].z);
      rand3(vel,vel,vel,&stars[k].u,&stars[k].v,&stars[k].w);
      switch (k%3)
      {
         case 0:
           stars[k].r = 1.0;
           stars[k].g = 1.0;
           stars[k].b = 1.0;
           break;
         case 1:
           stars[k].r = 1.0;
           stars[k].g = 0.9;
           stars[k].b = 0.5;
           break;
         case 2:
           stars[k].r = 0.5;
           stars[k].g = 0.9;
           stars[k].b = 1.0;
           break;
      }
      stars[k+N] = stars[k];
   }
}

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT);
   //  Undo previous transformations
   View(th,ph,55,dim);

   //  Integrate
   Step();

   //  Set shader
   if (mode==2)
   {
      glUseProgram(shader);
      int id = glGetUniformLocation(shader,"star");
      glUniform1i(id,0);
      id = glGetUniformLocation(shader,"size");
      glUniform1f(id,0.3);
      glBlendFunc(GL_ONE,GL_ONE);
      glEnable(GL_BLEND);
      float mat[16];
      glGetFloatv(GL_MODELVIEW_MATRIX,mat);
      id = glGetUniformLocation(shader,"ModelViewMatrix");
      if (id>=0) glUniformMatrix4fv(id,1,0,mat);
      glGetFloatv(GL_PROJECTION_MATRIX,mat);
      id = glGetUniformLocation(shader,"ProjectionMatrix");
      if (id>=0) glUniformMatrix4fv(id,1,0,mat);

      //  Draw stars using vertex arrays
      int lv = glGetAttribLocation(shader,"Vertex");
      glVertexAttribPointer(lv,3,GL_FLOAT,0,sizeof(Star),&stars[0].x);
      glEnableVertexAttribArray(lv);
      int lc = glGetAttribLocation(shader,"Color");
      glVertexAttribPointer(lc,3,GL_FLOAT,0,sizeof(Star),&stars[0].r);
      glEnableVertexAttribArray(lc);
      //  Draw all stars from dst count N
      glDrawArrays(GL_POINTS,dst,N);
      //  Disable vertex arrays
      glDisableVertexAttribArray(lv);
      glDisableVertexAttribArray(lc);
   }
   else
   {
      //  Define vertex and color arrays
      glVertexPointer(3,GL_FLOAT,sizeof(Star),&stars[0].x);
      glEnableClientState(GL_VERTEX_ARRAY);
      glColorPointer(3,GL_FLOAT,sizeof(Star),&stars[0].r);
      glEnableClientState(GL_COLOR_ARRAY);
      //  Draw all stars from dst count N
      glDrawArrays(GL_POINTS,dst,N);
      //  Disable vertex and color arrays
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);
   }

   //  Unset shader
   if (mode==2)
   {
      glUseProgram(0);
      glDisable(GL_BLEND);
   }

   //  Draw axes
   Axes(2.5);
   //  Display parameters
   glWindowPos2i(5,5);
   Print("FPS=%d Angle=%d,%d Mode=%s",
      FramesPerSecond(),th,ph,text[mode]);
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
   //  Cycle modes
   else if (key == GLFW_KEY_M)
      mode = (mode+1)%3;
   //  Reset view angle
   else if (key == GLFW_KEY_0)
      th = ph = 0;
   //  Reset simulation
   else if (key == GLFW_KEY_R)
      InitLoc();
   //  Toggle axes
   else if (key == GLFW_KEY_A)
      axes = 1-axes;
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

   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
}

//
//  Window resized callback
//
void reshape(GLFWwindow* window,int width,int height)
{
   //  Get framebuffer dimensions (makes Apple work right)
   glfwGetFramebufferSize(window,&width,&height);
   //  Ratio of the width to the height of the window
   float asp = (height>0) ? (float)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Projection(55,asp,dim);
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("nBody Simulator",0,600,600,&reshape,&key);

   //  Initialize stars
   InitLoc();
   //  Shader program
#ifdef __APPLE__
   shader = CreateShaderGeom("nbody32.vert","nbody32.geom","nbody32.frag");
#else
   shader = CreateShaderGeom("nbody.vert","nbody.geom","nbody.frag");
#endif
   //  Star texture
   LoadTexBMP("star.bmp");

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
