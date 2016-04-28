/**************************************************************************************
 * This class only handles the openGL part
 *
 *
 **************************************************************************************/

#include "myglwidget.h"
#include <QtWidgets>
#include <QtOpenGL>
#include <GL/glu.h>
#include <QDebug>
#include <QFile>
#include <QImage>


GLuint texture[3];
GLuint texturePlanet[1];
GLUquadric* q;


MyGLWidget::MyGLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{

    //starting angles to draw the cube - its orientation
    setGreekAngles(1, 90);
    setGreekAngles(2,1);
    setGreekAngles(3,1);

}

/*
float MyGLWidget::getCubeAngle(int xyzAngle)
{
    if(xyzAngle==1){
        return xAngle;}

    if(xyzAngle==2){
        return yAngle;
    }

    if(xyzAngle==3){
        return zAngle;
    }
}

void MyGLWidget::setCubeAngle(float angle, int xyzAngle)
{

    if(xyzAngle==1){
    xAngle = angle;
    }
    if(xyzAngle==2){
        yAngle = angle;
    }
    if(xyzAngle==3){
        zAngle = angle;
    }

    qDebug("angle set\n");
    updateGL();
}*/

void MyGLWidget::initializeGL()
{
        TextureMappingCubeSat();                            //load the image for TM + necessary setup
        textureMappingPlanet();                             //load the image for TM + necessary setup

        glScalef(1,1,1);                                   // to make sure x, y and z axis are 1-1-1
      //  glEnable(GL_TEXTURE_2D);							// enable texture mapping - sun and cubesat already done in function
        glShadeModel(GL_SMOOTH);							// enable smooth shading - for nice colors by interpolation
        glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// black background setup - what color well be when you clear - this line doenst clear anything
        glClearDepth(1.0f);									// Depth Buffer Setup ----NECESSARY??
        glEnable(GL_DEPTH_TEST);							// Enables Depth Testing ----NECESSARY??
        glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do ----NECESSARY??
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations ----NECESSARY??

}

/*******************************************************************************************/
// Places specified texture on the surface of the polygon
//

void MyGLWidget::textureMappingPlanet()
{
    QImage t;
    QImage b;
\
//    if(!b.load("C:/Users/Yalishand/Desktop/fftestGL/images_openGL/sun.tga"))

    if(!b.load("C:/Users/Yalishand/Desktop/fftestGL/images_openGL/sunbmp.bmp"))
    {qDebug("error with image1\n");}

    t = QGLWidget::convertToGLFormat(b);
    glGenTextures( 1, &texturePlanet[0] );
    glBindTexture( GL_TEXTURE_2D, texturePlanet[0] );
    glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

}


/*******************************************************************************************************************/
// this function will load 3 different self made images to represent the cubesatn which is composed of 2 stacked
// cubes. Thos images will just cover the cubes

void MyGLWidget::TextureMappingCubeSat()
{
        QImage t;
        QImage b;

        if(!b.load("C:/Users/Yalishand/Desktop/fftestGL/images_openGL/cubesatPanels.png"))
        {qDebug("error with image2\n");}

        t = QGLWidget::convertToGLFormat(b);
        glGenTextures( 1, &texture[0] );
        glBindTexture( GL_TEXTURE_2D, texture[0] );
        glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

/***********************************************/
        QImage t2;
        QImage b2;

        if(!b2.load("C:/Users/Yalishand/Desktop/fftestGL/images_openGL/cubesatNoPanels.png"))
        {qDebug("error with image3\n");}


        t2 = QGLWidget::convertToGLFormat(b2);
        glGenTextures( 1, &texture[1] );
        glBindTexture( GL_TEXTURE_2D, texture[1] );
        glTexImage2D( GL_TEXTURE_2D, 0, 3, t2.width(), t2.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t2.bits() );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );


/***********************************************/
         QImage t3;
         QImage b3;

         if(!b3.load("C:/Users/Yalishand/Desktop/fftestGL/images_openGL/cubesatUpperPart.png"))
         {qDebug("error with image 4\n");}


         t3 = QGLWidget::convertToGLFormat(b3);
         glGenTextures( 1, &texture[2] );
         glBindTexture( GL_TEXTURE_2D, texture[2]);
         glTexImage2D( GL_TEXTURE_2D, 0, 3, t3.width(), t3.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t3.bits() );
         glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
         glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}


/******************************************************************************************/
// This function draws the two stages of the cubesat and adds the texture to it
//

void MyGLWidget::drawCube()
{
  //  glPushMatrix(); //transformatie matrix ongedeerd laten


    gluLookAt(position,0.5,-0.1,-0.5,-0.5,0,0,0,1); //changecamera angle


    //keep in mind this part is done the other way around
    glTranslatef( 0.8, -0.4, 0.0); // places the cube op de afbeelding op de x,y en z positie
    glRotatef(getGreekAngles(1), 1.0f, 0.0f, 0.0f);
    glRotatef(getGreekAngles(2), 0.0f, 1.0f, 0.0f);
    glRotatef(getGreekAngles(3), 0.0f, 0.0f, 1.0f);
    glTranslatef(0, 0, 0);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

   /* second stage*/
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glEnable(GL_TEXTURE_2D); // enable texture mapping - sun and cubesat
    glBegin(GL_QUADS); //drawing a polygon - this will create a square

   //back
     glTexCoord2f(0.0, 1.0); glVertex3f(-0.1, 0.3,-0.1 );//upper left corner
     glTexCoord2f(1.0, 1.0); glVertex3f(0.1, 0.3,-0.1); //uper right
     glTexCoord2f(1.0, 0.0); glVertex3f(0.1,0.1,-0.1); // down right
     glTexCoord2f(0.0, 0.0); glVertex3f(-0.1,0.1,-0.1); // down left

     //right
     glTexCoord2f(1.0, 1.0); glVertex3f(0.1,0.3,-0.1); //back up
     glTexCoord2f(1.0, 0.0); glVertex3f(0.1,0.1,-0.1); //back down
     glTexCoord2f(0.0, 0.0); glVertex3f(0.1,0.1,0.1);//front  d
     glTexCoord2f(0.0, 1.0); glVertex3f(0.1,0.3,0.1);//front up

     //left
     glTexCoord2f(0.0, 1.0); glVertex3f(-0.1,0.3,-0.1); //back up
     glTexCoord2f(0.0, 0.0); glVertex3f(-0.1,0.1,-0.1); //back down
     glTexCoord2f(1.0, 0.0); glVertex3f(-0.1,0.1,0.1);//front  d
     glTexCoord2f(1.0, 1.0); glVertex3f(-0.1,0.3,0.1);//front up

    //front
     glTexCoord2f(0.0, 1.0); glVertex3f(-0.1, 0.3,0.1 );//upper left corner
     glTexCoord2f(1.0, 1.0); glVertex3f(0.1, 0.3,0.1); //uper right
     glTexCoord2f(1.0, 0.0); glVertex3f(0.1,0.1,0.1 ); // down right
     glTexCoord2f(0.0, 0.0); glVertex3f(-0.1,0.1,0.1); // down left

    glEnd();

   glBindTexture(GL_TEXTURE_2D, texture[2]);

   glBegin(GL_QUADS);
   glTexCoord2f(0.0, 1.0); glVertex3f(-0.1,0.3,-0.1); //left back
   glTexCoord2f(1.0, 1.0); glVertex3f(0.1,0.3,-0.1); //right back
   glTexCoord2f(1.0, 0.0); glVertex3f(0.1,0.3,0.1);//right front
   glTexCoord2f(0.0, 0.0); glVertex3f(-0.1,0.3,0.1);

    glEnd();




    /*first stage*/
   //glPushMatrix();
     glBindTexture(GL_TEXTURE_2D, texture[0]);
     glBegin(GL_QUADS);
       //back
      glTexCoord2f(0.0, 1.0); glVertex3f(-0.1, 0.1,-0.1 );//upper left corner
      glTexCoord2f(1.0, 1.0); glVertex3f(0.1, 0.1,-0.1); //uper right
      glTexCoord2f(1.0, 0.0); glVertex3f(0.1,-0.1,-0.1); // down right
      glTexCoord2f(0.0, 0.0); glVertex3f(-0.1,-0.1,-0.1); // down left


       //right
       glTexCoord2f(1.0, 1.0); glVertex3f(0.1,0.1,-0.1); //back up
       glTexCoord2f(1.0, 0.0); glVertex3f(0.1,-0.1,-0.1); //back down
       glTexCoord2f(0.0, 0.0); glVertex3f(0.1,-0.1,0.1);//front  d
       glTexCoord2f(0.0, 1.0); glVertex3f(0.1,0.1,0.1);//front up

       //left
       glTexCoord2f(0.0, 1.0); glVertex3f(-0.1,0.1,-0.1); //back up
       glTexCoord2f(0.0, 0.0); glVertex3f(-0.1,-0.1,-0.1); //back down
       glTexCoord2f(1.0, 0.0); glVertex3f(-0.1,-0.1,0.1);//front  d
       glTexCoord2f(1.0, 1.0); glVertex3f(-0.1,0.1,0.1);//front up

      glEnd();

       //top
       glBindTexture(GL_TEXTURE_2D, texture[2]);

       glBegin(GL_QUADS);
       glTexCoord2f(0.0, 1.0); glVertex3f(-0.1,0.1,-0.1); //left back
       glTexCoord2f(1.0, 1.0); glVertex3f(0.1,0.1,-0.1); //right back
       glTexCoord2f(1.0, 0.0); glVertex3f(0.1,0.1,0.1);//right front
       glTexCoord2f(0.0, 0.0); glVertex3f(-0.1,0.1,0.1);
       //bottom
       glTexCoord2f(0.0, 1.0); glVertex3f(-0.1,-0.1,-0.1); //left back
       glTexCoord2f(1.0, 1.0); glVertex3f(0.1,-0.1,-0.1); //right back
       glTexCoord2f(1.0, 0.0); glVertex3f(0.1,-0.1,0.1);//right front
       glTexCoord2f(0.0, 0.0); glVertex3f(-0.1,-0.1,0.1);
       glEnd();


       glBindTexture(GL_TEXTURE_2D, texture[0]);
       glBegin(GL_QUADS);
       //front
       glTexCoord2f(0.0, 1.0); glVertex3f(-0.1, 0.1,0.1 );//upper left corner
       glTexCoord2f(1.0, 1.0); glVertex3f(0.1, 0.1,0.1); //uper right
       glTexCoord2f(1.0, 0.0); glVertex3f(0.1,-0.1,0.1 ); // down right
       glTexCoord2f(0.0, 0.0); glVertex3f(-0.1,-0.1,0.1); // down left
       glEnd();

      glFlush();  //causes all rest commands to be executed
  //    glPopMatrix();
      glDisable(GL_TEXTURE_2D);
  //    glPopMatrix();
}


void MyGLWidget::drawSun()
{
   glEnable(GL_TEXTURE_2D);//
   glPushMatrix();
// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity(); //move back to the center of the screen/origin 0,0,0
   glBindTexture(GL_TEXTURE_2D, texturePlanet[0]);

   glPushMatrix();

   glScalef(1,1,1);
   glLoadIdentity();
   GLUquadricObj *quadric=gluNewQuadric();
   gluQuadricTexture(quadric, GLU_TRUE);
   gluQuadricNormals(quadric, GLU_SMOOTH);

   glEnable(GL_TEXTURE_2D); //
   glBindTexture(GL_TEXTURE_2D,texturePlanet[0]);//

   gluSphere(quadric, 0.50, 360,360);
   glDisable(GL_TEXTURE_2D);//

   gluDeleteQuadric(quadric);
  glPopMatrix();
    glDisable(GL_TEXTURE_2D);

}


void MyGLWidget::drawOrbit()
    {

    glEnable(GL_TEXTURE_2D);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
    glLoadIdentity();


    glPushMatrix();

    glRotatef(130, 1.0f, 0.0f, 0.0f); //roteert de orbit rond de x-as
    glRotatef(15, 0.0f, 1.0f, 0.0f); //roteert de orbit rond de x-as


    glBegin(GL_LINE_STRIP);
   glColor3f(255,255,255);

    // loop round from 0 to 2*PI and draw around the radius of the orbit using trigonometry
    for (float angle = 0.0f; angle < 6.283185307f; angle += 0.1f)
    {
        glTranslatef( 0.8, -0.4, 0.0);
        glVertex3f(sin(angle) * 1.4, cos(angle) * 1.2, 0);
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();

}



/**********************************************************************************************/
// overriden protected function which is composed of 3 self-written functions
// 1 drawing the sun, 1 drawing the CubeSat and, 1 drawing the orbit

void MyGLWidget::paintGL()
{

    //setLight();
  drawOrbit();
   drawCube();
   drawSun();


}

void MyGLWidget::setLight()
{
           // Prepare light parameters.
           float SHINE_ALL_DIRECTIONS = 1;
           float lightPos[4] = {-30, 0, 0, SHINE_ALL_DIRECTIONS};
           float lightColorAmbient[4] = {200, 200, 200, 1};
           float lightColorSpecular[4] = {255, 255, 255, 1};

           // Set light parameters.
           glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
           glLightfv(GL_LIGHT1, GL_AMBIENT, lightColorAmbient);
           glLightfv(GL_LIGHT1, GL_SPECULAR, lightColorSpecular);

           // Enable lighting in GL.
           glEnable(GL_LIGHT1);
           glEnable(GL_LIGHTING);

           // Set material properties.
           float rgba[3] = {0.3, 0.5, 1};
           glMaterialfv(GL_FRONT, GL_AMBIENT, rgba);
           glMaterialfv(GL_FRONT, GL_SPECULAR, rgba);
           glMaterialf(GL_FRONT, GL_SHININESS, 0.5f);
}

void MyGLWidget::setGreekAngles(int letter, float angle)
{
       if(letter==1)
       {
           theta = angle;

       }
       else if(letter==2)
       {
           epsilon = angle;
       }
       else if(letter == 3)
       {
           phi = angle;
       }
}


float MyGLWidget::getGreekAngles(int letter)
{
    if(letter==1)
    {return theta;}


    if(letter==2)
    {
        return epsilon;
    }

    if (letter==3)
    {
        return phi;
    }
}

void MyGLWidget::resizeGL(int width, int height)
{
   if (height==0)										// Prevent A Divide By Zero By
    {
        height=1;										// Making Height Equal One
    }

   glViewport(0,0,width,height);						// Reset The Current Viewport

   glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
   glLoadIdentity();									// Reset The Projection Matrix
   glScalef(height *1./width, 1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
    glLoadIdentity();

}


