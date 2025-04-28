#include "myopenglclass.h"

MyOpenGLClass::MyOpenGLClass(QWidget *parent): QOpenGLWidget(parent)
{

}

void MyOpenGLClass::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0.0, 200.0, 0.0, 150.0, 0.0, 1.0);
}

void MyOpenGLClass::resizeGL(int w, int h)
{

}

void MyOpenGLClass::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.0, 0.4, 0.2);
    glBegin(GL_LINES);
        glVertex2i(180, 15);
        glVertex2i(10, 145);
    glEnd();

    glFlush();
}
