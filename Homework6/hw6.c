/*
 *  Peter Tran Huynh
 *  Texture & Lighting
 *
 *  Displays a scene of 3D objects with basic lighting.
 *  Used Example 13 for Lighting and Example 14-18 for Textures
 *
 *  Key bindings:
 *  `             Reset all of the display to initial startup settings
 *  1             Reset view angle to y,z
 *  2             Reset view angle to x,z
 *  3             Reset view angle to x,y
 *  4             Reset to default zoom
 *
 *  5 / %         Lower/rise light
 *  6 / ^         Decrease/increase ambient light
 *  7 / &         Decrease/increase diffuse light
 *  8 / *         Decrease/increase specular light
 *  9 / (         Decrease/increase emitted light
 *  0 / )         Decrease/increase shininess
 *
 *  q             Change View Mode
 *  w             Zoom In
 *  s             Zoom Out
 *  a             Manually move light clockwise
 *  d             Manually move light counter clockwise
 *  e             Toggle axes
 *  r             Increase Field of View
 *  f             Decrease Field of View
 *  arrows        Change view angle
 *
 *  z             Toggle Lighting
 *  x             Toggle Light Movement
 *
 *  c             Toggle between First Person Mode
 *  WASD          Basic World Movement in First Person Mode
 *
 *  F1            Toggle smooth/flat shading
 *  F2            Toggle local viewer mode
 *  F3            Toggle light distance (1/5)
 *  F8            Change ball increment
 *  F9            Invert bottom normal
 *
 *  mouse motion  Change view angle, dynamically
 *  mouse scroll  Changes Zoom value, dynamically
 *  mouse left    Cycle through previous set of objects
 *  mouse right   Cycle through next set of objects
 *  mouse middle  Reset position
 *  ESC    Exit
 */
 
// Using Class Lib
#include "CSCIx229.h"
 
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//  Define GLUT wheels
#if !defined(GLUT_WHEEL_UP)
# define GLUT_WHEEL_UP 3
# define GLUT_WHEEL_DOWN 4
#endif

/*  Constants  */
//  PI as a default constant.
#define PI 3.14159265358979323846
#define PI_LESSER 3.1415927
#define TWOPI (2*M_PI)
//  Macro for Cosine and Sine in degrees
//#define Cos(x) (cos((x) * (PI_LESSER / 180)))
//#define Sin(x) (sin((x) * (PI_LESSER / 180)))

/*  Globals  */
//GLuint theTorus;      //  GL unsigned integer for torus donut, array list
int th        =   0;  //  Azimuth of view angle
int ph        =   0;  //  Elevation of view angle
int axes      =   1;  //  Display axes
int mode      =   0;  //  Projection Mode
int fov       =  55;  //  Field of view (for perspective)
double asp    =   1;  //  Aspect ratio
double dim    =   3;  //  Dimension of viewing box
// Light values, From Example 13
int move      =   1;  //  Move light
int light     =   1;  //  Lighting
int one       =   1;  //  Unit value
int distance  =   5;  //  Light distance
int inc       =  10;  //  Ball increment
int smooth    =   1;  //  Smooth/Flat shading
int local     =   0;  //  Local Viewer Model
int emission  =   0;  //  Emission intensity (%)
int ambient   =  30;  //  Ambient intensity (%)
int diffuse   = 100;  //  Diffuse intensity (%)
int specular  =   0;  //  Specular intensity (%)
int shininess =   0;  //  Shininess (power of two)
float shiny   =   1;  //  Shininess (value)
int zh        =  90;  //  Light azimuth, and object rotation
float ylight  =   0;  //  Elevation of light

// Used to determine mouse movement
int oldX = 0, oldY = 0;
// First Person Check and Rotation Variable
int firstPerson = 0;
// First Person View Coordinates
double povX = 0, povY = 0, povZ = 2.5;
// Camera Orientation
double cameraX = 0, cameraY = 0, cameraZ = 0;

// Donut Texture
char* pinkTexture = "donut_pink.bmp";
char* chocoTexture = "donut_chocolate.bmp";
char* sugarTexture = "donut_sugar.bmp";
unsigned int pink, choco, sugar;

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  // Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/* ********************************************************************** */

/*
 *  Draw vertex in polar coordinates with normal, From Example 13
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

/*
 *  Draw a ball, From Example 13
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

/* 
 *  Got this from OpenGL site: https://www.opengl.org/archives/resources/code/samples/redbook/torus.c
 *  Draw a torus DONUT
 *
 */
static void torus(int ringCurves, int ringEdges, double piValue, 
                 double xSize, double ySize, double zSize, 
                 double xRadius, double yRadius,
                 double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int texture)
{
   int i, j, k;
   double torusX, torusY, torusZ, xCenter, yCenter;
   
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,0,1);
   glScaled(dx,dy,dz);

   //  Set texture and face positions
   glEnable(GL_TEXTURE_2D);
   glFrontFace(GL_CW);
   
   // Picks the texture to use
   if(texture == 1) glBindTexture(GL_TEXTURE_2D, pink);
   else if(texture == 2) glBindTexture(GL_TEXTURE_2D, choco);
   else glBindTexture(GL_TEXTURE_2D, sugar);
   
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   // Tested Other methods of applying textures. These work, but not as effectively as setting faces.
   //glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
   //glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
   //glEnable(GL_TEXTURE_GEN_S);
   //glEnable(GL_TEXTURE_GEN_T);
   
   //  Torus
   for (i = 0; i < ringCurves; i++)
   {
      glBegin(GL_QUAD_STRIP);
      for (j = 0; j <= ringEdges; j++)
      {
         for (k = 1; k >= 0; k--)
         {
            xCenter = cos(((i + k) % ringCurves + 0.5) * piValue / ringCurves) * xRadius;
	        yCenter = sin(((i + k) % ringCurves + 0.5) * piValue / ringCurves) * yRadius;
            torusY = (ySize + yRadius * cos(((i + k) % ringCurves + 0.5) * piValue / ringCurves)) * sin((j % (ringEdges + 1)) * piValue / ringEdges);
            torusX = (xSize + xRadius * cos(((i + k) % ringCurves + 0.5) * piValue / ringCurves)) * cos((j % (ringEdges + 1)) * piValue / ringEdges);
            torusZ = zSize * sin(((i + k) % ringCurves + 0.5) * piValue / ringCurves);

			// For texture mapping
            glTexCoord2d((i + k) / (float) ringCurves, (j % (ringEdges + 1)) / (float) ringEdges);
			
			// Draws object and sets normals for lighting
            glVertex3f(2 * torusX, 2 * torusY, 2 * torusZ);
            glNormal3d(2 * (torusX - xCenter), 2 * (torusY - yCenter), 2 * torusZ);
         }
      }
      glEnd();
   }
   
   glFrontFace(GL_CCW);
   //  Undo transformations
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}

/* ********************************************************************** */

/*
 *  Set projection
 */
static void projection()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode || firstPerson)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Display the scene
 */
void display()
{
   const double len=2.0;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Set blue background
   glClearColor(0.0, 0.12, 0.34, 1.0);
   //  Undo previous transformations
   glLoadIdentity();
   
   if (firstPerson)
   {
      // Y Rotation Limits
      if (ph > 90) ph = 90;
	  if (ph < -90) ph = -90;
      // Ajust the camera vector based on rotation
      cameraX = +2*dim*Sin(th)*Cos(ph);
	  cameraY = +2*dim        *Sin(ph);
      cameraZ = -2*dim*Cos(th)*Cos(ph);;
      //  Use gluLookAt, y is the up-axis 
      gluLookAt(povX,povY,povZ, cameraX+povX,cameraY+povY,cameraZ+povZ, 0,Cos(ph),0);
   }
   //  Perspective - set eye position
   else if (mode)
   {
      double cX = -2*dim*Sin(th)*Cos(ph);
      double cY = +2*dim        *Sin(ph);
      double cZ = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(cX,cY,cZ , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
	  //  Set view angle
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Light switch
   if (light)
   {
        //  Translate intensity to color vectors
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
        //  Light position
        float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
        //  Draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        ball(Position[0],Position[1],Position[2] , 0.1);
        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
        //  Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
        //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else
     glDisable(GL_LIGHTING);

   //  Draw scene
   //glColor3f (1.0, 0.7, 0.36);
   //glColor3f (1, 0.65, 0);
   glColor3f(1, 1, 1);
   torus(80, 80, 2*PI , 0.15,0.15,0.15, 0.07,0.07 , 0,0,0 , 2,2,1 , 0 , 1);
   torus(80, 80, 2*PI , 0.15,0.15,0.15, 0.07,0.07 , 1,1,1 , 1,1,0.5 , 0 , 2);
   torus(80, 80, 2*PI , 0.15,0.15,0.15, 0.07,0.07 , -1,-1,-1 , 1,1,0.5 , 0 , 3);
   /*
   glColor3f (1, 0, 0);
   torus(80, 80, 2*PI , 1.0,1.0,0.2, 0.1,0.1 , -0.8,0.5,0.01 , 0.55,0.3,1 , 0);
   torus(80, 80, 2*PI , 1.0,1.0,0.2, 0.1,0.1 ,  1.1,0.5,0.01 , 0.55,0.3,1 , 0);
   glColor3f (1, 1, 1);
   torus(80, 80, 2*PI , 1.0,1.0,0.2, 0.1,0.1 , 0,-0.7,0.01 , 1,0.2,1 , -10);
   glColor3f (0,  1, 0);
   torus(80, 80, 2*PI , 1.0,1.0,0.2, 0.1,0.1 , -1.7,2.3,0.1 , 0.75,0.75,7.5 , 1);
   glColor3f (0, 0, 1);
   torus(80, 80, 2*PI , 1.0,1.0,0.2, 0.2,0.2 , 1.5,0.0,0.01 , 0.1,0.2,1 , 0);
   torus(80, 80, 2*PI , 1.0,1.0,0.2, 0.2,0.2 , 1.7,-0.3,0.01 , 0.05,0.1,1 , 0);
   torus(80, 80, 2*PI , 1.0,1.0,0.2, 0.2,0.2 , -1.5,0.1,0.01 , 0.1,0.2,1 , 0);
   torus(80, 80, 2*PI , 1.0,1.0,0.2, 0.2,0.2 , -1.7,-0.2,0.01 , 0.05,0.1,1 , 0);
   */
   
   //  No lighting from here on
   glDisable(GL_LIGHTING);
   //  White
   glColor3f(1,1,1);
   //  Draw axes
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
	  glVertex3d(0.0,0.0,0.0);
      glVertex3d(-len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,-len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,-len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
	  glRasterPos3d(-len,0.0,0.0);
      Print("-X");
      glRasterPos3d(0.0,-len,0.0);
      Print("-Y");
      glRasterPos3d(0.0,0.0,-len);
      Print("-Z");
   }
   //  Five pixels from the lower left corner of the window
   glWindowPos2i(5,5);
   //  Print the text string
   Print("FP=%s  Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",firstPerson?"ON":"OFF",th,ph,dim,fov,mode?"Perpective":"Orthogonal", light?"On":"Off");
   if (light)
   {
      glWindowPos2i(5,45);
      Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
      glWindowPos2i(5,25);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
   }
   //  Render the scene and make it visible
   ErrCheck("display");
   //  Render the scene
   glFlush();
   //  Make the rendered scene visible
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when there is nothing else to do
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360);
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase azimuth by 5 degrees
   if (key == GLUT_KEY_RIGHT) th += 5;
   //  Left arrow key - decrease azimuth by 5 degrees
   else if (key == GLUT_KEY_LEFT) th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
   {
      if (firstPerson) ph += 5;
	  else ph -= 5;
   }
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
   {
      if (firstPerson) ph -= 5;
	  else ph += 5;
   }
   //  Smooth color model
   else if (key == GLUT_KEY_F1) smooth = 1-smooth;
   //  Local Viewer
   else if (key == GLUT_KEY_F2) local = 1-local;
   else if (key == GLUT_KEY_F3) distance = (distance==1) ? 5 : 1;
   //  Toggle ball increment
   else if (key == GLUT_KEY_F8) inc = (inc==10)?3:10;
   //  Flip sign
   else if (key == GLUT_KEY_F9) one = -one;
   
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   //projection(mode?fov:0,asp,dim);
   projection();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27) exit(0);

   //  Reset angle to y, z
   else if (ch == '1')
   {
      th = 90;
      ph = 0;
   }
   //  Reset angle to x, z
   else if (ch == '2')
   {
      th = 0;
      ph = -90;
   }
   //  Reset angle to x, y
   else if (ch == '3') th = ph = 0;
   //  Reset all displays
   else if (ch == '`')
   {
      axes = 1;
      fov = 55;
	  mode = th = ph = 0;
   }
   //  Reset Dimensional View
   else if (ch == '4') dim = 2.5;
   //  Toggle axes
   else if (ch == 'e' || ch == 'E') axes = 1 - axes;
   //  Switch display projection mode
   else if (ch == 'q' || ch == 'Q') mode = 1-mode;
   //  Change field of view angle
   else if ((ch == 'r'|| ch == 'R') && ch<179) fov++;
   else if ((ch == 'f'|| ch == 'F') && ch>1) fov--;
   
   //  Light level adjustments
   else if (ch=='5') ylight -= 0.1;
   else if (ch=='%') ylight += 0.1;
   else if (ch=='6' && ambient>0) ambient -= 5;
   else if (ch=='^' && ambient<100) ambient += 5;
   else if (ch=='7' && diffuse>0) diffuse -= 5;
   else if (ch=='&' && diffuse<100) diffuse += 5;
   else if (ch=='8' && specular>0) specular -= 5;
   else if (ch=='*' && specular<100) specular += 5;
   else if (ch=='9' && emission>0) emission -= 5;
   else if (ch=='(' && emission<100) emission += 5;
   else if (ch=='0' && shininess>-1) shininess -= 1;
   else if (ch==')' && shininess<7) shininess += 1;
   //  Toggle lighting
   else if (ch == 'z' || ch == 'Z') light = 1-light;
   //  Toggle light movement
   else if (ch == 'x' || ch == 'X') move = 1-move;
   
   //  Toggle First Person
   else if (ch == 'c'|| ch == 'C') firstPerson = 1 - firstPerson;
   // First Person Mode key overrides
   if (firstPerson)
   {
      dim = 3.0;
	  fov = 55;
      if (ch == 'w' || ch == 'W')
	  {
	     povX += cameraX * 0.05;
		 povZ += cameraZ * 0.05;
	  }
      else if (ch == 's' || ch == 'S')
	  {
	     povX -= cameraX * 0.05;
		 povZ -= cameraZ * 0.05;
	  }
	  else if (ch == 'a' || ch == 'a')
	  {
	     povX += cameraZ * 0.05;
		 povZ -= cameraX * 0.05;
	  }
	  else if (ch == 'd' || ch == 'D')
	  {
	     povX -= cameraZ * 0.05;
		 povZ += cameraX * 0.05;
	  }
   }
   else
   {
      //  None 1st Person keys
      if ((ch == 'w' || ch == 'W') && dim > 0.00) dim -= 0.05;
      else if ((ch == 's' || ch == 'S') && dim < 6.00) dim += 0.05;
	  else if (ch == 'a' || ch == 'A') zh += 1;
      else if (ch == 'd' || ch == 'D') zh -= 1;	  
   }
   
   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);
   // Reproject
   projection();
   //  Animate if requested
   glutIdleFunc(move?idle:NULL);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   //projection(mode?fov:0,asp,dim);
   projection();
   
}

/*
 *  Got this off the internet: http://www.gamedev.net/topic/310624-glut---using-the-mouse-wheel/
 *  GLUT calls this routine when the mouse is used
 */
void processMouse(int button, int state, int x, int y) 
{
    // Used for wheels, has to be up
   if (state == GLUT_UP )
   {
      if ( button == GLUT_WHEEL_UP )
	    dim -= 0.05;
	  else if( button == GLUT_WHEEL_DOWN )
	    dim += 0.05;
   }
	
   if (state == GLUT_DOWN)
   {
      if ( button == GLUT_LEFT_BUTTON )
	    zh += 1;
      else if ( button == GLUT_RIGHT_BUTTON )
	    zh -= 1;
      else if ( button == GLUT_MIDDLE_BUTTON )
	    th = ph = 0;
   }
   
   projection();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}
/*
 *  Got this off the internet: https://www.opengl.org/resources/libraries/glut/spec3/node51.html
 *  GLUT calls this routine when the mouse is passive and on screen
 */
void processMouseMotion(int x, int y)
{
     //  Right arrow key - increase azimuth by 5 degrees
   if (oldX < x)
      th += 1;
   //  Left arrow key - decrease azimuth by 5 degrees
   else if (oldX > x)
      th -= 1;
   //  Up arrow key - increase elevation by 5 degrees
   if (oldY < y)
      ph -= 1;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (oldY > y)
      ph += 1;

   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   oldX = x, oldY = y;
   
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 *  Modified from example 6 on Moodle
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window 
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   //  Request 800 x 600 pixel window
   glutInitWindowSize(800,600);
   //  Request Position on screen
   glutInitWindowPosition(300, 50);
   //  Create the window
   glutCreateWindow("Peter Huynh - Homework 6: Textures & Lighting");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "processMouse" when scroll wheel is used
   glutMouseFunc(processMouse);
   //  Tell GLUT to call "passiveMotionFunc" when mouse is on screen and moving
   glutPassiveMotionFunc(processMouseMotion);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Tell GLUT to call "idle" when there is nothing else to do
   glutIdleFunc(idle);
   //  Load Texture
   pink = LoadTexBMP(pinkTexture);
   choco = LoadTexBMP(chocoTexture);
   sugar = LoadTexBMP(sugarTexture);
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   //  Return code
   return 0;
}
