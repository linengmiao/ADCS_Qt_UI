#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H


#include <QGLWidget>
#include "widget.h"

//class MyGLWidget;

class MyGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = 0);
    void repositionCubeAnimation(double);
    void TextureMappingCubeSat();
    void textureMappingPlanet();

   // float getCubeAngle(int xyzAngle);
   // void setCubeAngle(float, int xyzAngle);
    void drawSun();
    void drawCube();
    void drawOrbit();

    void setLight();

    float angle,x1,y1,x2,y2;
    double radius;
    double position;

    void setGreekAngles(int letter, float angle);
    float getGreekAngles(int letter);

public slots:


protected:
   void initializeGL();
   void paintGL();
   void resizeGL(int width, int height);

private:

    float xAngle;
    float yAngle;
    float zAngle;

    float theta;
    float epsilon;
    float phi;

    int xRot;
    int yRot;
    int zRot;

    MyGLWidget * mijntussenPtr;

};

#endif // MYGLWIDGET_H
