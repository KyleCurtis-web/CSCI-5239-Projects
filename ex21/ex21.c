/*
 *  Geodesic Sphere
 *  This program requires OpenGL 4.0 or above
 *
 *  Demonstrate a tesselation shader by drawing a sphere
 *  from an icosahedron.
 *
 *  Based on The Little Grashopper tutorial
 *  http://prideout.net/blog/?p=48
 *
 *  Key bindings:
 *  +/-        Increase/decrease inner and outer
 *  i/I        Increase/decrease inner
 *  o/O        Increase/decrease outer
 *  r          Reset inner/outer levels
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int zh=0;         //  Light angle
int shader=0;     //  Shader
int N;            //  Number of faces
int Inner=3;      //  Tesselation inner level
int Outer=2;      //  Tesselation outer level
int move=1;       //  Move light
float asp=1;      //  Screen aspect ratio
float dim=2;      //  World dimension
float elv=-10;    //  Light elevation
unsigned int vao; //  Icosahedron VAO

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Orthogonal projection matrix
   float project[16];
   mat4identity(project);
   mat4ortho(project , -dim*asp, +dim*asp, -dim, +dim, -dim, +dim);
   //  Modelview matrix
   float modelview[16];
   mat4identity(modelview);
   mat4rotate(modelview , ph,1,0,0);
   mat4rotate(modelview , th,0,1,0);
   //  Normal matrix
   float normal[9];
   mat3normalMatrix(modelview , normal);

   //  Set shader
   glUseProgram(shader);

   //  Lighting parameters
   int id = glGetUniformLocation(shader,"LightDir");
   glUniform3f(id,Cos(zh),0.1*elv,Sin(zh));

   //  Set transformation matrixes
   id = glGetUniformLocation(shader,"Projection");
   glUniformMatrix4fv(id,1,0,project);
   id = glGetUniformLocation(shader,"Modelview");
   glUniformMatrix4fv(id,1,0,modelview);
   id = glGetUniformLocation(shader,"NormalMatrix");
   glUniformMatrix3fv(id,1,0,normal);

   //  Controls for tesselation level
   id = glGetUniformLocation(shader,"Inner");
   glUniform1f(id,Inner);
   id = glGetUniformLocation(shader,"Outer");
   glUniform1f(id,Outer);

   // Render the scene
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glClearColor(0.2,0.2,0.2,1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glPatchParameteri(GL_PATCH_VERTICES,3);

   //  Bind VAO and render
   glBindVertexArray(vao);
   glDrawElements(GL_PATCHES,N,GL_UNSIGNED_INT,0);

   //  Unset shader
   glUseProgram(0);

#ifndef APPLE_GL4
   //  Display parameters
   SetColor(1,1,1);
   glWindowPos2i(5,5);
   Print("FPS=%d Angle=%d,%d Inner=%d Outer=%d",
      FramesPerSecond(),th,ph,Inner,Outer);
#endif
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
   //  Reset view angle
   else if (key == GLFW_KEY_0)
      th = ph = 0;
   //  Toggle movement
   else if (key == GLFW_KEY_S)
      move = 1-move;
   //  Move light
   else if (key == GLFW_KEY_LEFT_BRACKET && !shift)
      zh += 5;
   else if (key == GLFW_KEY_RIGHT_BRACKET && !shift)
      zh -= 5;
   //  Move light
   else if (key == GLFW_KEY_LEFT_BRACKET && shift)
      elv++;
   else if (key == GLFW_KEY_RIGHT_BRACKET && shift)
      elv--;
   //  Reset levels
   else if (key == GLFW_KEY_R)
   {
      Inner = 3;
      Outer = 2;
   }
   //  Both levels
   else if (key==GLFW_KEY_KP_ADD || key==GLFW_KEY_EQUAL)
   {
      Inner++;
      Outer++;
   }
   else if ((key==GLFW_KEY_KP_SUBTRACT || key==GLFW_KEY_MINUS) && N>1)
   {
      Inner--;
      Outer--;
   }
   //  Inner level
   else if (key == GLFW_KEY_I)
      Inner += shift ? -1 : +1;
   //  Outer level
   else if (key == GLFW_KEY_O)
      Outer += shift ? -1 : +1;
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

   //  Limit tessalation level
   if (Inner<1) Inner = 1;
   if (Outer<1) Outer = 1;
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
   asp = (height>0) ? width/(double)height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
}

//
//  Create Shader Program
//
int CreateShaderProgTess(void)
{
   //  Create program
   int prog = glCreateProgram();
   //  Compile shaders
   CreateShader(prog,GL_VERTEX_SHADER         ,"geodesic.vert");
   CreateShader(prog,GL_TESS_CONTROL_SHADER   ,"geodesic.tcs");
   CreateShader(prog,GL_TESS_EVALUATION_SHADER,"geodesic.tes");
   CreateShader(prog,GL_GEOMETRY_SHADER       ,"geodesic.geom");
   CreateShader(prog,GL_FRAGMENT_SHADER       ,"geodesic.frag");
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

//
//  Icosahedron data
//    We don't need normals because we known this is an inscribed
//    icosahedron so the normals are the same as for a sphere.
//
static void CreateIcosahedron()
{
   unsigned int verts,faces;
   const int Faces[] =
      {
       2,1,0,    3,2,0,    4,3,0,    5, 4, 0,    1, 5, 0,
      11,6,7,   11,7,8,   11,8,9,   11, 9,10,   11,10, 6,
       1,2,6,    2,3,7,    3,4,8,    4, 5, 9,    5, 1,10,
       2,7,6,    3,8,7,    4,9,8,    5,10, 9,    1, 6,10
      };
   const float Verts[] =
      {
       0.000, 0.000, 1.000,
       0.894, 0.000, 0.447,
       0.276, 0.851, 0.447,
      -0.724, 0.526, 0.447,
      -0.724,-0.526, 0.447,
       0.276,-0.851, 0.447,
       0.724, 0.526,-0.447,
      -0.276, 0.851,-0.447,
      -0.894, 0.000,-0.447,
      -0.276,-0.851,-0.447,
       0.724,-0.526,-0.447,
       0.000, 0.000,-1.000
      };
   N = sizeof(Faces)/sizeof(int);

   //  Position attribute
   glUseProgram(shader);
   int loc = glGetAttribLocation(shader,"Position");

   // Create the VAO:
   glGenVertexArrays(1,&vao);
   glBindVertexArray(vao);

   // Create the VBO for facet indices
   glGenBuffers(1,&faces);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,faces);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(Faces),Faces,GL_STATIC_DRAW);

   // Create the VBO for coordinates
   glGenBuffers(1,&verts);
   glBindBuffer(GL_ARRAY_BUFFER,verts);
   glBufferData(GL_ARRAY_BUFFER,sizeof(Verts),Verts,GL_STATIC_DRAW);
   // Enable Position as vertex array
   glEnableVertexAttribArray(loc);
   glVertexAttribPointer(loc,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);

   glUseProgram(0);
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("Geodesic Tessalation Shader",1,600,600,&reshape,&key);

   //  Shader program
   shader = CreateShaderProgTess();
   CreateIcosahedron();

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
