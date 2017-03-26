#ifndef EQUIRETANGULARPROJECTION_H
#define EQUIRETANGULARPROJECTION_H

#include <QGLWidget>
#include <QOpenGLFunctions_3_3_Compatibility>
#include "shader.h"
#include <QMouseEvent>

class EquiretangularProjection:public QGLWidget, protected QOpenGLFunctions_3_3_Compatibility
{
public:
    EquiretangularProjection(QWidget* parent = 0);
    bool drawSphere;

private:
    GLuint VBO, VAO, EBO;
    GLuint texture_src;
    GLuint texture_logo;
    GLuint texture_res;
    Shader* shader_cal;
    Shader* shader_dis;//display
    Shader* shader_sph;//sphere
    int width, height;
    int w_logo,h_logo;
    GLuint FBO;
    //sphere
    GLUquadricObj *sphere;
    GLfloat xRot, zRot;
    GLfloat xRot0, zRot0;
    GLfloat zoom;
    int x0;
    int y0;
    float radius;
    void mousePressEvent ( QMouseEvent * e );
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent ( QMouseEvent * e );
protected:
    void initializeGL();
    void paintGL();
    void resizeGL( int screen_width, int screen_height  );

};

#endif // EQUIRETANGULARPROJECTION_H
