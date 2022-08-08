#ifndef LIFT_H
#define LIFT_H

#include <QMainWindow>
#include <QScopedPointer>
#include "liftcontroller.h"

namespace Ui {
class Lift;
}

class Lift : public QMainWindow
{
    Q_OBJECT

    LiftController controller;
public:
    explicit Lift(QWidget *parent = nullptr);
    ~Lift() override;

private:
    QScopedPointer<Ui::Lift> m_ui;

private slots:
    void handleFloor(bool, int);
    void handleDoor(bool, bool);
    void handleDirection(bool, Direction);
    void handleOutsideWork();
    void handleInsideWork();
};

#endif // LIFT_H
