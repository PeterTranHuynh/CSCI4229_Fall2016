/*
 *  Peter Tran Huynh
 *  Scene in 3D 
 *
 *  Displays a scene full of 3D objects, some duplicated from one another.
 *  Used Example 8 for coordinate display base, cube creation and scene changing
 *
 *  Key bindings:
 *  `      Reset all of the display to initial startup settings
 *  1      Reset view angle to y,z
 *  2      Reset view angle to x,z
 *  3      Reset view angle to x,y
 *  q      Reset to default zoom
 *  w      Zoom In
 *  s      Zoom Out
 *  a      Cycle through previous set of objects
 *  d      Cycle through next set of objects
 *  e      Toggle axes
 *  arrows Change view angle
 *
 *  mouse motion  Change view angle, dynamically
 *  mouse scroll  Changes Zoom value, dynamically
 *  mouse left    Cycle through previous set of objects
 *  mouse right   Cycle through next set of objects
 *  mouse middle  Reset position
 *  ESC    Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
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
//  Apparently C doesn't have PI as a default constant. C++ does though.
#define PI 3.14159265358979323846
//  Cosine and Sine in degrees
#define Cos(x) (cos((x) * (PI / 180)))
#define Sin(x) (sin((x) * (PI / 180)))
GLuint theTorus;

/*  Globals  */
int th=0;              //  Azimuth of view angle
int ph=0;              //  Elevation of view angle
double zh=0;           //  Rotation of object
int axes=1;            //  Display axes
int mode=0;            //  What to display
//  Moved to global from Reshape function to allow zooming in and out
double w2h, dim = 2.5; //  Dimension of orthogonal box
int oldX = 0, oldY = 0;

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
 *  Adapted from Example 8's cube function
 *  Draws a table
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void chair(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Leg Values
   double i;
   double radius = 0.3, height = 2.0;
   
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   
   glBegin(GL_QUADS);
   glColor3f(0.5, 0.35, 0.05);
   
   /*  DRAWING THE CHAIR BACK  */
   //  Front
   glVertex3f(-1.0,-0.1, 1);
   glVertex3f(-1.2,-0.1, 1);
   glVertex3f(-1.2,+2.2, 1);
   glVertex3f(-1.0,+2.2, 1);
   //  Back
   glVertex3f(-1.2,-0.1,-1);
   glVertex3f(-1.0,-0.1,-1);
   glVertex3f(-1.0,+2.2,-1);
   glVertex3f(-1.2,+2.2,-1);
   //  Right
   glVertex3f(-1.2,-0.1,+1);
   glVertex3f(-1.2,-0.1,-1);
   glVertex3f(-1.2,+2.2,-1);
   glVertex3f(-1.2,+2.2,+1);
   //  Left
   glVertex3f(-1.0,-0.1,-1);
   glVertex3f(-1.0,-0.1,+1);
   glVertex3f(-1.0,+2.2,+1);
   glVertex3f(-1.0,+2.2,-1);
   //  Top
   glVertex3f(-1.0,+2.2,+1);
   glVertex3f(-1.2,+2.2,+1);
   glVertex3f(-1.2,+2.2,-1);
   glVertex3f(-1.0,+2.2,-1);
   //  Bottom
   glVertex3f(-1.0,-0.1,-1);
   glVertex3f(-1.2,-0.1,-1);
   glVertex3f(-1.2,-0.1,+1);
   glVertex3f(-1.0,-0.1,+1);
   //  End
   glEnd();
   
   /*  OUTLINING THE BACK  */
   glColor3f(0.00, 0.00, 0.00);
   //  Front
   glBegin(GL_LINE_LOOP);
   glVertex3f(-0.99,-0.11, 1.01);
   glVertex3f(-1.21,-0.11, 1.01);
   glVertex3f(-1.21,+2.21, 1.01);
   glVertex3f(-0.99,+2.21, 1.01);
   glEnd();
   //  Back
   glBegin(GL_LINE_LOOP);
   glVertex3f(-1.21,-0.11,-1.01);
   glVertex3f(-0.99,-0.11,-1.01);
   glVertex3f(-0.99,+2.21,-1.01);
   glVertex3f(-1.21,+2.21,-1.01);
   glEnd();
   //  Right
   glBegin(GL_LINE_LOOP);
   glVertex3f(-1.21,-0.11,+1.01);
   glVertex3f(-1.21,-0.11,-1.01);
   glVertex3f(-1.21,+2.21,-1.01);
   glVertex3f(-1.21,+2.21,+1.01);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-1.21,-0.11,+0.50);
   glVertex3f(-1.21,+2.21,+0.50);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-1.21,-0.11,+0.00);
   glVertex3f(-1.21,+2.21,+0.00);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-1.21,-0.11,-0.50);
   glVertex3f(-1.21,+2.21,-0.50);
   glEnd();
   //  Left
   glBegin(GL_LINE_LOOP);
   glVertex3f(-0.99,-0.11,-1.01);
   glVertex3f(-0.99,-0.11,+1.01);
   glVertex3f(-0.99,+2.21,+1.01);
   glVertex3f(-0.99,+2.21,-1.01);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-0.99,-0.11,+0.50);
   glVertex3f(-0.99,+2.21,+0.50);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-0.99,-0.11,+0.00);
   glVertex3f(-0.99,+2.21,+0.00);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-0.99,-0.11,-0.50);
   glVertex3f(-0.99,+2.21,-0.50);
   glEnd();
   //  Top
   glBegin(GL_LINE_LOOP);
   glVertex3f(-0.99,+2.21,+1.01);
   glVertex3f(-1.21,+2.21,+1.01);
   glVertex3f(-1.21,+2.21,-1.01);
   glVertex3f(-0.99,+2.21,-1.01);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-0.99,+2.21,+0.50);
   glVertex3f(-1.21,+2.21,+0.50);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-0.99,+2.21,+0.00);
   glVertex3f(-1.21,+2.21,+0.00);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-1.21,+2.21,-0.50);
   glVertex3f(-0.99,+2.21,-0.50);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-0.99,+0.11,-1.01);
   glVertex3f(-0.99,+0.11,+1.01);
   glEnd();
   
   glBegin(GL_QUADS);
   glColor3f(0.5, 0.35, 0.05);
   /*  DRAWING THE SEAT  */
   //  Front
   glVertex3f(-1.2,-0.1, 1);
   glVertex3f(+1,-0.1, 1);
   glVertex3f(+1,+0.1, 1);
   glVertex3f(-1.2,+0.1, 1);
   //  Back
   glVertex3f(+1,-0.1,-1);
   glVertex3f(-1.2,-0.1,-1);
   glVertex3f(-1.2,+0.1,-1);
   glVertex3f(+1,+0.1,-1);
   //  Right
   glVertex3f(+1,-0.1,+1);
   glVertex3f(+1,-0.1,-1);
   glVertex3f(+1,+0.1,-1);
   glVertex3f(+1,+0.1,+1);
   //  Left
   glVertex3f(-1.2,-0.1,-1);
   glVertex3f(-1.2,-0.1,+1);
   glVertex3f(-1.2,+0.1,+1);
   glVertex3f(-1.2,+0.1,-1);
   //  Top
   glVertex3f(-1.2,+0.1,+1);
   glVertex3f(+1,+0.1,+1);
   glVertex3f(+1,+0.1,-1);
   glVertex3f(-1.2,+0.1,-1);
   //  Bottom
   glVertex3f(-1.2,-0.1,-1);
   glVertex3f(+1,-0.1,-1);
   glVertex3f(+1,-0.1,+1);
   glVertex3f(-1.2,-0.1,+1);
   //  End
   glEnd();
   
   /*  OUTLINING THE SEAT  */
   glColor3f(0.00, 0.00, 0.00);
   //  Front
   glBegin(GL_LINE_LOOP);
   glVertex3f(-1.21,-0.11, 1.01);
   glVertex3f(+1.01,-0.11, 1.01);
   glVertex3f(+1.01,+0.11, 1.01);
   glVertex3f(-1.21,+0.11, 1.01);
   glEnd();
   //  Back
   glBegin(GL_LINE_LOOP);
   glVertex3f(+1.01,-0.11,-1.01);
   glVertex3f(-1.21,-0.11,-1.01);
   glVertex3f(-1.21,+0.11,-1.01);
   glVertex3f(+1.01,+0.11,-1.01);
   glEnd();
   //  Right
   glBegin(GL_LINE_LOOP);
   glVertex3f(+1.01,-0.11,+1.01);
   glVertex3f(+1.01,-0.11,-1.01);
   glVertex3f(+1.01,+0.11,-1.01);
   glVertex3f(+1.01,+0.11,+1.01);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(+1.01,-0.11,+0.50);
   glVertex3f(+1.01,+0.11,+0.50);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(+1.01,-0.11,+0.00);
   glVertex3f(+1.01,+0.11,+0.00);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(+1.01,-0.11,-0.50);
   glVertex3f(+1.01,+0.11,-0.50);
   glEnd();
   //  Top
   glBegin(GL_LINE_LOOP);
   glVertex3f(-1.21,+0.11,+1.01);
   glVertex3f(+1.01,+0.11,+1.01);
   glVertex3f(+1.01,+0.11,-1.01);
   glVertex3f(-1.21,+0.11,-1.01);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-1.21,+0.11,+0.50);
   glVertex3f(+1.01,+0.11,+0.50);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-1.21,+0.11,+0.00);
   glVertex3f(+1.01,+0.11,+0.00);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(+1.01,+0.11,-0.50);
   glVertex3f(-1.21,+0.11,-0.50);
   glEnd();
   //  Bottom
   glBegin(GL_LINE_LOOP);
   glVertex3f(-1.21,-0.11,-1.01);
   glVertex3f(+1.01,-0.11,-1.01);
   glVertex3f(+1.01,-0.11,+1.01);
   glVertex3f(-1.21,-0.11,+1.01);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-1.21,-0.11,-0.50);
   glVertex3f(+1.01,-0.11,-0.50);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-1.21,-0.11,-0.00);
   glVertex3f(+1.01,-0.11,-0.00);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(+1.01,-0.11,+0.50);
   glVertex3f(-1.21,-0.11,+0.50);
   glEnd();
   
   /*  DRAWING THE LEGS  */
   glColor3f(0.48, 0.33, 0.05);
   //  Scale change for rotation!
   glScaled(0.3,0.3,0.3);
   //  Cylinder Tube 
   glBegin(GL_QUAD_STRIP);
   for(i = 0.0; i <= 2.1 * PI; i += 0.05)
   {
      glVertex3d(radius * cos(i) - 3.6, height - 2.2, radius * sin(i) - 3);
      glVertex3d(radius * cos(i) - 3.6, -height - 2.2, radius * sin(i) - 3);
   }
   glEnd();
   //  Bottom of Cylinder
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0.0 - 3.6, -height - 2.2, 0.0 - 3);
   for(i = 0.0; i < 2 * PI * radius * 4; i += 0.125)
   {
      glVertex3d(radius * cos(i) - 3.6, -height - 2.2, radius * sin(i) - 3);
   }
   glEnd();
   glBegin(GL_QUAD_STRIP);
   for(i = 0.0; i <= 2.1 * PI; i += 0.05)
   {
      glVertex3d(radius * cos(i) + 3, height - 2.2, radius * sin(i) - 3);
      glVertex3d(radius * cos(i) + 3, -height - 2.2, radius * sin(i) - 3);
   }
   glEnd();
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0.0 + 3, -height - 2.2, 0.0 - 3);
   for(i = 0.0; i < 2 * PI * radius * 4; i += 0.125)
   {
      glVertex3d(radius * cos(i) + 3, -height - 2.2, radius * sin(i) - 3);
   }
   glEnd();
      glBegin(GL_QUAD_STRIP);
   for(i = 0.0; i <= 2.1 * PI; i += 0.05)
   {
      glVertex3d(radius * cos(i) - 3.6, height - 2.2, radius * sin(i) + 3);
      glVertex3d(radius * cos(i) - 3.6, -height - 2.2, radius * sin(i) + 3);
   }
   glEnd();
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0.0 - 3.6, -height - 2.2, 0.0 + 3);
   for(i = 0.0; i < 2 * PI * radius * 4; i += 0.125)
   {
      glVertex3d(radius * cos(i) - 3.6, -height - 2.2, radius * sin(i) + 3);
   }
   glEnd();
   glBegin(GL_QUAD_STRIP);
   for(i = 0.0; i <= 2.1 * PI; i += 0.05)
   {
      glVertex3d(radius * cos(i) + 3, height - 2.2, radius * sin(i) + 3);
      glVertex3d(radius * cos(i) + 3, -height - 2.2, radius * sin(i) + 3);
   }
   glEnd();
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0.0 + 3, -height - 2.2, 0.0 + 3);
   for(i = 0.0; i < 2 * PI * radius * 4; i += 0.125)
   {
      glVertex3d(radius * cos(i) + 3, -height - 2.2, radius * sin(i) + 3);
   }
   glEnd();
   
   /*  OUTLINING THE LEGS  */
   glColor3f(0.0, 0.0, 0.00);
   glBegin(GL_LINE_LOOP);
   for(i = 0.0; i <= 360; i++)
   {
      glVertex3f(cos(i * PI/180) * radius - 3.6, -height - 2.3, sin(i * PI/180) * radius - 3);
   }
   glEnd();
   glBegin(GL_LINE_LOOP);
   for(i = 0.0; i <= 360; i++)
   {
      glVertex3f(cos(i * PI/180) * radius + 3, -height - 2.3, sin(i * PI/180) * radius - 3);
   }
   glEnd();
   glBegin(GL_LINE_LOOP);
   for(i = 0.0; i <= 360; i++)
   {
      glVertex3f(cos(i * PI/180) * radius - 3.6, -height - 2.3, sin(i * PI/180) * radius + 3);
   }
   glEnd();
   glBegin(GL_LINE_LOOP);
   for(i = 0.0; i <= 360; i++)
   {
      glVertex3f(cos(i * PI/180) * radius + 3, -height - 2.3, sin(i * PI/180) * radius + 3);
   }
   glEnd();
   
   //  Undo transformations
   glPopMatrix();
}

/*
 *  Adapted from Example 8's cube function
 *  Draws a table
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */

static void table(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Leg Values
   double i;
   double radius = 0.3, height = 2.0;
   
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   
   glBegin(GL_QUADS);
   glColor3f(0.5, 0.35, 0.05);
   
   /*  DRAWING THE TABLETOP  */
   //  Front
   glVertex3f(-2,-0.1, 1);
   glVertex3f(+2,-0.1, 1);
   glVertex3f(+2,+0.1, 1);
   glVertex3f(-2,+0.1, 1);
   //  Back
   glVertex3f(+2,-0.1,-1);
   glVertex3f(-2,-0.1,-1);
   glVertex3f(-2,+0.1,-1);
   glVertex3f(+2,+0.1,-1);
   //  Right
   glVertex3f(+2,-0.1,+1);
   glVertex3f(+2,-0.1,-1);
   glVertex3f(+2,+0.1,-1);
   glVertex3f(+2,+0.1,+1);
   //  Left
   glVertex3f(-2,-0.1,-1);
   glVertex3f(-2,-0.1,+1);
   glVertex3f(-2,+0.1,+1);
   glVertex3f(-2,+0.1,-1);
   //  Top
   glVertex3f(-2,+0.1,+1);
   glVertex3f(+2,+0.1,+1);
   glVertex3f(+2,+0.1,-1);
   glVertex3f(-2,+0.1,-1);
   //  Bottom
   glVertex3f(-2,-0.1,-1);
   glVertex3f(+2,-0.1,-1);
   glVertex3f(+2,-0.1,+1);
   glVertex3f(-2,-0.1,+1);
   //  End
   glEnd();
   
   /*  OUTLINING THE TABLETOP  */
   glColor3f(0.00, 0.00, 0.00);
   //  Front
   glBegin(GL_LINE_LOOP);
   glVertex3f(-2.01,-0.11, 1.01);
   glVertex3f(+2.01,-0.11, 1.01);
   glVertex3f(+2.01,+0.11, 1.01);
   glVertex3f(-2.01,+0.11, 1.01);
   glEnd();
   //  Back
   glBegin(GL_LINE_LOOP);
   glVertex3f(+2.01,-0.11,-1.01);
   glVertex3f(-2.01,-0.11,-1.01);
   glVertex3f(-2.01,+0.11,-1.01);
   glVertex3f(+2.01,+0.11,-1.01);
   glEnd();
   //  Right
   glBegin(GL_LINE_LOOP);
   glVertex3f(+2.01,-0.11,+1.01);
   glVertex3f(+2.01,-0.11,-1.01);
   glVertex3f(+2.01,+0.11,-1.01);
   glVertex3f(+2.01,+0.11,+1.01);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(+2.01,-0.11,+0.50);
   glVertex3f(+2.01,+0.11,+0.50);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(+2.01,-0.11,+0.00);
   glVertex3f(+2.01,+0.11,+0.00);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(+2.01,-0.11,-0.50);
   glVertex3f(+2.01,+0.11,-0.50);
   glEnd();
   //  Left
   glBegin(GL_LINE_LOOP);
   glVertex3f(-2.01,-0.11,-1.01);
   glVertex3f(-2.01,-0.11,+1.01);
   glVertex3f(-2.01,+0.11,+1.01);
   glVertex3f(-2.01,+0.11,-1.01);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-2.01,-0.11,+0.50);
   glVertex3f(-2.01,+0.11,+0.50);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-2.01,-0.11,+0.00);
   glVertex3f(-2.01,+0.11,+0.00);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-2.01,-0.11,-0.50);
   glVertex3f(-2.01,+0.11,-0.50);
   glEnd();
   //  Top
   glBegin(GL_LINE_LOOP);
   glVertex3f(-2.01,+0.11,+1.01);
   glVertex3f(+2.01,+0.11,+1.01);
   glVertex3f(+2.01,+0.11,-1.01);
   glVertex3f(-2.01,+0.11,-1.01);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-2.01,+0.11,+0.50);
   glVertex3f(+2.01,+0.11,+0.50);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-2.01,+0.11,+0.00);
   glVertex3f(+2.01,+0.11,+0.00);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(+2.01,+0.11,-0.50);
   glVertex3f(-2.01,+0.11,-0.50);
   glEnd();
   //  Bottom
   glBegin(GL_LINE_LOOP);
   glVertex3f(-2.01,-0.11,-1.01);
   glVertex3f(+2.01,-0.11,-1.01);
   glVertex3f(+2.01,-0.11,+1.01);
   glVertex3f(-2.01,-0.11,+1.01);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-2.01,-0.11,-0.50);
   glVertex3f(+2.01,-0.11,-0.50);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(-2.01,-0.11,-0.00);
   glVertex3f(+2.01,-0.11,-0.00);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(+2.01,-0.11,+0.50);
   glVertex3f(-2.01,-0.11,+0.50);
   glEnd();
   
   /*  DRAWING THE LEGS  */
   glColor3f(0.48, 0.33, 0.05);
   //  Scale change for rotation!
   glScaled(0.3,0.3,0.3);
   //  Cylinder Tube 
   glBegin(GL_QUAD_STRIP);
   for(i = 0.0; i <= 2.1 * PI; i += 0.05)
   {
      glVertex3d(radius * cos(i) - 6, height - 2.2, radius * sin(i) - 3);
      glVertex3d(radius * cos(i) - 6, -height - 2.2, radius * sin(i) - 3);
   }
   glEnd();
   //  Bottom of Cylinder
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0.0 - 6, -height - 2.2, 0.0 - 3);
   for(i = 0.0; i < 2 * PI * radius * 4; i += 0.125)
   {
      glVertex3d(radius * cos(i) - 6, -height - 2.2, radius * sin(i) - 3);
   }
   glEnd();
   glBegin(GL_QUAD_STRIP);
   for(i = 0.0; i <= 2.1 * PI; i += 0.05)
   {
      glVertex3d(radius * cos(i) + 6, height - 2.2, radius * sin(i) - 3);
      glVertex3d(radius * cos(i) + 6, -height - 2.2, radius * sin(i) - 3);
   }
   glEnd();
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0.0 + 6, -height - 2.2, 0.0 - 3);
   for(i = 0.0; i < 2 * PI * radius * 4; i += 0.125)
   {
      glVertex3d(radius * cos(i) + 6, -height - 2.2, radius * sin(i) - 3);
   }
   glEnd();
      glBegin(GL_QUAD_STRIP);
   for(i = 0.0; i <= 2.1 * PI; i += 0.05)
   {
      glVertex3d(radius * cos(i) - 6, height - 2.2, radius * sin(i) + 3);
      glVertex3d(radius * cos(i) - 6, -height - 2.2, radius * sin(i) + 3);
   }
   glEnd();
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0.0 - 6, -height - 2.2, 0.0 + 3);
   for(i = 0.0; i < 2 * PI * radius * 4; i += 0.125)
   {
      glVertex3d(radius * cos(i) - 6, -height - 2.2, radius * sin(i) + 3);
   }
   glEnd();
   glBegin(GL_QUAD_STRIP);
   for(i = 0.0; i <= 2.1 * PI; i += 0.05)
   {
      glVertex3d(radius * cos(i) + 6, height - 2.2, radius * sin(i) + 3);
      glVertex3d(radius * cos(i) + 6, -height - 2.2, radius * sin(i) + 3);
   }
   glEnd();
   glBegin(GL_TRIANGLE_FAN);
   glVertex3d(0.0 + 6, -height - 2.2, 0.0 + 3);
   for(i = 0.0; i < 2 * PI * radius * 4; i += 0.125)
   {
      glVertex3d(radius * cos(i) + 6, -height - 2.2, radius * sin(i) + 3);
   }
   glEnd();
   
   /*  OUTLINING THE LEGS  */
   glColor3f(0.0, 0.0, 0.00);
   glBegin(GL_LINE_LOOP);
   for(i = 0.0; i <= 360; i++)
   {
      glVertex3f(cos(i * PI/180) * radius - 6, -height - 2.3, sin(i * PI/180) * radius - 3);
   }
   glEnd();
   glBegin(GL_LINE_LOOP);
   for(i = 0.0; i <= 360; i++)
   {
      glVertex3f(cos(i * PI/180) * radius + 6, -height - 2.3, sin(i * PI/180) * radius - 3);
   }
   glEnd();
   glBegin(GL_LINE_LOOP);
   for(i = 0.0; i <= 360; i++)
   {
      glVertex3f(cos(i * PI/180) * radius - 6, -height - 2.3, sin(i * PI/180) * radius + 3);
   }
   glEnd();
   glBegin(GL_LINE_LOOP);
   for(i = 0.0; i <= 360; i++)
   {
      glVertex3f(cos(i * PI/180) * radius + 6, -height - 2.3, sin(i * PI/180) * radius + 3);
   }
   glEnd();
   
   //  Undo transformations
   glPopMatrix();
}

/* 
 *  Got this from OpenGL site: https://www.opengl.org/archives/resources/code/samples/redbook/torus.c
 *  Draw a torus DONUT
 *  Takes in radius's 
 */

static void torus(int numc, int numt)
{
   int i, j, k;
   double s, t, x, y, z, twopi;

   twopi = 2 * PI;
   for (i = 0; i < numc; i++) {
      glBegin(GL_QUAD_STRIP);
      for (j = 0; j <= numt; j++) {
         for (k = 1; k >= 0; k--) {
            s = (i + k) % numc + 0.5;
            t = j % numt;

            x = (1+.1*cos(s*twopi/numc))*cos(t*twopi/numt);
            y = (1+.1*cos(s*twopi/numc))*sin(t*twopi/numt);
            z = .1 * sin(s * twopi / numc);
            glVertex3f(x, y, z);
         }
      }
      glEnd();
   }
}

/* ********************************************************************** */

/*
 *  Display the scene
 */
void display()
{
   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Set background
   glClearColor(0.0, 0.12, 0.34, 1.0);
   //  Undo previous transformations
   glLoadIdentity();
   //  Set view angle
   glRotatef(ph,1,0,0);
   glRotatef(th,0,1,0);

   //  Decide what to draw
   switch (mode)
   {
      case 0:
	     table(0,0.25,0 , 0.5,0.5,0.5 , 90);
         chair(-1,0,0 , 0.3,0.3,0.3 , 0);
         break;
      case 1:
	     table(0.2,-1,0.5 , 0.4,0.2,0.6 , 30);
         chair(-1,0.3,-0.3 , 0.3,0.29,0.3 , 69);
         chair(1,0,-1 , 0.5,0.5,0.5 , 90);
         table(0,0.7,1 , 0.4,0.4,0.2 , 45);
         break;
	  case 2:
		 table(0,-0.75,0 , 0.9,0.5,0.5 , 90);
         chair(-1,-1,-1 , 0.3,0.3,0.3 , 0);
		 chair(-1,-1,1 , 0.3,0.3,0.3 , 0);
		 chair(1,-1,-1 , 0.3,0.3,0.3 , 190);
		 chair(1,-1,1 , 0.3,0.3,0.3 , 180);
		 break;
      case 3:
		 chair(0,-1,0 , 0.9,0.1,0.3 , 90);
         chair(-1,0,-1 , 0.4,0.5,0.2 , 69);
		 chair(-1,1,1 , 0.8,0.23,0.31 , 44);
		 chair(1,0,-1 , 0.7,0.63,0.53 , 10);
		 chair(1,-1,1 , 0.35,0.33,0.32 , 180);
		 break;
	  case 4:
		 table(0,-1,0 , 0.2,0.5,0.4 , 23);
         table(-1,0,-1 , 0.4,0.8,0.6 , 49);
		 table(-1,1,1 , 0.4,0.6,0.2 , 64);
		 table(1,0,-1 , 0.8,0.4,0.7 , 85);
		 table(1,-0.5,1 , 1,0.2,0.6 , 134);
		 break;
      case 5:
         table(1,0,0 , 0.3,0.3,0.3 , 3*zh);
		 chair(-1,0,0 , 0.3,0.3,0.3 , 3*zh);
         break;
	  case 6:
	     glClear(GL_COLOR_BUFFER_BIT);
		 glColor3f (1.0, 0.7, 0.36);
		 glCallList(theTorus);
		 glFlush();
	     break;
   }

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
   Print("Angle=%d,%d",th,ph);
   //  Render the scene
   glFlush();
   //  Make the rendered scene visible
   glutSwapBuffers();
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
	  mode = th = ph = 0;
   }
   else if (ch == 'q' || ch == 'Q') dim = 2.5;
   //  Change dimensional orthogonal view 
   else if (ch == 'w' || ch == 'W') dim -= 0.05;
   else if (ch == 's' || ch == 'S') dim += 0.05;
   //  Toggle axes
   else if (ch == 'e' || ch == 'E') axes = 1 - axes;
   //  Switch display mode
   else if (ch == 'a' || ch == 'A') mode = (mode + 6) % 7;
   else if (ch == 'd' || ch == 'D') mode = (mode + 1) % 7;

   // Limits for dimensions for view
   if (dim > 3.0) dim = 3.0; 
   if (dim < 0.0) dim = 0.0; 

   /*  ALL FOR RESIZING THE PROJECTION VIEW  */
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   glOrtho(-dim*w2h,+dim*w2h, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();

   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase azimuth by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease azimuth by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph -= 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph += 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection
   glOrtho(-w2h*dim,+w2h*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
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
	    mode = (mode + 6) % 7;
      else if ( button == GLUT_RIGHT_BUTTON )
	    mode = (mode + 1) % 7;
      else if ( button == GLUT_MIDDLE_BUTTON )
	    th = ph = 0;
   }
   
   /*  ALL FOR RESIZING THE PROJECTION VIEW  */
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   glOrtho(-dim*w2h,+dim*w2h, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();

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
   glutCreateWindow("Peter Huynh - Homework 3");
   
   // DONUT
   theTorus = glGenLists (1);
   glNewList(theTorus, GL_COMPILE);
   torus(8, 20);
   glEndList();

   glShadeModel(GL_FLAT);
   glClearColor(0.0, 0.0, 0.0, 0.0);
   
   //  Tell GLUT to call "idle" when there is nothing else to do
   glutIdleFunc(idle);
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
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   //  Return code
   return 0;
}