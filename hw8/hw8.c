/*
 *  Normal Map
 *
 *  Key bindings:
 *  m          Toggle shaders
 *  o          Change objects
 *  s          Toggle light movement
 *  <>+-       Move light
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int mode=0;    // Flat/Normal map
int move=1;    // Moving light
int th=0,ph=0; // View angles
int zh=0;      // Light azimuth
float YL=1;    // Light elevation
int fov=57;    // Field of view (for perspective)
int tex,nml;   // Textures
int obj=0;     // Object
float asp=1;   // Aspect ratio
float dim=3;   // Size of world
int shader=0;  // Shader
//  Vertex buffer objects
unsigned int vbo = 0;   // Vertex buffer objects
const char* text[] = {"Flat","Normal Map in Tangent Space","Normal Map in Model Space"};
const char* normalText[] = { "Plain Wally", "Just Wally", "Whole Picture", "Origional Image" };

//textures and normal maps
const int NUMTEXTURES = 4;
const int NUMNORMALS = 4;
int texNum = 0;
int normalMode = 0;
unsigned int textures[] = { 0,0,0,0};
unsigned int junk[] = { 0,0,0,0 };
unsigned int plain[] = { 0,0,0,0 };
unsigned int whole[] = { 0,0,0,0 };
unsigned int just[] = { 0,0,0,0 };

//  Cube vertex, normal, tangent and texture data
const float vertex[] =
{
//  X  Y  Z   Nx Ny Nz   Tx Ty Tz   s t
   // Front (Texture data corrected)
   -1,-1, 1,   0, 0, 1,   1, 0, 0,  0,0,
    1,-1, 1,   0, 0, 1,   1, 0, 0,  1,0,
    1, 1, 1,   0, 0, 1,   1, 0, 0,  1,1,
    1, 1, 1,   0, 0, 1,   1, 0, 0,  1,1,
   -1, 1, 1,   0, 0, 1,   1, 0, 0,  0,1,
   -1,-1, 1,   0, 0, 1,   1, 0, 0,  0,0,
   // Back
    1,-1,-1,   0, 0,-1,  -1, 0, 0,  0,1,
   -1,-1,-1,   0, 0,-1,  -1, 0, 0,  1,1,
   -1, 1,-1,   0, 0,-1,  -1, 0, 0,  1,0,
   -1, 1,-1,   0, 0,-1,  -1, 0, 0,  1,0,
    1, 1,-1,   0, 0,-1,  -1, 0, 0,  0,0,
    1,-1,-1,   0, 0,-1,  -1, 0, 0,  0,1,
   // Right
    1,-1, 1,   1, 0, 0,   0, 0,-1,  0,1,
    1,-1,-1,   1, 0, 0,   0, 0,-1,  1,1,
    1, 1,-1,   1, 0, 0,   0, 0,-1,  1,0,
    1, 1,-1,   1, 0, 0,   0, 0,-1,  1,0,
    1, 1, 1,   1, 0, 0,   0, 0,-1,  0,0,
    1,-1, 1,   1, 0, 0,   0, 0,-1,  0,1,
   // Left
   -1,-1,-1,  -1, 0, 0,   0, 0, 1,  0,1,
   -1,-1, 1,  -1, 0, 0,   0, 0, 1,  1,1,
   -1, 1, 1,  -1, 0, 0,   0, 0, 1,  1,0,
   -1, 1, 1,  -1, 0, 0,   0, 0, 1,  1,0,
   -1, 1,-1,  -1, 0, 0,   0, 0, 1,  0,0,
   -1,-1,-1,  -1, 0, 0,   0, 0, 1,  0,1,
   // Top
   -1,-1,-1,   0,-1, 0,   1, 0, 0,  0,1,
    1,-1,-1,   0,-1, 0,   1, 0, 0,  1,1,
    1,-1, 1,   0,-1, 0,   1, 0, 0,  1,0,
    1,-1, 1,   0,-1, 0,   1, 0, 0,  1,0,
   -1,-1, 1,   0,-1, 0,   1, 0, 0,  0,0,
   -1,-1,-1,   0,-1, 0,   1, 0, 0,  0,1,
   // Bottom
   -1, 1, 1,   0, 1, 0,   1, 0, 0,  0,1,
    1, 1, 1,   0, 1, 0,   1, 0, 0,  1,1,
    1, 1,-1,   0, 1, 0,   1, 0, 0,  1,0,
    1, 1,-1,   0, 1, 0,   1, 0, 0,  1,0,
   -1, 1,-1,   0, 1, 0,   1, 0, 0,  0,0,
   -1, 1, 1,   0, 1, 0,   1, 0, 0,  0,1,

   //  --------------------------------Cylinder Side--------------------------------
   //  X      Y      Z       Nx     Ny     Nz       Tx    Ty      Tz        s      t
    0.000, 1.000, 1.000,   0.000, 0.000, 1.000,   1.000, 0.000,-0.000,   0.000,-1.000,
    0.000,-1.000, 1.000,   0.000, 0.000, 1.000,   1.000, 0.000,-0.000,   0.000, 1.000,
    0.259, 1.000, 0.966,   0.259, 0.000, 0.966,   0.966, 0.000,-0.259,   0.250,-1.000,
    0.259,-1.000, 0.966,   0.259, 0.000, 0.966,   0.966, 0.000,-0.259,   0.250, 1.000,
    0.500, 1.000, 0.866,   0.500, 0.000, 0.866,   0.866, 0.000,-0.500,   0.500,-1.000,
    0.500,-1.000, 0.866,   0.500, 0.000, 0.866,   0.866, 0.000,-0.500,   0.500, 1.000,
    0.707, 1.000, 0.707,   0.707, 0.000, 0.707,   0.707, 0.000,-0.707,   0.750,-1.000,
    0.707,-1.000, 0.707,   0.707, 0.000, 0.707,   0.707, 0.000,-0.707,   0.750, 1.000,
    0.866, 1.000, 0.500,   0.866, 0.000, 0.500,   0.500, 0.000,-0.866,   1.000,-1.000,
    0.866,-1.000, 0.500,   0.866, 0.000, 0.500,   0.500, 0.000,-0.866,   1.000, 1.000,
    0.966, 1.000, 0.259,   0.966, 0.000, 0.259,   0.259, 0.000,-0.966,   1.250,-1.000,
    0.966,-1.000, 0.259,   0.966, 0.000, 0.259,   0.259, 0.000,-0.966,   1.250, 1.000,
    1.000, 1.000,-0.000,   1.000, 0.000,-0.000,  -0.000, 0.000,-1.000,   1.500,-1.000,
    1.000,-1.000,-0.000,   1.000, 0.000,-0.000,  -0.000, 0.000,-1.000,   1.500, 1.000,
    0.966, 1.000,-0.259,   0.966, 0.000,-0.259,  -0.259, 0.000,-0.966,   1.750,-1.000,
    0.966,-1.000,-0.259,   0.966, 0.000,-0.259,  -0.259, 0.000,-0.966,   1.750, 1.000,
    0.866, 1.000,-0.500,   0.866, 0.000,-0.500,  -0.500, 0.000,-0.866,   2.000,-1.000,
    0.866,-1.000,-0.500,   0.866, 0.000,-0.500,  -0.500, 0.000,-0.866,   2.000, 1.000,
    0.707, 1.000,-0.707,   0.707, 0.000,-0.707,  -0.707, 0.000,-0.707,   2.250,-1.000,
    0.707,-1.000,-0.707,   0.707, 0.000,-0.707,  -0.707, 0.000,-0.707,   2.250, 1.000,
    0.500, 1.000,-0.866,   0.500, 0.000,-0.866,  -0.866, 0.000,-0.500,   2.500,-1.000,
    0.500,-1.000,-0.866,   0.500, 0.000,-0.866,  -0.866, 0.000,-0.500,   2.500, 1.000,
    0.259, 1.000,-0.966,   0.259, 0.000,-0.966,  -0.966, 0.000,-0.259,   2.750,-1.000,
    0.259,-1.000,-0.966,   0.259, 0.000,-0.966,  -0.966, 0.000,-0.259,   2.750, 1.000,
   -0.000, 1.000,-1.000,  -0.000, 0.000,-1.000,  -1.000, 0.000, 0.000,   3.000,-1.000,
   -0.000,-1.000,-1.000,  -0.000, 0.000,-1.000,  -1.000, 0.000, 0.000,   3.000, 1.000,
   -0.259, 1.000,-0.966,  -0.259, 0.000,-0.966,  -0.966, 0.000, 0.259,   3.250,-1.000,
   -0.259,-1.000,-0.966,  -0.259, 0.000,-0.966,  -0.966, 0.000, 0.259,   3.250, 1.000,
   -0.500, 1.000,-0.866,  -0.500, 0.000,-0.866,  -0.866, 0.000, 0.500,   3.500,-1.000,
   -0.500,-1.000,-0.866,  -0.500, 0.000,-0.866,  -0.866, 0.000, 0.500,   3.500, 1.000,
   -0.707, 1.000,-0.707,  -0.707, 0.000,-0.707,  -0.707, 0.000, 0.707,   3.750,-1.000,
   -0.707,-1.000,-0.707,  -0.707, 0.000,-0.707,  -0.707, 0.000, 0.707,   3.750, 1.000,
   -0.866, 1.000,-0.500,  -0.866, 0.000,-0.500,  -0.500, 0.000, 0.866,   4.000,-1.000,
   -0.866,-1.000,-0.500,  -0.866, 0.000,-0.500,  -0.500, 0.000, 0.866,   4.000, 1.000,
   -0.966, 1.000,-0.259,  -0.966, 0.000,-0.259,  -0.259, 0.000, 0.966,   4.250,-1.000,
   -0.966,-1.000,-0.259,  -0.966, 0.000,-0.259,  -0.259, 0.000, 0.966,   4.250, 1.000,
   -1.000, 1.000, 0.000,  -1.000, 0.000, 0.000,   0.000, 0.000, 1.000,   4.500,-1.000,
   -1.000,-1.000, 0.000,  -1.000, 0.000, 0.000,   0.000, 0.000, 1.000,   4.500, 1.000,
   -0.966, 1.000, 0.259,  -0.966, 0.000, 0.259,   0.259, 0.000, 0.966,   4.750,-1.000,
   -0.966,-1.000, 0.259,  -0.966, 0.000, 0.259,   0.259, 0.000, 0.966,   4.750, 1.000,
   -0.866, 1.000, 0.500,  -0.866, 0.000, 0.500,   0.500, 0.000, 0.866,   5.000,-1.000,
   -0.866,-1.000, 0.500,  -0.866, 0.000, 0.500,   0.500, 0.000, 0.866,   5.000, 1.000,
   -0.707, 1.000, 0.707,  -0.707, 0.000, 0.707,   0.707, 0.000, 0.707,   5.250,-1.000,
   -0.707,-1.000, 0.707,  -0.707, 0.000, 0.707,   0.707, 0.000, 0.707,   5.250, 1.000,
   -0.500, 1.000, 0.866,  -0.500, 0.000, 0.866,   0.866, 0.000, 0.500,   5.500,-1.000,
   -0.500,-1.000, 0.866,  -0.500, 0.000, 0.866,   0.866, 0.000, 0.500,   5.500, 1.000,
   -0.259, 1.000, 0.966,  -0.259, 0.000, 0.966,   0.966, 0.000, 0.259,   5.750,-1.000,
   -0.259,-1.000, 0.966,  -0.259, 0.000, 0.966,   0.966, 0.000, 0.259,   5.750, 1.000,
    0.000, 1.000, 1.000,   0.000, 0.000, 1.000,   1.000, 0.000,-0.000,   6.000,-1.000,
    0.000,-1.000, 1.000,   0.000, 0.000, 1.000,   1.000, 0.000,-0.000,   6.000, 1.000,
   //  --------------------------------Cylinder Top---------------------------------
   //  X      Y      Z       Nx     Ny     Nz       Tx    Ty      Tz        s      t
    0.000, 1.000, 0.000,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,   0.000, 0.000,
    0.000, 1.000, 1.000,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,   0.000, 1.000,
    0.259, 1.000, 0.966,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,   0.259, 0.966,
    0.500, 1.000, 0.866,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,   0.500, 0.866,
    0.707, 1.000, 0.707,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,   0.707, 0.707,
    0.866, 1.000, 0.500,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,   0.866, 0.500,
    0.966, 1.000, 0.259,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,   0.966, 0.259,
    1.000, 1.000,-0.000,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,   1.000,-0.000,
    0.966, 1.000,-0.259,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,   0.966,-0.259,
    0.866, 1.000,-0.500,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,   0.866,-0.500,
    0.707, 1.000,-0.707,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,   0.707,-0.707,
    0.500, 1.000,-0.866,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,   0.500,-0.866,
    0.259, 1.000,-0.966,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,   0.259,-0.966,
   -0.000, 1.000,-1.000,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,  -0.000,-1.000,
   -0.259, 1.000,-0.966,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,  -0.259,-0.966,
   -0.500, 1.000,-0.866,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,  -0.500,-0.866,
   -0.707, 1.000,-0.707,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,  -0.707,-0.707,
   -0.866, 1.000,-0.500,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,  -0.866,-0.500,
   -0.966, 1.000,-0.259,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,  -0.966,-0.259,
   -1.000, 1.000, 0.000,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,  -1.000, 0.000,
   -0.966, 1.000, 0.259,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,  -0.966, 0.259,
   -0.866, 1.000, 0.500,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,  -0.866, 0.500,
   -0.707, 1.000, 0.707,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,  -0.707, 0.707,
   -0.500, 1.000, 0.866,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,  -0.500, 0.866,
   -0.259, 1.000, 0.966,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,  -0.259, 0.966,
    0.000, 1.000, 1.000,   0.000, 1.000, 0.000,   1.000, 0.000, 0.000,   0.000, 1.000,
   //  -------------------------------Cylinder Bottom-------------------------------
   //  X      Y      Z       Nx     Ny     Nz       Tx    Ty      Tz        s      t
    0.000,-1.000, 0.000,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,   0.000, 0.000,
   -0.000,-1.000, 1.000,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,  -0.000,-1.000,
   -0.259,-1.000, 0.966,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,  -0.259,-0.966,
   -0.500,-1.000, 0.866,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,  -0.500,-0.866,
   -0.707,-1.000, 0.707,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,  -0.707,-0.707,
   -0.866,-1.000, 0.500,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,  -0.866,-0.500,
   -0.966,-1.000, 0.259,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,  -0.966,-0.259,
   -1.000,-1.000,-0.000,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,  -1.000, 0.000,
   -0.966,-1.000,-0.259,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,  -0.966, 0.259,
   -0.866,-1.000,-0.500,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,  -0.866, 0.500,
   -0.707,-1.000,-0.707,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,  -0.707, 0.707,
   -0.500,-1.000,-0.866,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,  -0.500, 0.866,
   -0.259,-1.000,-0.966,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,  -0.259, 0.966,
    0.000,-1.000,-1.000,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,   0.000, 1.000,
    0.259,-1.000,-0.966,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,   0.259, 0.966,
    0.500,-1.000,-0.866,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,   0.500, 0.866,
    0.707,-1.000,-0.707,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,   0.707, 0.707,
    0.866,-1.000,-0.500,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,   0.866, 0.500,
    0.966,-1.000,-0.259,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,   0.966, 0.259,
    1.000,-1.000, 0.000,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,   1.000,-0.000,
    0.966,-1.000, 0.259,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,   0.966,-0.259,
    0.866,-1.000, 0.500,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,   0.866,-0.500,
    0.707,-1.000, 0.707,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,   0.707,-0.707,
    0.500,-1.000, 0.866,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,   0.500,-0.866,
    0.259,-1.000, 0.966,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,   0.259,-0.966,
   -0.000,-1.000, 1.000,   0.000,-1.000, 0.000,   1.000, 0.000, 0.000,  -0.000,-1.000,
   };

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   //  Set view
   View(th,ph,fov,dim);
   //  Enable lighting
   Lighting(3*Cos(zh),YL,3*Sin(zh) , 0.3,1.0,1.0);

   //  Bind Textures
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,tex);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D,nml);
   glActiveTexture(GL_TEXTURE0);

   //  Select shader
   glUseProgram(shader);
   glUniform1i(glGetUniformLocation(shader,"mode"),mode);
   glUniform1i(glGetUniformLocation(shader,"tex"),0);
   glUniform1i(glGetUniformLocation(shader,"nml"),1);

   //  Define arrays
   glBindBuffer(GL_ARRAY_BUFFER,vbo);
   glVertexPointer(3,GL_FLOAT,44,(void*)0);
   glNormalPointer(GL_FLOAT,44,(void*)12);
   glTexCoordPointer(2,GL_FLOAT,44,(void*)36);
   int loc = glGetAttribLocation(shader,"glTangent");
   glVertexAttribPointer(loc,3,GL_FLOAT,0,44,(void*)24);

   //  Enable arrays
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glEnableVertexAttribArray(loc);

   //  Draw textured cylinder
   glEnable(GL_TEXTURE_2D);
   if (obj)
   {
      glDrawArrays(GL_TRIANGLE_STRIP,36,50); // Side
      glDrawArrays(GL_TRIANGLE_FAN,  86,26); // Top
      glDrawArrays(GL_TRIANGLE_FAN, 112,26); // Bottom
   }
   //  Draw textured cube
   else
      glDrawArrays(GL_TRIANGLES,0,36);
   glDisable(GL_TEXTURE_2D);

   //  Revert to fixed pipeline
   glUseProgram(0);
   glDisable(GL_LIGHTING);

   //  Disable arrays
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableVertexAttribArray(loc);
   glBindBuffer(GL_ARRAY_BUFFER,0);

   //  Display parameters
   SetColor(1,1,1);
   Axes(1.5);
   glWindowPos2i(5, 25);
   Print("Texture=%d, Normal Map = %s", texNum, normalText[normalMode]);
   glWindowPos2i(5,5);
   Print("Angle=%d,%d FPS=%d Dim=%.1f Projection=%s Mode=%s",th,ph,FramesPerSecond(),dim,fov>0?"Perpective":"Orthogonal",text[mode]);
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
   //  Reset view angle and location
   else if (key==GLFW_KEY_0)
      th = ph = 0;
   //  Switch shaders
   else if (key==GLFW_KEY_M && shift)
      mode = (mode+2)%3;
   else if (key==GLFW_KEY_M)
      mode = (mode+1)%3;
   //  Switch objects
   else if (key==GLFW_KEY_O)
      obj = 1-obj;
   //switch textures
   else if (key == GLFW_KEY_T)
   {
       texNum = (texNum + 1) % NUMTEXTURES;//update texNum
       tex = textures[texNum];//update texture
       //update normal
       if (normalMode == 1)//just
       {
           nml = just[texNum];
       }
       else if (normalMode == 2)//whole
       {
           nml = whole[texNum];
       }
       else if (normalMode == 3)//oritional
       {
           nml = junk[texNum];
       }
       else//mode 0, plain
       {
           nml = plain[texNum];
       }
   }
   //switch normal sets
   else if (key == GLFW_KEY_N)
   {
       normalMode = (normalMode + 1) % NUMNORMALS;
       if (normalMode == 1)//just
       {
           nml = just[texNum];
       }
       else if (normalMode == 2)//whole
       {
           nml = whole[texNum];
       }
       else if (normalMode == 3)//oritional
       {
           nml = junk[texNum];
       }
       else//mode 0, plain
       {
           nml = plain[texNum];
       }
   }
   //  Light movement
   else if (key==GLFW_KEY_S)
      move = 1-move;
   else if (key==GLFW_KEY_COMMA)
      zh++;
   else if (key==GLFW_KEY_PERIOD)
      zh--;
   //  Light elevation
   else if (key==GLFW_KEY_KP_SUBTRACT || key==GLFW_KEY_MINUS)
      YL -= 0.05;
   else if (key==GLFW_KEY_KP_ADD || key==GLFW_KEY_EQUAL)
      YL += 0.05;
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
   GLFWwindow* window = InitWindow("Kyle Curtis: HW7",0,600,600,&reshape,&key);

   //  Load shaders
   shader = CreateShaderProg("normal.vert","normal.frag");
   //  Load textures
   //tex = LoadTexBMP("brickwall.bmp");
   //nml = LoadTexBMP("brickwall_normal.bmp");

   //base textures
   //textures[0] = LoadTexBMP("NoWallys/wallysGone1.bmp");
   textures[0] = LoadTexBMP("brickwall.bmp");
   textures[1] = LoadTexBMP("NoWallys/wallysGone2.bmp");
   textures[2] = LoadTexBMP("NoWallys/wallysGone3.bmp");
   textures[3] = LoadTexBMP("NoWallys/wallysGone4.bmp");

   //junk textures (the origional pictures
   junk[0] = LoadTexBMP("Wallys/wallys1.bmp");
   junk[1] = LoadTexBMP("Wallys/wallys2.bmp");
   junk[2] = LoadTexBMP("Wallys/wallys3.bmp");
   junk[3] = LoadTexBMP("Wallys/wallys4.bmp");

   //plain, just blue and a colored wally
   plain[0] = LoadTexBMP("PlainWallys/plainwallys1.bmp");
   plain[1] = LoadTexBMP("PlainWallys/plainwallys2.bmp");
   plain[2] = LoadTexBMP("PlainWallys/plainwallys3.bmp");
   plain[3] = LoadTexBMP("PlainWallys/plainwallys4.bmp");

   //just, convert plain into a normal map
   just[0] = LoadTexBMP("JustWallys/justwallys1.bmp");
   just[1] = LoadTexBMP("JustWallys/justwallys2.bmp");
   just[2] = LoadTexBMP("JustWallys/justwallys3.bmp");
   just[3] = LoadTexBMP("JustWallys/justwallys4.bmp");

   //whole, the entire junk texture turned into a normal map
   whole[0] = LoadTexBMP("WholeWallys/wholewallys1.bmp");
   whole[1] = LoadTexBMP("WholeWallys/wholewallys2.bmp");
   whole[2] = LoadTexBMP("WholeWallys/wholewallys3.bmp");
   whole[3] = LoadTexBMP("WholeWallys/wholewallys4.bmp");

   //
   tex = textures[0];
   //nml = LoadTexBMP("NormalWallys/wallys1Normal.bmp");
   nml = plain[0];

   //  Copy vertex data to VBO
   glGenBuffers(1,&vbo);
   glBindBuffer(GL_ARRAY_BUFFER,vbo);
   glBufferData(GL_ARRAY_BUFFER,sizeof(vertex),vertex,GL_STATIC_DRAW);

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Light animation
      if (move) zh = fmod(90*glfwGetTime(),360);
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
