/*
 *  Peter Tran Huynh
 *  Lorenz Attractor Coordinates
 *
 *  Displays 2, 3 and 4 dimensional coordinates in 3D for the Lorenz Attractor Algorithm
 *  Used example 6 as an OpenGl base template, example 11 for the updating tracer, and lorenz.c for the math portion
 *
 *  Key bindings:
 *  `      RGB Color Mode
 *  1      RNG Color Mode
 *  2      Enable/Disable Color Strobing
 *  3      White Color Mode
 *  4      View Tracing Only Mode
 *  5      Red Color Mode
 *  6      Green Color Mode
 *  7      Blue Color Mode
 *   
 *  9      YZ Coordinate View
 *  0      XZ Coordinate View
 *  -      XY Coordinate View
 *  =      Reset Lorenz Values
 *
 *  q      Increase s Lorenz Value
 *  a      Decrease s Lorenz Value
 *  w      Increase b Lorenz Value
 *  s      Decrease b Lorenz Value
 *  e      Increase r Lorenz Value
 *  d      Decrease r Lorenz Value
 *
 *  arrows Change View Angle
 *  ESC    Exit
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
//   Defined constant for Lorenz Attractor Increments
#define LORENZ_INTERVAL_MAX 50000

//  Globals, I removed a lot of ex6 ones
int th=0;       // Azimuth of view angle
int ph=0;       // Elevation of view angle
double dim=2;   // Dimension of orthogonal box

/*  Lorenz Parameters, To be change-able by the user in Key function and used in Display function  */
double s  = 10, b  = 2.6666, r  = 28;
/*  Float array with variables used for color values when drawing  */
float red[10], green[10], blue[10];
/*  Int variables used for loops, color, strobing and updating the tracer point position as time goes on  */
int i, rngType = 1, strobeCheck = 1, timeTracker = 0;

/*  Structure used to hold coord values for Lorenz Attractor for tracing, From example 11  */
typedef struct
{
    double xPosition, yPosition, zPosition;
} lorenzPts;

/*
 *   Function used for generating rgb values for drawing
 */
void RngColors(int toRand)
{
  if(toRand == 0)
  {
     for(i = 0; i < 10; i++)
	 {
	    red[i] = 0.00;
		green[i] = 0.00;
		blue[i] = 0.00;
	 }
     red[1] = (double)rand() / RAND_MAX; red[4] = (double)rand() / RAND_MAX; red[7] = (double)rand() / RAND_MAX;
	 green[2] = (double)rand() / RAND_MAX; green[5] = (double)rand() / RAND_MAX; green[8] = (double)rand() / RAND_MAX;
	 blue[0] = (double)rand() / RAND_MAX; blue[3] = (double)rand() / RAND_MAX; blue[6] = (double)rand() / RAND_MAX; blue[9] = (double)rand() / RAND_MAX;
  }
  else if(toRand == 1)
  {
     for(i = 0; i < 10; i++)
     {
       red[i] = (double)rand() / RAND_MAX;
       green[i] = (double)rand() / RAND_MAX;
       blue[i] = (double)rand() / RAND_MAX;
	
       // Checks for random black, if so, try again
       if(red[i] == green[i] && red[i] == blue[i] && red[i] == 0)
       {
          red[i] = (double)rand() / RAND_MAX;
          green[i] = (double)rand() / RAND_MAX;
          blue[i] = (double)rand() / RAND_MAX;
       }
     }
  }
  else if(toRand == 2)
  {
     for(i = 0; i < 10; i++)
	 {
	    red[i] = 1.00;
		green[i] = 1.00;
		blue[i] = 1.00;
	 }
  }
  else if(toRand == 3)
  {
     for(i = 0; i < 10; i++)
	 {
	    red[i] = 0.00;
		green[i] = 0.00;
		blue[i] = 0.00;
	 }
  }
  else if(toRand == 4)
  {
     for(i = 0; i < 10; i++)
	 {
	    red[i] = (double)rand() / RAND_MAX;
		green[i] = 0.00;
		blue[i] = 0.00;
	 }
  }
  else if(toRand == 5)
  {
     for(i = 0; i < 10; i++)
	 {
	    red[i] = 0.00;
		green[i] = (double)rand() / RAND_MAX;
		blue[i] = 0.00;
	 }
  }
  else if(toRand == 6)
  {
     for(i = 0; i < 10; i++)
	 {
	    red[i] = 0.00;
		green[i] = 0.00;
		blue[i] = (double)rand() / RAND_MAX;
	 }
  }
}

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

/*
 *  Display the scene
 */
void display()
{
   /*  Lorenz Variables from Lorenz.c */
   /*  Coordinates, Position Steps, and Time Step  */
   double x = 1, y = 1, z = 1, dx, dy, dz, dt = 0.001;
   /*  Used for tracing the Lorenz Attractor  */
   lorenzPts lorenz[LORENZ_INTERVAL_MAX];
   
   // Strobing Check
   if(strobeCheck == 1)
      RngColors(rngType);
   
   //  Clear the image
   glClear(GL_COLOR_BUFFER_BIT);
   //  Reset previous transforms
   glLoadIdentity();
   //  Set view angle
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   
   // Starts drawing, set color to yellow for first point for now
   glColor3f(1,1,0);
   glBegin(GL_LINES);
   
   for (i = 0; i < LORENZ_INTERVAL_MAX; i++)
   {
      // Draws initial point based on whichever color was selected
      glVertex4d(x, y, z, 25);
   
      // Calculate the next point in the lorenz attractor for tracing
      // Adapted from the lorenz.c program given on Moodle
      lorenzPts nextPts = {x, y, z};
      lorenz[i] = nextPts;       

      // Switch the color every 5000 steps up to 50,000. a total of 10 times
      if((i % 5000) == 0)
	  {
		 glColor3f(red[(i / 5000) - 1], green[(i / 5000) - 1], blue[(i / 5000) - 1]);
      }

	  // Lorenz Attractor Math from lorenz.c, used to find the next point
      dx = s*(y-x);
      dy = x*(r-z)-y;
      dz = x*y - b*z;
      x += dt*dx;
      y += dt*dy;
      z += dt*dz;

   }
   
   /*  Set final drawing point with a 25 down scaling, colored in white  */
   glVertex4d(x, y, z, 25);
   
   // Ends drawing
   glEnd();
   
   /*  Tracing. Using a point to move along the lorenz array x, y, and z pushed onto a matrix queue for poping later and idle updating, From Example 11, partially */
   glColor3f(1,1,0);
   glPushMatrix();
   glTranslatef(lorenz[timeTracker].xPosition / 25, lorenz[timeTracker].yPosition / 25, lorenz[timeTracker].zPosition / 25);
   glutSolidCube(0.069);
   glPopMatrix();
   
   //  Draw axes in white
   glColor3f(1,1,1);
   glBegin(GL_LINES);
   glVertex3d(0,0,0);
   glVertex3d(1,0,0);
   glVertex3d(0,0,0);
   glVertex3d(0,1,0);
   glVertex3d(0,0,0);
   glVertex3d(0,0,1);
   glEnd();
   //  Label axes
   glRasterPos3d(1,0,0);
   Print("X");
   glRasterPos3d(0,1,0);
   Print("Y");
   glRasterPos3d(0,0,1);
   Print("Z");
   //  Display parameters
   glWindowPos2i(5,5);
   Print("View Angle = %d, %d",th,ph);
   
   /*  Displays Lorenz Values  */
   glWindowPos2i(10, 25);
   Print("s = %.02f, b = %.04f, r = %.02f", s, b, r);
   
   //  Flush and swap
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   // Adjust Lorenz Attractor Values
   else if (ch == 'q')
      s += 0.05;
   else if (ch == 'a')
      s -= 0.05;
   else if (ch == 'w')
      b += 0.05;
   else if (ch == 's')
      b -= 0.05;
   else if (ch == 'e')
      r += 0.05;
   else if (ch == 'd')
      r -= 0.05;
   // RGB View
   else if (ch == '`')
   {
      rngType = 0;
      RngColors(rngType);
   }
   // Randomly changes the color
   else if (ch == '1')
   {
      rngType = 1;
      RngColors(rngType);
   }
   // Strobing control
   else if (ch == '2')
   {
      if (strobeCheck == 1)
         strobeCheck = 0;
      else
	     strobeCheck = 1;
   }
   // Other Color Options
   else if (ch == '3')
   {
      rngType = 2;
	  RngColors(rngType);
   }
   else if (ch == '4')
   {
      rngType = 3;
      RngColors(rngType);
   }
   else if (ch == '5')
   {
      rngType = 4;
	  RngColors(rngType);
   }
   else if (ch == '6')
   {
      rngType = 5;
	  RngColors(rngType);
   }
   else if (ch == '7')
   {
      rngType = 6;
	  RngColors(rngType);
   }
   //  View angle to y, z
   else if (ch == '9')
   {
      th = 90;
      ph = 0;
   }
   //  View angle to x, z
   else if (ch == '0')
   {
      th = 0;
      ph = -90;
   }
   //  Reset view angle to default x, y
   else if (ch == '-')
      th = ph = 0;
   // Reset Lorenz Values
   else if (ch == '=')
   {
      s  = 10;
      b  = 2.6666;
      r  = 28;
   }
	  
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
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
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
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   glOrtho(-dim*w2h,+dim*w2h, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  GLUT calls this toutine when there is nothing else to do
 *  Taken from Example 11
 */
void idle()
{
   timeTracker = glutGet(GLUT_ELAPSED_TIME) % LORENZ_INTERVAL_MAX;
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   // Setting RNG Colors
   RngColors(rngType);

   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window 
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   //  Request 500 x 500 pixel window
   glutInitWindowSize(500,500);
   //  Create the window
   glutCreateWindow("Peter Tran Huynh - HW2 - Lorenz Attractor Coordinates");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   
   //  From example 11: Tell GLUT to call "idle" when nothing else is going on
   glutIdleFunc(idle);
   
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   //  Return code
   return 0;
}
