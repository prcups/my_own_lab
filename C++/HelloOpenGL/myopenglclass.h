#ifndef MYOPENGLCLASS_H
#define MYOPENGLCLASS_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>

class MyOpenGLClass : public QOpenGLWidget, QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    explicit MyOpenGLClass(QWidget *parent = nullptr);
protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
};

#endif // MYOPENGLCLASS_H
