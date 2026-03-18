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
int tex =0;      //  Texture
int wally = 0;   //wally texture
int grass = 0;   //grass texture
float asp=1;    //  Aspect ratio
float dim=5;    //  Size of world
int shader = 0; //  Shaders
int defaultShader = 0;
float xyz[75]; //  125 vec3 locations 


int numInstances = 30; //the number of instances default 125
int randomInstance;
int heightCap = 10;
float time;
int paused = 0;

//F data stored in buffer
static unsigned int vbo = 0;
static const int n      = 96;                //  Number of vertexes
static const int stride = 11*sizeof(float);  //  Stride (bytes)

static const float data[] =
{
  // X Y Z       Nx Ny Nz    S T   R G B 
        // left column front
    0, 0, 0,        0, 0, 1,    0,0,    0,0,0,
    0, -1.5, 0,      0, 0, 1,    0,1,    0,0,0,
    .3, 0, 0,       0, 0, 1,    1,0,    0,0,0,
    0, -1.5, 0,      0, 0, 1,    0,1,    0,0,0,
    .3, -1.5, 0,     0, 0, 1,    1,1,    0,0,0,
    .3, 0, 0,       0, 0, 1,    1,0,    0,0,0,

       // top rung front
    .3, 0, 0,       0, 0, 1,    0,0,    1,0,0,
    .3, -.3, 0,      0, 0, 1,    0,1,    1,0,0,
    1, 0, 0,      0, 0, 1,    1,0,    1,0,0,
    .3, -.3, 0,      0, 0, 1,    0,1,    1,0,0,
    1, -.3, 0,     0, 0, 1,    1,1,    1,0,0,
    1, 0, 0,      0, 0, 1,    1,0,    1,0,0,

    // middle rung front
    .3, -.6, 0,      0, 0, 1,    0,0,    0,1,0,
    .3, -.9, 0,      0, 0, 1,    0,1,    0,1,0,
    .67, -.6, 0,      0, 0, 1,    1,0,    0,1,0,
    .3, -.9, 0,      0, 0, 1,    0,1,    0,1,0,
    .67, -.9, 0,      0, 0, 1,    1,1,    0,1,0,
    .67, -.6, 0,      0, 0, 1,    1,0,    0,1,0,

    // left column back
    0, 0, -.3,       0, 0, -1,   0,1,    0,0,1,
    .3, 0, -.3,      0, 0, -1,   1,1,    0,0,1,
    0, -1.5, -.3,     0, 0, -1,   0,0,    0,0,1,
    0, -1.5, -.3,     0, 0, -1,   0,0,    0,0,1,
    .3, 0, -.3,      0, 0, -1,   1,1,    0,0,1,
    .3, -1.5, -.3,    0, 0, -1,   1,0,    0,0,1,

    // top rung back
    .3, 0, -.3,      0, 0, -1,   0,1,    1,1,0,
    1, 0, -.3,     0, 0, -1,   1,1,    1,1,0,
    .3, -.3, -.3,     0, 0, -1,   0,0,    1,1,0,
    .3, -.3, -.3,     0, 0, -1,   0,0,    1,1,0,
    1, 0, -.3,     0, 0, -1,   1,1,    1,1,0,
    1, -.3, -.3,    0, 0, -1,   1,0,    1,1,0,

    // middle rung back
    .3, -.6, -.3,     0, 0, -1,   0,1,    1,0,1,
    .67, -.6, -.3,     0, 0, -1,   1,1,    1,0,1,
    .3, -.9, -.3,     0, 0, -1,   0,0,    1,0,1,
    .3, -.9, -.3,     0, 0, -1,   0,0,    1,0,1,
    .67, -.6, -.3,     0, 0, -1,   1,1,    1,0,1,
    .67, -.9, -.3,     0, 0, -1,   1,0,    1,0,1,

    // top
    0, 0, 0,        0, 1, 0,    0,0,    0,1,1,
    1, 0, 0,      0, 1, 0,    1,0,    0,1,1,
    1, 0, -.3,     0, 1, 0,    1,1,    0,1,1,
    0, 0, 0,        0, 1, 0,    0,0,    0,1,1,
    1, 0, -.3,     0, 1, 0,    1,1,    0,1,1,
    0, 0, -.3,       0, 1, 0,    0,1,    0,1,1,

    // top rung right
    1, -0, 0,      1, 0, 0,    0,0,    1,1,1,
    1, -.3, 0,     1, 0, 0,    1,0,    1,1,1,
    1, -.3, -.3,    1, 0, 0,    1,1,    1,1,1,
    1, -0, 0,      1, 0, 0,    0,0,    1,1,1,
    1, -.3, -.3,    1, 0, 0,    1,1,    1,1,1,
    1, -0, -.3,     1, 0, 0,    0,1,    1,1,1,

    // under top rung
    .3, -.3, -0,      0, -1, 0,   0,0,     1,1,1,
    .3, -.3, -.3,     0, -1, 0,   0,1,    1,1,1,
    1, -.3, -.3,    0, -1, 0,   1,1,    1,1,1,
    .3, -.3, -0,      0, -1, 0,   0,0,    1,1,1,
    1, -.3, -.3,    0, -1, 0,   1,1,    1,1,1,
    1, -.3, -0,     0, -1, 0,   1,0,    1,1,1,

    // between top rung and middle
    .3, -.3, -0,      1, 0, 0,    0,0,    1,1,1,
    .3, -.6, -.3,     1, 0, 0,    1,1,    1,1,1,
    .3, -.3, -.3,     1, 0, 0,    0,1,    1,1,1,
    .3, -.3, -0,      1, 0, 0,    0,0,    1,1,1,
    .3, -.6, -0,      1, 0, 0,    1,0,    1,1,1,
    .3, -.6, -.3,     1, 0, 0,    1,1,    1,1,1,

    // top of middle rung
    .3, -.6, -0,      0, 1, 0,    0,0,    1,1,1,
    .67, -.6, -.3,     0, 1, 0,    1,1,    1,1,1,
    .3, -.6, -.3,     0, 1, 0,    0,1,    1,1,1,
    .3, -.6, -0,      0, 1, 0,    0,0,    1,1,1,
    .67, -.6, -0,      0, 1, 0,    1,0,    1,1,1,
    .67, -.6, -.3,     0, 1, 0,    1,1,    1,1,1,

    // right of middle rung
    .67, -.6, -0,      1, 0, 0,    0,0,    1,1,1,
    .67, -.9, -.3,     1, 0, 0,    1,1,    1,1,1,
    .67, -.6, -.3,     1, 0, 0,    0,1,    1,1,1,
    .67, -.6, -0,      1, 0, 0,    0,0,    1,1,1,
    .67, -.9, -0,      1, 0, 0,    1,0,    1,1,1,
    .67, -.9, -.3,     1, 0, 0,    1,1,    1,1,1,

    // bottom of middle rung.
    .3, -.9, -0,      0, -1, 0,   0,0,    1,1,1,
    .3, -.9, -.3,     0, -1, 0,   0,1,    1,1,1,
    .67, -.9, -.3,     0, -1, 0,   1,1,    1,1,1,
    .3, -.9, -0,      0, -1, 0,   0,0,    1,1,1,
    .67, -.9, -.3,     0, -1, 0,   1,1,    1,1,1,
    .67, -.9, -0,      0, -1, 0,   1,0,    1,1,1,

    // right of bottom
    .3, -.9, -0,      1, 0, 0,    0,0,    1,1,1,
    .3, -1.5, -.3,    1, 0, 0,    1,1,    1,1,1,
    .3, -.9, -.3,     1, 0, 0,    0,1,    1,1,1,
    .3, -.9, -0,      1, 0, 0,    0,0,    1,1,1,
    .3, -1.5, -0,     1, 0, 0,    1,0,    1,1,1,
    .3, -1.5, -.3,    1, 0, 0,    1,1,    1,1,1,

    // bottom
    0, -1.5, -0,      0, -1, 0,   0,0,    1,1,1,
    0, -1.5, -.3,     0, -1, 0,   0,1,    1,1,1,
    .3, -1.5, -.3,    0, -1, 0,   1,1,    1,1,1,
    0, -1.5, -0,      0, -1, 0,   0,0,    1,1,1,
    .3, -1.5, -.3,    0, -1, 0,   1,1,    1,1,1,
    .3, -1.5, -0,     0, -1, 0,   1,0,    1,1,1,

    // left side
    0, -0, -0,        -1, 0, 0,   0,0,    1,1,1,
    0, -0, -.3,       -1, 0, 0,   0,1,    1,1,1,
    0, -1.5, -.3,     -1, 0, 0,   1,1,    1,1,1,
    0, -0, -0,        -1, 0, 0,   0,0,    1,1,1,
    0, -1.5, -.3,     -1, 0, 0,   1,1,    1,1,1,
    0, -1.5, -0,       -1, 0, 0,   1,0,    1,1,1,
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

   //bind textures
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, tex);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, wally);
   glActiveTexture(GL_TEXTURE0);

   //update time if not paused
   if (!paused)
   {
       time = glfwGetTime();
   }

   //  Select shader
   glUseProgram(shader);
   //  Set time
   int id = glGetUniformLocation(shader, "time");
   glUniform1f(id, time);
   int loc = glGetUniformLocation(shader,"xyz");
   glUniform1i(glGetUniformLocation(shader, "tex"), 0);
   glUniform1i(glGetUniformLocation(shader, "wally"), 1);
   glUniform1f(glGetUniformLocation(shader, "randomInstance"), randomInstance);
   glUniform1f(glGetUniformLocation(shader, "heightCap"), heightCap);
   glUniform3fv(loc,75,xyz);
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
   //glDrawArraysInstanced(GL_TRIANGLES,0,n,125);
   glDrawArraysInstanced(GL_TRIANGLES, 0, n, numInstances);

   //  Disable arrays
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);

   //  Release VBO
   glBindBuffer(GL_ARRAY_BUFFER,0);

   //the ground
   glUseProgram(defaultShader);
   double sceneDimensions = 5;
   double groundEdge = 1;

   glTranslated(0, 0, 0);

   glBindTexture(GL_TEXTURE_2D, grass);//ground texture
   glBegin(GL_QUADS);
   glColor3f(0, 1, 0); //ground color (green)
   for (double len = -sceneDimensions; len < sceneDimensions; len += groundEdge)
   {
       for (double wid = -sceneDimensions; wid < sceneDimensions; wid += groundEdge)
       {
           glTexCoord2f(0, 0); glVertex3f(wid, 0, len);
           glTexCoord2f(1, 0); glVertex3f(wid + groundEdge, 0, len);
           glTexCoord2f(1, 1); glVertex3f(wid + groundEdge, 0, len + groundEdge);
           glTexCoord2f(0, 1); glVertex3f(wid, 0, len + groundEdge);
       }
   }
   glEnd();

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
   //  change the randomInstance wally is assigned to
   else if (key == GLFW_KEY_R)
       randomInstance = (rand() % (numInstances - 1)) + 1;
   //  pause/unpause
   else if (key == GLFW_KEY_T)
       paused = paused ? 0: 1;
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
   GLFWwindow* window = InitWindow("HW9: Kyle Curtis",1,600,600,&reshape,&key);

   //  Load shaders
   shader = CreateShaderProg("instance.vert","instance.frag");
   defaultShader = CreateShaderProg("stored.vert", "stored.frag");
   //  Load textures
   tex = LoadTexBMP("odlaw.bmp");
   wally = LoadTexBMP("wally.bmp");
   grass = LoadTexBMP("grass.bmp");

   //setup random number
   randomInstance = (rand() % (numInstances - 1)) + 1;

   time = glfwGetTime();

   //  Initialize locations
   int k=0;
   for (int x=-2;x<=2;x++)
         for (int z=-2;z<=2;z++)
         {
            xyz[k++] = x+frand(-1,1);
            xyz[k++] = frand(0, heightCap);
            xyz[k++] = z+frand(-1,1);
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
