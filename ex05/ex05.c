/*
 *  Shadow Map with Shaders
 *    with Frame Buffer Depth Texture by Jay Kominek
 *
 *  Demonstate shadows using the shadow map algorithm using a shader.
 *
 *  Key bindings:
 *  m/M        Show/hide shadow map
 *  o/O        Cycle through objects
 *  +/-        Change light elevation
 *  []         Change light position
 *  s/S        Start/stop light movement
 *  <>         Decrease/increase number of slices in objects
 *  b/B        Toggle room box
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
typedef struct {float x,y,z;} Point;
//  Global variables
int          mode=0;    // Display mode
int          obj=15;    // Display objects (bitmap)
int          move=1;    // Light movement
int          box=0;     // Display enclosing box
int          th=-30;    // Azimuth of view angle
int          ph=+30;    // Elevation of view angle
int          tex2d[3];  // Textures (names)
float        dim=3;     // Size of world
int          zh=0;      // Light azimuth
int          n=4;       // Number of slices
float        YL=2;      // Elevation of light
float        Lpos[4];   // Light position
unsigned int framebuf=0;// Frame buffer id
double       Svec[4];   // Texture planes S
double       Tvec[4];   // Texture planes T
double       Rvec[4];   // Texture planes R
double       Qvec[4];   // Texture planes Q
int          shadowdim; // Size of shadow map textures
int          shader;    // Shader
const char* text[]={"Shadows","Shadow Map"};

#define MAXN 64    // Maximum number of slices (n) and points in a polygon

//
//  Draw a cube
//
static void ShadowCube(float x,float y,float z , float th,float ph , float D)
{
   Cube(x,y,z , D,D,D , th,ph , tex2d[2]);
}

//
//  Draw a cylinder
//
static void ShadowCylinder(float x,float y,float z , float th,float ph , float R,float H)
{
   SetColor(0,1,1);
   Cylinder(x,y,z , R,H , th,ph , 4*n,tex2d[2]);
}

//
//  Draw torus
//
static void ShadowTorus(float x,float y,float z , float th,float ph , float R,float r)
{
   SetColor(1,1,0);
   Torus(x,y,z , R,r , th,ph , 4*n,tex2d[2]);
}

//
//  Draw teapot
//
static void ShadowTeapot(float x,float y,float z , float th,float ph , float S)
{
   SetColor(0,1,0);
   Teapot(x,y+0.5,z,0.5 , th,ph , n,tex2d[2]);
}

//
//  Draw a wall
//
static void Wall(float x,float y,float z, float th,float ph , float Sx,float Sy,float Sz , float St)
{
   //  Transform
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   glScaled(Sx,Sy,Sz);

   //  Draw walls
   float s=1.0/n;
   float t=0.5*St/n;
   glNormal3f(0,0,1);
   for (int j=-n;j<n;j++)
   {
      glBegin(GL_QUAD_STRIP);
      for (int i=-n;i<=n;i++)
      {
         glTexCoord2f((i+n)*t,(j  +n)*t); glVertex3f(i*s,    j*s,-1);
         glTexCoord2f((i+n)*t,(j+1+n)*t); glVertex3f(i*s,(j+1)*s,-1);
      }
      glEnd();
   }

   //  Restore
   glPopMatrix();
}

//
//  Set light
//    light>0 bright
//    light<0 dim
//    light=0 off
//
static void Light(int light)
{
   //  Set light position
   Lpos[0] = 2*Cos(zh);
   Lpos[1] = YL;
   Lpos[2] = 2*Sin(zh);
   Lpos[3] = 1;

   //  Enable lighting
   if (light)
   {
      float Med[]  = {0.3,0.3,0.3,1.0};
      float High[] = {1.0,1.0,1.0,1.0};
      //  Enable lighting with normalization
      glEnable(GL_LIGHTING);
      glEnable(GL_NORMALIZE);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0,GL_POSITION,Lpos);
      glLightfv(GL_LIGHT0,GL_AMBIENT,Med);
      glLightfv(GL_LIGHT0,GL_DIFFUSE,High);
   }
   else
   {
      glDisable(GL_LIGHTING);
      glDisable(GL_COLOR_MATERIAL);
      glDisable(GL_NORMALIZE);
   }
}

//
//  Draw scene
//    light (true enables lighting)
//
void Scene(int light)
{
   //  Set light position and properties
   Light(light);
 
   //  Enable textures if lit
   if (light)
   {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,tex2d[2]);
   }

   //  Draw objects         x    y   z          th,ph    dims   
   if (obj&0x01)     ShadowCube(-0.8,+0.8,0.0 , -0.25*zh, 0  , 0.3    );
   if (obj&0x02) ShadowCylinder(+0.8,+0.5,0.0 ,   0.5*zh,zh  , 0.2,0.5);
   if (obj&0x04)    ShadowTorus(+0.5,-0.8,0.0 ,        0,zh  , 0.5,0.1);
   if (obj&0x08)   ShadowTeapot(-0.5,-0.5,0.0 ,     2*zh, 0  , 0.5    );

   //  Disable textures
   if (light) glDisable(GL_TEXTURE_2D);

   //  The floor, ceiling and walls don't cast a shadow, so bail here
   if (!light) return;

   //  Enable textures for floor, ceiling and walls
   glEnable(GL_TEXTURE_2D);

   //  Water texture for floor and ceiling
   glBindTexture(GL_TEXTURE_2D,tex2d[0]);
   SetColor(1,1,1);
   for (int k=-1;k<=box;k+=2)
      Wall(0,0,0, 0,90*k , 8,8,box?6:2 , 4);
   //  Crate texture for walls
   glBindTexture(GL_TEXTURE_2D,tex2d[1]);
   for (int k=0;k<4*box;k++)
      Wall(0,0,0, 90*k,0 , 8,box?6:2,8 , 1);

   //  Disable textures
   glDisable(GL_TEXTURE_2D);
}

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Eye position
   float Ex = -2*dim*Sin(th)*Cos(ph);
   float Ey = +2*dim        *Sin(ph);
   float Ez = +2*dim*Cos(th)*Cos(ph);

   //  Erase the window and the depth buffers
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Disable lighting
   glDisable(GL_LIGHTING);

   //
   //  Draw the scene with shadows
   //
   int width,height;
   glfwGetFramebufferSize(window,&width,&height);
   float asp = width/(float)height;
   //  Set perspective view
   if (mode)
   {
      //  Half width for shadow map display
      Projection(60,asp/2,dim);
      glViewport(0,0,width/2,height);
   }
   else
   {
      //  Full width
      Projection(60,asp,dim);
      glViewport(0,0,width,height);
   }
   gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);

   //  Draw light position as sphere (still no lighting here)
   SetColor(1,1,1);
   Sphere(Lpos[0],Lpos[1],Lpos[2],0.03,0,8,0);

   //  Enable shader program
   glUseProgram(shader);
   int id = glGetUniformLocation(shader,"tex");
   glUniform1i(id,0);
   id = glGetUniformLocation(shader,"depth");
   glUniform1i(id,1);

   // Set up the eye plane for projecting the shadow map on the scene
   glActiveTexture(GL_TEXTURE1);
   glTexGendv(GL_S,GL_EYE_PLANE,Svec);
   glTexGendv(GL_T,GL_EYE_PLANE,Tvec);
   glTexGendv(GL_R,GL_EYE_PLANE,Rvec);
   glTexGendv(GL_Q,GL_EYE_PLANE,Qvec);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_COMPARE_R_TO_TEXTURE);
   glActiveTexture(GL_TEXTURE0);

   // Draw objects in the scene (including walls)
   Scene(1);

   //  Disable shader program
   glUseProgram(0);

   //  Draw axes
   Axes(2);

   //
   //  Show the shadow map
   //
   if (mode)
   {
      int k,ix=width/2+5,iy=height-5;
      //  Orthogonal view (right half)
      Projection(0,asp/2,1);
      glViewport(width/2+1,0,width/2,height);
      //  Disable any manipulation of textures
      glActiveTexture(GL_TEXTURE1);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_NONE);
      //  Display texture by drawing quad
      glEnable(GL_TEXTURE_2D);
      SetColor(1,1,1);
      glBegin(GL_QUADS);
      glMultiTexCoord2f(GL_TEXTURE1,0,0);glVertex2f(-1,-1);
      glMultiTexCoord2f(GL_TEXTURE1,1,0);glVertex2f(+1,-1);
      glMultiTexCoord2f(GL_TEXTURE1,1,1);glVertex2f(+1,+1);
      glMultiTexCoord2f(GL_TEXTURE1,0,1);glVertex2f(-1,+1);
      glEnd();
      glDisable(GL_TEXTURE_2D);
      //  Switch back to default texture unit
      glActiveTexture(GL_TEXTURE0);
      //  Show buffer info
      SetColor(1,0,0);
      glWindowPos2i(ix,iy-=20);
      glGetIntegerv(GL_MAX_TEXTURE_UNITS,&k);
      Print("Maximum Texture Units %d",k);
      glGetIntegerv(GL_MAX_TEXTURE_SIZE,&k);
      glWindowPos2i(ix,iy-=20);
      Print("Maximum Texture Size %d",k);
      glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,&k);
      glWindowPos2i(ix,iy-=20);
      Print("Maximum Buffer Size  %d",k);
      glWindowPos2i(ix,iy-=20);
      Print("Shadow Texture Size %dx%d",shadowdim,shadowdim);
   }

   //  Display parameters
   SetColor(1,1,1);
   glWindowPos2i(5,5);
   Print("Angle=%d,%d,%d  Dim=%.1f Slices=%d Mode=%s",
     th,ph,zh,dim,n,text[mode]);

   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glfwSwapBuffers(window);
}

//
//  Build Shadow Map
//
void ShadowMap(void)
{
   double Lmodel[16];  //  Light modelview matrix
   double Lproj[16];   //  Light projection matrix
   double Tproj[16];   //  Texture projection matrix
   double Dim=2.0;     //  Bounding radius of scene
   double Ldist;       //  Distance from light to scene center

   //  Save transforms and modes
   glPushMatrix();
   glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
   //  No write to color buffer and no smoothing
   glShadeModel(GL_FLAT);
   glColorMask(0,0,0,0);
   // Overcome imprecision
   glEnable(GL_POLYGON_OFFSET_FILL);

   //  Turn off lighting and set light position
   Light(0);

   //  Light distance
   Ldist = sqrt(Lpos[0]*Lpos[0] + Lpos[1]*Lpos[1] + Lpos[2]*Lpos[2]);
   if (Ldist<1.1*Dim) Ldist = 1.1*Dim;

   //  Set perspective view from light position
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(114.6*atan(Dim/Ldist),1,Ldist-Dim,Ldist+Dim);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(Lpos[0],Lpos[1],Lpos[2] , 0,0,0 , 0,1,0);
   //  Size viewport to desired dimensions
   glViewport(0,0,shadowdim,shadowdim);

   // Redirect traffic to the frame buffer
   glBindFramebuffer(GL_FRAMEBUFFER,framebuf);

   // Clear the depth buffer
   glClear(GL_DEPTH_BUFFER_BIT);
   // Draw all objects that can cast a shadow
   Scene(0);

   //  Retrieve light projection and modelview matrices
   glGetDoublev(GL_PROJECTION_MATRIX,Lproj);
   glGetDoublev(GL_MODELVIEW_MATRIX,Lmodel);

   // Set up texture matrix for shadow map projection,
   // which will be rolled into the eye linear
   // texture coordinate generation plane equations
   glLoadIdentity();
   glTranslated(0.5,0.5,0.5);
   glScaled(0.5,0.5,0.5);
   glMultMatrixd(Lproj);
   glMultMatrixd(Lmodel);

   // Retrieve result and transpose to get the s, t, r, and q rows for plane equations
   glGetDoublev(GL_MODELVIEW_MATRIX,Tproj);
   Svec[0] = Tproj[0];    Tvec[0] = Tproj[1];    Rvec[0] = Tproj[2];    Qvec[0] = Tproj[3];
   Svec[1] = Tproj[4];    Tvec[1] = Tproj[5];    Rvec[1] = Tproj[6];    Qvec[1] = Tproj[7];
   Svec[2] = Tproj[8];    Tvec[2] = Tproj[9];    Rvec[2] = Tproj[10];   Qvec[2] = Tproj[11];
   Svec[3] = Tproj[12];   Tvec[3] = Tproj[13];   Rvec[3] = Tproj[14];   Qvec[3] = Tproj[15];

   // Restore normal drawing state
   glShadeModel(GL_SMOOTH);
   glColorMask(1,1,1,1);
   glDisable(GL_POLYGON_OFFSET_FILL);
   glPopAttrib();
   glPopMatrix();
   glBindFramebuffer(GL_FRAMEBUFFER,0);

   //  Check if something went wrong
   ErrCheck("ShadowMap");
}

//
//  Initialize shadow map
//
void InitShadowMap()
{
   unsigned int shadowtex; //  Shadow buffer texture id
   int n;

   //  Make sure multi-textures are supported
   glGetIntegerv(GL_MAX_TEXTURE_UNITS,&n);
   if (n<2) Fatal("Multiple textures not supported\n");

   //  Get maximum texture buffer size
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,&shadowdim);
   //  Limit texture size to maximum buffer size
   glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,&n);
   if (shadowdim>n) shadowdim = n;
   //  Limit texture size to 2048 for performance
   if (shadowdim>2048) shadowdim = 2048;
   if (shadowdim<512) Fatal("Shadow map dimensions too small %d\n",shadowdim);

   //  Do Shadow textures in MultiTexture 1
   glActiveTexture(GL_TEXTURE1);

   //  Allocate and bind shadow texture
   glGenTextures(1,&shadowtex);
   glBindTexture(GL_TEXTURE_2D,shadowtex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowdim, shadowdim, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

   //  Map single depth value to RGBA (this is called intensity)
   glTexParameteri(GL_TEXTURE_2D,GL_DEPTH_TEXTURE_MODE,GL_INTENSITY);

   //  Set texture mapping to clamp and linear interpolation
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

   //  Set automatic texture generation mode to Eye Linear
   glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);

   // Switch back to default textures
   glActiveTexture(GL_TEXTURE0);

   // Attach shadow texture to frame buffer
   glGenFramebuffers(1,&framebuf);
   glBindFramebuffer(GL_FRAMEBUFFER,framebuf);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowtex, 0);
   //  Don't write or read to visible color buffer
   glDrawBuffer(GL_NONE);
   glReadBuffer(GL_NONE);
   //  Make sure this all worked
   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) Fatal("Error setting up frame buffer\n");
   glBindFramebuffer(GL_FRAMEBUFFER,0);

   //  Check if something went wrong
   ErrCheck("InitShadowMap");

   //  Create shadow map
   ShadowMap();
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
   else if (key==GLFW_KEY_M)
      mode = 1-mode;
   //  Light movement
   else if (key==GLFW_KEY_S)
      move = 1-move;
   //  Light elevation
   else if (key==GLFW_KEY_KP_SUBTRACT || key==GLFW_KEY_MINUS)
      YL -= 0.05;
   else if (key==GLFW_KEY_KP_ADD || key==GLFW_KEY_EQUAL)
      YL += 0.05;
   //  Draw walls
   else if (key==GLFW_KEY_B)
      box = 1-box;
   //  Switch objects
   else if (key==GLFW_KEY_O)
      obj = 1-obj;
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
   //  Change slices
   else if (key==GLFW_KEY_COMMA && shift && n>1)
      n--;
   else if (key==GLFW_KEY_PERIOD && shift)
      n++;
   // Move light
   else if (key==GLFW_KEY_RIGHT_BRACKET)
      zh += 1;
   else if (key==GLFW_KEY_LEFT_BRACKET)
      zh -= 1;
   //  PageUp key - increase dim
   else if (key==GLFW_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key==GLFW_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
   //  Wrap angles
   th %= 360;
   ph %= 360;
   //  Change window size
   if (key==GLFW_KEY_M)
   {
      int width,height;
      glfwGetWindowSize(window,&width,&height);
      width = mode ? 2*width : width/2;
      glfwSetWindowSize(window,width,height);
   }
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   //  No reshape - set projection in display depending on mode
   GLFWwindow* window = InitWindow("Shadows",1,600,600,NULL,&key);

   //  Load textures
   tex2d[0] = LoadTexBMP("water.bmp");
   tex2d[1] = LoadTexBMP("crate.bmp");
   tex2d[2] = LoadTexBMP("pi.bmp");
   // Enable Z-buffer
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glPolygonOffset(2,2);
   //  Load shaders
   shader = CreateShaderProg("shadow.vert","shadow.frag");
   //  Initialize shadow texture
   InitShadowMap();

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Light animation
      if (move) zh = fmod(90*glfwGetTime(),1440);
      //  Calculate shadow map
      ShadowMap();
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
