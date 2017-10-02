/* CREDITS: ALL THINGS WERE MODIFIED FROM THE FOLLOWING SOURCES
 * Sky Box:                   Modified from Example 25
 * Blending & Transparency:   Modified from Example 22
 * Bird Third Person:         Modified from Example 20
 * Lighting:                  Modified from Example 13
 * Fog:                       Based off November 7-13 Slides
 * Weather Particle Source:   Modified from https://classes.soe.ucsc.edu/cmps161/Winter09/projects/mang/finalproject.html
 * Bird Model Source:         Two frames of http://www.blendswap.com/blends/view/25541
 * Grass and Skybox Textures: Modified from https://www.youtube.com/watch?v=rqx9IDLKV28
 * Tree Source:               Modified from http://www.blendswap.com/blends/view/82037
 *
 *
 *  Peter Tran Huynh
 *  Final Project: Bird, Weather and Time
 *
 *  Key bindings:
 *  1             Rain Mode, Default Mode
 *  2             Hail Mode
 *  3             Snow Mode, WARNING: LAGGY
 *  w/a/s/d       Move around along x-z axis, like typical video games
 *  arrows        Change view angle
 *  page up       Fly Up
 *  page down     Fly Down
 *  ESC    Exit
 */

// Using Class Lib
#include "CSCIx229.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//  Constants
#define MAX_PARTICLES 3000
#define SCREENX       854
#define SCREENY	      480
#define POSX          50
#define POSY          50
#define RAIN          0
#define	HAIL          1
#define SNOW          2

/*  Global Variables  */
int    i, j, k;          //  Loop Vars
// Floor Values
float  r = 1.0, g = 1.0, b = 1.0;
float  groundY = -10.0;
float  groundPts[51][51][3];
float  groundRgb[51][51][4];
// World, Texture, and Weather Values
//   Camera Orientation
double cameraX = 0, cameraY = 0, cameraZ = 0;
//   First Person View Coordinates
double povX = 0, povY = 0, povZ = 2.5;
//   Time and environment values
double timeDiff   = 0.0;  //  Time Value
double realTime   = 0.0;  //  Time Value
float  skyColor   = 0.05;  //  Sky Transition
double fogIntensity = 0.015; //  fog Intensity
int    weatherType  = 0;  //  Weather Type
//   Viewing Elems
int    th         =   0;  //  Azimuth of view angle
int    ph         =   0;  //  Elevation of view angle
int    fov        =  55;  //  Field of view (for perspective)
double asp        =   1;  //  Aspect ratio
double dim        =  10;  //  Size of world
//   Textures & Objects
int    sky[4];            //  Sky textures
int    ground;            //  Ground Textures
int    obj[3];            //  Bird & other Objects
int    flap       =   0;  //  Bird Flap Var
//   Light values, From Example 13
double ambient    = 20.0; //  Ambient Lighting
int    emission   =   0;  //  Emission intensity (%)
int    shininess  =   0;  //  Shininess (power of two)
float  shiny      =   1;  //  Shininess (value)
float  yh         =   0;
//  Translate intensity to color vectors
float Diffuse[]  = {1.00,1.00,1.00,1.0};
float Specular[] = {0.00,0.00,0.00,1.0};
//  Weather Structure
typedef struct
{
  // Life
  int   alive;   // True/False of particle to confirm if still alive
  float life;    // particle lifespan, set to 5 at start and decrements
  float fade;    // Decay rate, random for each particle
  // Positions
  float xpos;
  float ypos;
  float zpos;
  // Velocity/Direction, y dir
  float vel;
  // Gravity
  float gravity;
} particles;
//  Global Paticle System Array for weather particles
particles weatherParticles[MAX_PARTICLES];

/*
 *  Initialize & Reset particle array values
 */
void initParticles()
{
  weatherParticles[i].alive     = 1;
  weatherParticles[i].life      = 5.0;
  weatherParticles[i].fade      = (float)(rand()%100) / 1000.0 + 0.003;
  weatherParticles[i].xpos = (float)(rand()%101) - 50;
  weatherParticles[i].ypos      = 50.0;
  weatherParticles[i].zpos = (float)(rand()%101) - 50;
  weatherParticles[i].vel       = 5.0;
  weatherParticles[i].gravity   = -0.981;
}

/*
 *  Initialize Everything for weather and world floor vertices
 */
void init()
{
  int x, z;

  glShadeModel(GL_SMOOTH);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClearDepth(1.0);

  //  Setting Particle Field Ground and Color
  for (z = 0; z < 51; z++)
  {
    for (x = 0; x < 51; x++)
	{
	  //  Colors
      groundRgb[z][x][0] = r;
      groundRgb[z][x][1] = g;
      groundRgb[z][x][2] = b;
      groundRgb[z][x][3] = 0.0; // Acummulation
    }
  }
  
  for (x = 0; x < 51; x++)
  {
    for (z = 0; z < 51; z++)
	{
	  //  Points
      groundPts[x][z][0] = x - 10.0;
      groundPts[x][z][1] = groundY;
      groundPts[x][z][2] = z - 10.0;
    }
  }

  //  Initializes & Reset Particles. Gives them their attributes
  for (i = 0; i < MAX_PARTICLES; i++)
  {
      initParticles();
  }
}

/* 
 *  Draw sky box, added skybox following feature to first/third person view
 */
static void skyBox(double D, int dayTime)
{
   //  Enable blending
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE);

   //glColor4f(skyColor, skyColor, skyColor, 0.99);
   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);
   
   //  Sides
   if(dayTime == 1) glBindTexture(GL_TEXTURE_2D,sky[0]);
   else glBindTexture(GL_TEXTURE_2D,sky[2]);
   glBegin(GL_QUADS);
     glTexCoord2f(0.00,0); glVertex3f(-D+povX,-D+povY,-D+povZ);
     glTexCoord2f(0.25,0); glVertex3f(+D+povX,-D+povY,-D+povZ);
     glTexCoord2f(0.25,1); glVertex3f(+D+povX,+D+povY,-D+povZ);
     glTexCoord2f(0.00,1); glVertex3f(-D+povX,+D+povY,-D+povZ);

     glTexCoord2f(0.25,0); glVertex3f(+D+povX,-D+povY,-D+povZ);
     glTexCoord2f(0.50,0); glVertex3f(+D+povX,-D+povY,+D+povZ);
     glTexCoord2f(0.50,1); glVertex3f(+D+povX,+D+povY,+D+povZ);
     glTexCoord2f(0.25,1); glVertex3f(+D+povX,+D+povY,-D+povZ);

     glTexCoord2f(0.50,0); glVertex3f(+D+povX,-D+povY,+D+povZ);
     glTexCoord2f(0.75,0); glVertex3f(-D+povX,-D+povY,+D+povZ);
     glTexCoord2f(0.75,1); glVertex3f(-D+povX,+D+povY,+D+povZ);
     glTexCoord2f(0.50,1); glVertex3f(+D+povX,+D+povY,+D+povZ);

     glTexCoord2f(0.75,0); glVertex3f(-D+povX,-D+povY,+D+povZ);
     glTexCoord2f(1.00,0); glVertex3f(-D+povX,-D+povY,-D+povZ);
     glTexCoord2f(1.00,1); glVertex3f(-D+povX,+D+povY,-D+povZ);
     glTexCoord2f(0.75,1); glVertex3f(-D+povX,+D+povY,+D+povZ);
   glEnd();

   //  Top and bottom
   if(dayTime == 1) glBindTexture(GL_TEXTURE_2D,sky[1]);
   else glBindTexture(GL_TEXTURE_2D,sky[3]);
   glBegin(GL_QUADS);
     glTexCoord2f(0.0,0); glVertex3f(+D+povX,+D+povY,-D+povZ);
     glTexCoord2f(0.5,0); glVertex3f(+D+povX,+D+povY,+D+povZ);
     glTexCoord2f(0.5,1); glVertex3f(-D+povX,+D+povY,+D+povZ);
     glTexCoord2f(0.0,1); glVertex3f(-D+povX,+D+povY,-D+povZ);

     glTexCoord2f(1.0,1); glVertex3f(-D+povX,-D+povY,+D+povZ);
     glTexCoord2f(0.5,1); glVertex3f(+D+povX,-D+povY,+D+povZ);
     glTexCoord2f(0.5,0); glVertex3f(+D+povX,-D+povY,-D+povZ);
     glTexCoord2f(1.0,0); glVertex3f(-D+povX,-D+povY,-D+povZ);
   glEnd();
      
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);
}

/* 
 *  Binds Timed Transitions together for smooth transactions, similar to idle, but also calls skyBox to display.
 */
void bindTextures()
{
	//  BOUNDS FOR VARS, incase the timing gets messed up via lag
	if(skyColor < 0.05) skyColor = 0.05;
	if(skyColor > 0.75) skyColor = 0.75;
	if(ambient < 20.0) ambient  = 20.0;
	if(ambient > 80.0) ambient  = 50.0;
	if(fogIntensity > 0.025) fogIntensity = 0.025;
	if(fogIntensity < 0.01) fogIntensity = 0.01;
	
	//  TIMING TRANSITIONS
	if(realTime >= 0 && realTime < 20)
	{
		if(fogIntensity < 0.015) fogIntensity = 0.015;
		skyColor += 0.0005;
		ambient += 0.05;
		fogIntensity += 0.00002;
		skyBox(5*dim, 0);
	}
	else if(realTime >= 20 && realTime < 40)
	{
		if(realTime <= 20.2) weatherType = rand()%3;
		skyBox(5*dim, 1);
		if(fogIntensity > 0.015) fogIntensity -= 0.00008;
	}
	else if(realTime >= 40 && realTime < 60)
	{
		skyColor -= 0.0005;
		ambient -= 0.05;
		fogIntensity += 0.00002;
		skyBox(5*dim, 1);
	}
	else if(realTime >= 60 && realTime < 80)
	{
		if(realTime <= 60.2) weatherType = rand()%3;
		skyBox(5*dim, 0);
		if(fogIntensity > 0.01) fogIntensity -= 0.00008;
	}
	//  CATCH CONDITION FOR GLITCHES & BUGS
	else
	{
		skyBox(5*dim, 0);
		fogIntensity = 0.015;
		ambient = 20;
		skyColor = 0.05;
	}
	
}

/* 
 *  Draws ground & particle field for snow and hail bounce
 */
static void drawGround(double x, double y, double z)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

    //  Save transformation
   glPushMatrix();
   //  Enable blending
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   //  Ground Particle field, transparent mostly until snow hits it or hail bounces off
   glColor3f(r, g, b);
   glBegin(GL_QUADS);
     //  Along z - y const
     for (i = 0; i + 1 < 51; i++)
     {
       //  Along x - y const
       for (j = 0; j + 1 < 51; j++)
	   {
         glColor4fv(groundRgb[i][j]);
         glVertex3f(groundPts[j][i][0],     groundPts[j][i][1],     groundPts[j][i][2] + 1);
         glColor4fv(groundRgb[i][j+1]);
         glVertex3f(groundPts[j+1][i][0],   groundPts[j+1][i][1],   groundPts[j+1][i][2] + 1);
         glColor4fv(groundRgb[i+1][j+1]);
         glVertex3f(groundPts[j+1][i+1][0], groundPts[j+1][i+1][1], groundPts[j+1][i+1][2] + 1);
         glColor4fv(groundRgb[i+1][j]);
         glVertex3f(groundPts[j][i+1][0],   groundPts[j][i+1][1],   groundPts[j][i+1][2] + 1);
		 //  NORMAL VECTOR IS MEAN OF POINTS. AS SNOW BUILDS, THIS LETS MORE LIGHTING HAPPEN ON THE BUMPS AND LESS IN THE PITS
		 glNormal3d((groundPts[j][i][0] + groundPts[j+1][i][0] + groundPts[j+1][i+1][0] + groundPts[j][i+1][0]) / 4, 
					(groundPts[j][i][1] + groundPts[j+1][i][1] + groundPts[j+1][i+1][1] + groundPts[j][i+1][1]) / 4,
					(groundPts[j][i][2] + groundPts[j+1][i][2] + groundPts[j+1][i+1][2] + groundPts[j][i+1][2] + 4) / 4);
       }
     }
   glEnd();
   
   //  Texture
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,ground);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   
   //  Visible ground
   glColor4f(1,1,1,0.9);
   glBegin(GL_QUADS);
     for(i = -x; i < x; i+=10)
     {
       for(j = -z; j < z; j+=10)
	   {
	     glNormal3d(i, y, j);
         glTexCoord2f(0.0,0.0); glVertex3f(i-5, y, j+5);
         glTexCoord2f(1.0,0.0); glVertex3f(i+5, y, j+5);
         glTexCoord2f(1.0,1.0); glVertex3f(i+5, y, j-5);
         glTexCoord2f(0.0,1.0); glVertex3f(i-5, y, j-5);
	   }
     }
   glEnd();
   
   //  Undo transformations and textures
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
   glDisable(GL_BLEND);
}

/* 
 *  Draws Rain
 */
void drawRain()
{
  float x, y, z;
  //  Set specular color to white
  float white[] = {1,1,1,1};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
  
  //  Save transformation
  glPushMatrix();
  //  Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  
  for (i = 0; i < MAX_PARTICLES; i += 2)
  {
    if (weatherParticles[i].alive == 1)
	{
      x = weatherParticles[i].xpos;
      y = weatherParticles[i].ypos;
      z = weatherParticles[i].zpos + 1;

      //  Draw particles
      glColor4f(0.5, 0.5, 1.0, 0.5);
      glBegin(GL_LINES);
      glNormal3d(x, y + 0.25, z);
	  glVertex3f(x, y, z);
      glVertex3f(x, y + 0.7, z);
      glEnd();
	  
      //  Update values and Move
      weatherParticles[i].ypos += 20*weatherParticles[i].vel / (2000);
      weatherParticles[i].vel += weatherParticles[i].gravity;
	  
      //  Decay Lifespan
      weatherParticles[i].life -= weatherParticles[i].fade;

      if (weatherParticles[i].ypos <= -10)
	    weatherParticles[i].life = -1.0;
		
      //  Reset Particle
      if (weatherParticles[i].life < 0.0) initParticles();
    }
  }
  
  //  Undo transformations and textures
  glPopMatrix();
  glDisable(GL_BLEND);
}

/* 
 *  Draws Hail, similar to rain, with different shapes, color, speed, and bouncing
 */
void drawHail()
{
  float x, y, z;
  //  Set specular color to white
  float white[] = {1,1,1,1};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
  
  //  Save transformation
  glPushMatrix();
  //  Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   
  for (i = 0; i < MAX_PARTICLES-1000; i += 2)
  {
    if (weatherParticles[i].alive == 1)
	{
      x = weatherParticles[i].xpos;
      y = weatherParticles[i].ypos;
      z = weatherParticles[i].zpos + 1;

      // Draw particles
      glColor4f(0.8, 0.8, 0.9, 0.5);
      glBegin(GL_QUADS);
        // Front
		glNormal3d(x, y, z+0.1);
        glVertex3f(x-0.1, y-0.1, z+0.1); // lower left
        glVertex3f(x-0.1, y+0.1, z+0.1); // upper left
        glVertex3f(x+0.1, y+0.1, z+0.1); // upper right
        glVertex3f(x+0.1, y-0.1, z+0.1); // lower left
        //Left
		glNormal3d(x-0.1, y, z);
        glVertex3f(x-0.1, y-0.1, z+0.1);
        glVertex3f(x-0.1, y-0.1, z-0.1);
        glVertex3f(x-0.1, y+0.1, z-0.1);
        glVertex3f(x-0.1, y+0.1, z+0.1);
        // Back
		glNormal3d(x, y, z-0.1);
        glVertex3f(x-0.1, y-0.1, z-0.1);
        glVertex3f(x-0.1, y+0.1, z-0.1);
        glVertex3f(x+0.1, y+0.1, z-0.1);
        glVertex3f(x+0.1, y-0.1, z-0.1);
        //Right
		glNormal3d(x+0.1, y, z);
        glVertex3f(x+0.1, y+0.1, z+0.1);
        glVertex3f(x+0.1, y+0.1, z-0.1);
        glVertex3f(x+0.1, y-0.1, z-0.1);
        glVertex3f(x+0.1, y-0.1, z+0.1);
        //Top
		glNormal3d(x, y+0.1, z);
        glVertex3f(x-0.1, y+0.1, z+0.1);
        glVertex3f(x-0.1, y+0.1, z-0.1);
        glVertex3f(x+0.1, y+0.1, z-0.1);
        glVertex3f(x+0.1, y+0.1, z+0.1);
        //Bottom
		glNormal3d(x, y-0.1, z);
        glVertex3f(x-0.1, y-0.1, z+0.1);
        glVertex3f(x-0.1, y-0.1, z-0.1);
        glVertex3f(x+0.1, y-0.1, z-0.1);
        glVertex3f(x+0.1, y-0.1, z+0.1);
      glEnd();
	  
      // Update values and Move
      if (weatherParticles[i].ypos <= -10)
	  {
        weatherParticles[i].vel = weatherParticles[i].vel * -1.0;
		weatherParticles[i].life = 0.06;
	  }
		
      weatherParticles[i].ypos += 30*weatherParticles[i].vel / (2000); // * 1000
      weatherParticles[i].vel += weatherParticles[i].gravity;

      // Decay lifespan
      weatherParticles[i].life -= weatherParticles[i].fade;

      // Reset particle
      if (weatherParticles[i].life < 0.0) initParticles();
    }
  }
  
  //  Undo transformations and textures
  glPopMatrix();
  glDisable(GL_BLEND);
}

/* 
 *  Draws Snow, similar to hail, but slow and effects particle field ground to make snow floors
 */
void drawSnow()
{
  float x, y, z;
  int xi, zi;
  //  Set specular color to white
  float white[] = {1,1,1,1};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
  
  //  Save transformation
  glPushMatrix();
  //  Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  
  for (i = 0; i < MAX_PARTICLES - 2000; i += 2)
  {
    if (weatherParticles[i].alive == 1)
	{
	  // locate positions
      x = weatherParticles[i].xpos;
      y = weatherParticles[i].ypos;
      z = weatherParticles[i].zpos + 1;

      // Draw particles
      glColor4f(1.0, 1.0, 1.0, 0.5);
      glPushMatrix();
      glTranslatef(x, y, z);
      glutSolidSphere(0.2, 16, 16);
      glPopMatrix();
	  
      // Update values and Move
      weatherParticles[i].ypos += 5*weatherParticles[i].vel / (2000);
      weatherParticles[i].vel += weatherParticles[i].gravity;
	  
      // Decay lifespan
      weatherParticles[i].life -= weatherParticles[i].fade;

      // Change particle field for snow ground
      if (weatherParticles[i].ypos <= -10)
	  {
	    if(x <= 26 && x >= -25 && z <= 27 && z >= -24)
		{
          zi = z - 1 + 25;
          xi = x + 25;
          groundRgb[zi][xi][0] = 1.0;
          groundRgb[zi][xi][2] = 1.0;
          groundRgb[zi][xi][3] += 1.1;
          if (groundRgb[zi][xi][3] > 1.0)
            groundPts[xi][zi][1] += 0.5;
        }
		weatherParticles[i].life = -1.0;
      }

      // Reset particle
      if (weatherParticles[i].life < 0.0) initParticles();
    }
  }
  //  Undo transformations and textures
  glPopMatrix();
  glDisable(GL_BLEND);
}

/* 
 *  Draws Bird and binds it to camera
 */
void drawBird()
{
   //  Screen edge
   //float w = asp>2 ? asp : 2;
	  
   //  Save transform attributes (Matrix Mode and Enabled Modes)
   glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
   //  Save projection matrix and set unit transform
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(-asp,+asp,-1,1,-1,1);
   //  Save model view matrix and set to indentity
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   
   //  Draw the model
   glPushMatrix();
   glColor3f(0.63, 0.59, 0.52);
   glScaled(1.3,1.3,1.3);
   glTranslated(-0.5,-0.5,0);
   glRotated(170,0,100,0);
   glCallList(obj[flap]);
   glPopMatrix();
  
   //  Reset model view matrix
   glPopMatrix();
   //  Reset projection matrix
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   //  Pop transform attributes (Matrix Mode and Enabled Modes)
   glPopAttrib();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
  //  Lighting local variables for position and ambient light
  float Ambient[]  = {0.01*ambient,0.01*ambient,0.01*ambient,1.0};
  float Position[] = {500*Sin(yh-30),500*Cos(yh-30),povZ,1.0};
  
  //  Set background color behind skybox for added lighting transition illusion
  glClearColor(skyColor, skyColor, skyColor, 1.0);

  // Ajust the camera vector based on rotation
  cameraX = +2*dim*Sin(th)*Cos(ph);
  cameraY = +2*dim        *Sin(ph);
  cameraZ = -2*dim*Cos(th)*Cos(ph);;
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  //  FOG
  GLfloat fogColor[4] = {skyColor, skyColor, skyColor, 0.01f}; 
  glFogi(GL_FOG_MODE,  GL_EXP2);
  glFogfv(GL_FOG_COLOR, fogColor); 
  glFogf(GL_FOG_DENSITY, fogIntensity); 
  glHint(GL_FOG_HINT, GL_DONT_CARE); 
  glFogf(GL_FOG_START, 10.0f); // Fog Start Depth 
  glFogf(GL_FOG_END, 100.0f); // Fog End Depth
  glEnable(GL_FOG);
  
  //  Enable Z-buffering in OpenGL
  glEnable(GL_DEPTH_TEST);
  //  Use gluLookAt, y is the up-axis 
  gluLookAt(povX,povY,povZ, cameraX+povX,cameraY+povY,cameraZ+povZ, 0,Cos(ph),0);
  //  Sky Box
  bindTextures();
  
  //  LIGHTING
    //  OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    //  Enable lighting
    glEnable(GL_LIGHTING);
    //  Location of viewer for specular calculations
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);
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
		
  //  Draws Ground and Particle Field for Snow and Hail
  drawGround(91, groundY + 0.1, 91);
  
  //  Draws All trees for forest scene
   glPushMatrix();
   glColor3f(0.07,0.277,0.005);
   glScaled(5,5,5);
   glTranslated(0,-2,3);
   glRotated(0,0,1,0);
   glCallList(obj[2]);
   glPopMatrix();
  
  // Draws Weather Depending on Setting
  if      (weatherType == RAIN) drawRain();
  else if (weatherType == HAIL) drawHail();
  else if (weatherType == SNOW) drawSnow();
  // Draws bird
  drawBird();
  
  //  Disable Z-buffering & Lighting in OpenGL
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  //  Render the scene and make it visible
  ErrCheck("display");
  glFlush();
  glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key, int x, int y)
{
   //  Azimuth by 5 degrees
   if (key == GLUT_KEY_RIGHT)     th += 5;
   else if (key == GLUT_KEY_LEFT) th -= 5;
   //  Elevation by 5 degrees
   else if (key == GLUT_KEY_UP && ph < 90)    ph += 5;
   else if (key == GLUT_KEY_DOWN && ph > -90) ph -= 5;
   //  Page keys change camera height
   else if (key == GLUT_KEY_PAGE_UP && povY < 40)    povY += 0.5;
   else if (key == GLUT_KEY_PAGE_DOWN && povY > -5) povY -= 0.5;
   
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   
   //  Update projection
   Project(fov,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/* 
 *  GLUT calls this routine when other keys are pressed
 */
void key(unsigned char ch, int x, int y)
{
  //  Exit on ESC
  if(ch == 27) exit(0);
  //  Weather Controls
  if(ch == '1') weatherType = RAIN;
  if(ch == '2') weatherType = HAIL;
  if(ch == '3') weatherType = SNOW;
  //  WASD Traversal
  if(ch == 'w' || ch == 'W')
  {
     povX += cameraX * 0.05;
	 povZ += cameraZ * 0.05;
  }
  else if (ch == 's' || ch == 'S')
  {
     povX -= cameraX * 0.05;
	 povZ -= cameraZ * 0.05;
  }
  else if (ch == 'a' || ch == 'A')
  {
     povX += cameraZ * 0.05;
	 povZ -= cameraX * 0.05;
  }
  else if (ch == 'd' || ch == 'D')
  {
     povX -= cameraZ * 0.05;
	 povZ += cameraX * 0.05;
  }
  //  Keeps Pov camera within bounds for stablity
  if (povX > 50)  povX = 50;
  if (povX < -50) povX = -50;
  if (povZ > 50)  povZ = 50;
  if (povZ < -50) povZ = -50;
  
  glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width, int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(fov,asp,dim);
}

/*
 *  GLUT calls this routine when there is nothing else to do
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   realTime = fmod(glutGet(GLUT_ELAPSED_TIME)/1000.0, 80);
   yh = fmod(1.5*t,120);
   flap = fmod((int)t, 2);
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc, char** argv)
{
   glutInit(&argc,argv);
   glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(SCREENX, SCREENY);
   glutInitWindowPosition(POSX, POSY);
   glutCreateWindow("Peter Huynh - Final Project: Bird, Weather and Time");
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   
   //  Load files and record time
   sky[0]   = LoadTexBMP("assets/SkyDay0.bmp");
   sky[1]   = LoadTexBMP("assets/SkyDay1.bmp");
   sky[2]   = LoadTexBMP("assets/SkyNight0.bmp");
   sky[3]   = LoadTexBMP("assets/SkyNight1.bmp");
   ground   = LoadTexBMP("assets/GroundGrass0.bmp");
   obj[0]   = LoadOBJ("bird1.obj");
   obj[1]   = LoadOBJ("bird2.obj");
   obj[2]   = LoadOBJ("tree1.obj");
   
   glutIdleFunc(idle);
   ErrCheck("init");
   init();
   glutMainLoop();
   return 0;
}
