#ifndef LIFTCONTROLLER_H
#define LIFTCONTROLLER_H

#include <qobject.h>
#include <QTimer>
#include <QList>
#include <optional>

/**
 * @todo write docs
 */
enum class Direction
{
    Up, Down, Stop
};

namespace LiftSpace {
    class Lift: public QObject
    {
        Q_OBJECT

        virtual void timerEvent(QTimerEvent *event) override;
        Direction liftDirection;
        int door;
        int floor, maxFloor, doorCount;
        int timeId;
    public:
        Lift(int maxFloor = 33) :maxFloor(maxFloor)
        {
            floor = 1;
            door = 0;
            liftDirection = Direction::Stop;
            timeId = startTimer(1500);
        }
        void SetLiftDirection(Direction liftDirection);
        void OpenDoor();
        void CloseDoor();
        int GetFloor() { return floor; }
        int GetMaxFloor() { return maxFloor; }
        bool GetDoor() { return door; }
        Direction GetDirection() { return liftDirection; }
    signals:
        void floorChanged(int);
        void doorChanged(bool);
        void directionChanged(Direction);
    };
}



class LiftController : public QObject
{
    Q_OBJECT

    LiftSpace::Lift lift[2];
    std::optional<Direction> queue[33][2];
    int queueCount[2];

    bool selectLiftToHandle(int floor, Direction direction);
    void addTask(bool lift, int floor, Direction task);
    bool shouldContinue(int whichLift);
    void handleFloor(int whichLift, int floor);

public:
    LiftController();
    void outsideTask(int floor, Direction direction);
    void insideTask(bool whichLift, int floor);
signals:
    void floorChanged(bool, int);
    void doorChanged(bool, bool);
    void directionChanged(bool, Direction);
};

#endif // LIFTCONTROLLER_H
