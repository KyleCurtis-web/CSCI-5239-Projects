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
int mode=3;    // Flat/Normal map
int move=1;    // Moving light
int th=0,ph=0; // View angles
int zh=0;      // Light azimuth
float YL=1;    // Light elevation
int fov=57;    // Field of view (for perspective)
int tex,nml,hgt;   // Textures
int obj=0;     // Object
float asp=1;   // Aspect ratio
float dim=3;   // Size of world
int shader=0;  // Shader
//  Vertex buffer objects
unsigned int vbo = 0;   // Vertex buffer objects
const char* text[] = {"Flat","Normal Map in Tangent Space","Normal Map in Model Space", "Height Map"};
const char* textureText[] = {"Bricks", "Wood", "Stucco", "Wally"};

//textures and normal maps
const int NUMTEXTURES = 4;
const int NUMNORMALS = 4;
int texNum = 0;
unsigned int textures[] = { 0,0,0,0};
unsigned int normals[] = { 0,0,0,0 };
unsigned int heights[] = { 0,0,0,0 };

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
   glActiveTexture(GL_TEXTURE2);
   glBindTexture(GL_TEXTURE_2D, hgt);
   glActiveTexture(GL_TEXTURE0);


   //  Select shader
   glUseProgram(shader);
   glUniform1i(glGetUniformLocation(shader,"mode"),mode);
   glUniform1i(glGetUniformLocation(shader,"tex"),0);
   glUniform1i(glGetUniformLocation(shader,"nml"),1);
   glUniform1i(glGetUniformLocation(shader, "hgt"), 2);

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
   Print("Mode=%s Texture=%s", text[mode], textureText[texNum]);
   glWindowPos2i(5,5);
   Print("Angle=%d,%d FPS=%d Dim=%.1f Projection=%s",th,ph,FramesPerSecond(),dim,fov>0?"Perpective":"Orthogonal");
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
      mode = (mode+2)%4;
   else if (key==GLFW_KEY_M)
      mode = (mode+1)%4;
   //  Switch objects
   else if (key==GLFW_KEY_O)
      obj = 1-obj;
   //switch textures
   else if (key == GLFW_KEY_T)
   {
       texNum = (texNum + 1) % NUMTEXTURES;//update texNum
       tex = textures[texNum];//update texture
       //update normal
       nml = normals[texNum];
       hgt = heights[texNum];
        
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
   GLFWwindow* window = InitWindow("Kyle Curtis: HW8",0,600,600,&reshape,&key);

   //  Load shaders
   shader = CreateShaderProg("normal.vert","normal.frag");
   //  Load textures
   //tex = LoadTexBMP("brickwall.bmp");
   //nml = LoadTexBMP("brickwall_normal.bmp");

   //base textures
   textures[0] = LoadTexBMP("bricks_diffuse.bmp");
   textures[1] = LoadTexBMP("wood_diffuse.bmp");
   textures[2] = LoadTexBMP("stucco_diffuse.bmp");
   textures[3] = LoadTexBMP("wallysGone1.bmp");
   //textures[3] = LoadTexBMP("NoWallys/wallysGone4.bmp");

   //normal maps
   normals[0] = LoadTexBMP("bricks_normal.bmp");
   normals[1] = LoadTexBMP("wood_normal.bmp");
   normals[2] = LoadTexBMP("stucco_normal.bmp");
   normals[3] = LoadTexBMP("wholewallys1.bmp");

   //height maps
   heights[0] = LoadTexBMP("bricks_disp.bmp");
   heights[1] = LoadTexBMP("wood_height.bmp");
   heights[2] = LoadTexBMP("stucco_height.bmp");
   heights[3] = LoadTexBMP("height1.bmp");
   //initial values
   tex = textures[0];
   nml = normals[0];
   hgt = heights[0];

   //tex = LoadTexBMP("bricks_diffuse.bmp");
   //nml = LoadTexBMP("bricks_normal.bmp");
   //hgt = LoadTexBMP("bricks_disp.bmp");


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
