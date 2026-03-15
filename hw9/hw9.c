/*
 *  Instancing demonstrated using the icosahedron
 *
 *  Key bindings:
 *  xXyYzZ     Change location or zoom
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int zh=0;       //  Light position
int th=0,ph=0;  //  View angles
int fov=57;     //  Field of view (for perspective)
int tex=0;      //  Texture
float asp=1;    //  Aspect ratio
float dim=5;    //  Size of world
int shader = 0; //  Shaders
float xyz[375]; //  125 vec3 locations 

//  Icosahedron data stored in VBO
static unsigned int vbo=0;
static const int n      = 60;                //  Number of vertexes
static const int stride = 11*sizeof(float);  //  Stride (bytes)
//  Vertex coordinates, normals, textures and colors
static const float data[] =
{
// X      Y      Z       Nx     Ny     Nz    S   T   R G B 
 0.276, 0.851, 0.447,  0.471, 0.342, 0.761, 0.0,0.0, 0,0,1,
 0.894, 0.000, 0.447,  0.471, 0.342, 0.761, 1.0,0.0, 0,0,1,
 0.000, 0.000, 1.000,  0.471, 0.342, 0.761, 0.5,1.0, 0,0,1,
-0.724, 0.526, 0.447, -0.180, 0.553, 0.761, 0.0,0.0, 0,1,0,
 0.276, 0.851, 0.447, -0.180, 0.553, 0.761, 1.0,0.0, 0,1,0,
 0.000, 0.000, 1.000, -0.180, 0.553, 0.761, 0.5,1.0, 0,1,0,
-0.724,-0.526, 0.447, -0.582, 0.000, 0.762, 0.0,0.0, 0,1,1,
-0.724, 0.526, 0.447, -0.582, 0.000, 0.762, 1.0,0.0, 0,1,1,
 0.000, 0.000, 1.000, -0.582, 0.000, 0.762, 0.5,1.0, 0,1,1,
 0.276,-0.851, 0.447, -0.180,-0.553, 0.761, 0.0,0.0, 1,0,1,
-0.724,-0.526, 0.447, -0.180,-0.553, 0.761, 1.0,0.0, 1,0,1,
 0.000, 0.000, 1.000, -0.180,-0.553, 0.761, 0.5,1.0, 1,0,1,
 0.894, 0.000, 0.447,  0.471,-0.342, 0.761, 0.0,0.0, 1,1,0,
 0.276,-0.851, 0.447,  0.471,-0.342, 0.761, 1.0,0.0, 1,1,0,
 0.000, 0.000, 1.000,  0.471,-0.342, 0.761, 0.5,1.0, 1,1,0,
 0.000, 0.000,-1.000,  0.180, 0.553,-0.761, 0.0,0.0, 0,0,1,
 0.724, 0.526,-0.447,  0.180, 0.553,-0.761, 1.0,0.0, 0,0,1,
-0.276, 0.851,-0.447,  0.180, 0.553,-0.761, 0.5,1.0, 0,0,1,
 0.000, 0.000,-1.000, -0.471, 0.342,-0.761, 0.0,0.0, 0,1,0,
-0.276, 0.851,-0.447, -0.471, 0.342,-0.761, 1.0,0.0, 0,1,0,
-0.894, 0.000,-0.447, -0.471, 0.342,-0.761, 0.5,1.0, 0,1,0,
 0.000, 0.000,-1.000, -0.471,-0.342,-0.761, 0.0,0.0, 0,1,1,
-0.894, 0.000,-0.447, -0.471,-0.342,-0.761, 1.0,0.0, 0,1,1,
-0.276,-0.851,-0.447, -0.471,-0.342,-0.761, 0.5,1.0, 0,1,1,
 0.000, 0.000,-1.000,  0.180,-0.553,-0.761, 0.0,0.0, 1,0,0,
-0.276,-0.851,-0.447,  0.180,-0.553,-0.761, 1.0,0.0, 1,0,0,
 0.724,-0.526,-0.447,  0.180,-0.553,-0.761, 0.5,1.0, 1,0,0,
 0.000, 0.000,-1.000,  0.582, 0.000,-0.762, 0.0,0.0, 1,0,1,
 0.724,-0.526,-0.447,  0.582, 0.000,-0.762, 1.0,0.0, 1,0,1,
 0.724, 0.526,-0.447,  0.582, 0.000,-0.762, 0.5,1.0, 1,0,1,
 0.894, 0.000, 0.447,  0.761, 0.552, 0.180, 0.0,0.0, 1,1,0,
 0.276, 0.851, 0.447,  0.761, 0.552, 0.180, 1.0,0.0, 1,1,0,
 0.724, 0.526,-0.447,  0.761, 0.552, 0.180, 0.5,1.0, 1,1,0,
 0.276, 0.851, 0.447, -0.291, 0.894, 0.179, 0.0,0.0, 0,0,1,
-0.724, 0.526, 0.447, -0.291, 0.894, 0.179, 1.0,0.0, 0,0,1,
-0.276, 0.851,-0.447, -0.291, 0.894, 0.179, 0.5,1.0, 0,0,1,
-0.724, 0.526, 0.447, -0.940, 0.000, 0.179, 0.0,0.0, 0,1,0,
-0.724,-0.526, 0.447, -0.940, 0.000, 0.179, 1.0,0.0, 0,1,0,
-0.894, 0.000,-0.447, -0.940, 0.000, 0.179, 0.5,1.0, 0,1,0,
-0.724,-0.526, 0.447, -0.291,-0.894, 0.179, 0.0,0.0, 0,1,1,
 0.276,-0.851, 0.447, -0.291,-0.894, 0.179, 1.0,0.0, 0,1,1,
-0.276,-0.851,-0.447, -0.291,-0.894, 0.179, 0.5,1.0, 0,1,1,
 0.276,-0.851, 0.447,  0.761,-0.552, 0.180, 0.0,0.0, 1,0,0,
 0.894, 0.000, 0.447,  0.761,-0.552, 0.180, 1.0,0.0, 1,0,0,
 0.724,-0.526,-0.447,  0.761,-0.552, 0.180, 0.5,1.0, 1,0,0,
 0.276, 0.851, 0.447,  0.291, 0.894,-0.179, 0.0,0.0, 1,0,1,
-0.276, 0.851,-0.447,  0.291, 0.894,-0.179, 1.0,0.0, 1,0,1,
 0.724, 0.526,-0.447,  0.291, 0.894,-0.179, 0.5,1.0, 1,0,1,
-0.724, 0.526, 0.447, -0.761, 0.552,-0.180, 0.0,0.0, 1,1,0,
-0.894, 0.000,-0.447, -0.761, 0.552,-0.180, 1.0,0.0, 1,1,0,
-0.276, 0.851,-0.447, -0.761, 0.552,-0.180, 0.5,1.0, 1,1,0,
-0.724,-0.526, 0.447, -0.761,-0.552,-0.180, 0.0,0.0, 0,0,1,
-0.276,-0.851,-0.447, -0.761,-0.552,-0.180, 1.0,0.0, 0,0,1,
-0.894, 0.000,-0.447, -0.761,-0.552,-0.180, 0.5,1.0, 0,0,1,
 0.276,-0.851, 0.447,  0.291,-0.894,-0.179, 0.0,0.0, 0,1,0,
 0.724,-0.526,-0.447,  0.291,-0.894,-0.179, 1.0,0.0, 0,1,0,
-0.276,-0.851,-0.447,  0.291,-0.894,-0.179, 0.5,1.0, 0,1,0,
 0.894, 0.000, 0.447,  0.940, 0.000,-0.179, 0.0,0.0, 0,1,1,
 0.724, 0.526,-0.447,  0.940, 0.000,-0.179, 1.0,0.0, 0,1,1,
 0.724,-0.526,-0.447,  0.940, 0.000,-0.179, 0.5,1.0, 0,1,1,
};

//
//  Random numbers from min to max
//
static float frand(float min,float max)
{
   return rand()*(max-min)/RAND_MAX+min;
}

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
   //  Enable lighting
   Lighting(5*Cos(zh),5,5*Sin(zh) , 0.3,0.5,0.8);

   //  Select shader
   glUseProgram(shader);
   int loc = glGetUniformLocation(shader,"xyz");
   glUniform3fv(loc,125,xyz);
   //  Initialize VBO on first use
   if (!vbo)
   {
      //  Get buffer name
      glGenBuffers(1,&vbo);
      //  Bind VBO
      glBindBuffer(GL_ARRAY_BUFFER,vbo);
      //  Copy icosahedron to VBO
      glBufferData(GL_ARRAY_BUFFER,sizeof(data),data,GL_STATIC_DRAW);
   }
   //  On subsequanet calls, just bind VBO
   else
      glBindBuffer(GL_ARRAY_BUFFER,vbo);

   //  Define arrays
   glVertexPointer(3,GL_FLOAT,stride,(void*)0);
   glNormalPointer(GL_FLOAT,stride,(void*)12);
   glTexCoordPointer(2,GL_FLOAT,stride,(void*)24);
   glColorPointer(3,GL_FLOAT,stride,(void*)32);

   //  Enable arrays
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);

   //  Draw icosahedron
   glDrawArraysInstanced(GL_TRIANGLES,0,n,125);

   //  Disable arrays
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);

   //  Release VBO
   glBindBuffer(GL_ARRAY_BUFFER,0);
   //  Revert to fixed pipeline
   glUseProgram(0);
   glDisable(GL_LIGHTING);

   //  Display parameters
   SetColor(1,1,1);
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f Projection=%s",th,ph,dim,fov>0?"Perpective":"Orthogonal");
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
   //  int shift = (mods & GLFW_MOD_SHIFT);

   //  Exit on ESC
   if (key == GLFW_KEY_ESCAPE)
      glfwSetWindowShouldClose(window,1);
   //  Reset view angle and location
   else if (key==GLFW_KEY_0)
      th = ph = 0;
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
   GLFWwindow* window = InitWindow("Instanced Icosahedra",1,600,600,&reshape,&key);

   //  Load shaders
   shader = CreateShaderProg("instance.vert","instance.frag");
   //  Load textures
   tex = LoadTexBMP("pi.bmp");

   //  Initialize locations
   int k=0;
   for (int x=-2;x<=2;x++)
      for (int y=-2;y<=2;y++)
         for (int z=-2;z<=2;z++)
         {
            xyz[k++] = x+frand(-0.125,0.125);
            xyz[k++] = y+frand(-0.125,0.125);
            xyz[k++] = z+frand(-0.125,0.125);
         }

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Light animation
      zh = fmod(90*glfwGetTime(),360);
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
