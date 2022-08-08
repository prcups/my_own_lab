#include "lift.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Lift w;
    w.show();

    return app.exec();
}

