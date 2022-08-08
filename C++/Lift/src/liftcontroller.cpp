#include "liftcontroller.h"
#include <QDebug>

void LiftSpace::Lift::CloseDoor()
{
    door = 0;
    emit doorChanged(0);
}

void LiftSpace::Lift::OpenDoor()
{
    door = 6;
    emit doorChanged(1);
}

void LiftSpace::Lift::SetLiftDirection(Direction liftDirection)
{
    if ((floor == maxFloor && liftDirection == Direction::Up)
            || (floor == 1 && liftDirection == Direction::Down))
            return;
    this->liftDirection = liftDirection;
    emit directionChanged(liftDirection);
}


void LiftSpace::Lift::timerEvent(QTimerEvent *event Q_DECL_UNUSED)
{
    if (door) {
        --door;
        if (door == 0) emit doorChanged(0);
        return;
    }
    switch (liftDirection) {
    case Direction::Up: {
        ++floor;
        if (floor == maxFloor) {
            liftDirection = Direction::Stop;
            emit directionChanged(liftDirection);
        }
        emit floorChanged(floor);
        break;
    }
    case Direction::Down: {
        --floor;
        if (floor == 1) {
            liftDirection = Direction::Stop;
            emit directionChanged(liftDirection);
        }
        emit floorChanged(floor);
        break;
    }
    default:
        break;
    }
}


LiftController::LiftController()
{
    queueCount[0] = queueCount[1] = 0;
    connect(&lift[0], &LiftSpace::Lift::floorChanged, this, [ = ](int x) {
        emit LiftController::floorChanged(0, x);
        handleFloor(0, x);
    });
    connect(&lift[1], &LiftSpace::Lift::floorChanged, this, [ = ](int x) {
        emit LiftController::floorChanged(1, x);
        handleFloor(1, x);
    });
    connect(&lift[0], &LiftSpace::Lift::doorChanged, this, [ = ](bool x) {
        emit LiftController::doorChanged(0, x);

    });
    connect(&lift[1], &LiftSpace::Lift::doorChanged, this, [ = ](bool x) {
        emit LiftController::doorChanged(1, x);
    });
    connect(&lift[0], &LiftSpace::Lift::directionChanged, this, [ = ](Direction x) {
        emit LiftController::directionChanged(0, x);
    });
    connect(&lift[1], &LiftSpace::Lift::directionChanged, this, [ = ](Direction x) {
        emit LiftController::directionChanged(1, x);
    });
}


void LiftController::outsideTask(int floor, Direction direction)
{
    for (int i = 0; i < 2; ++i)
        if (lift[i].GetFloor() == floor) {
            lift[i].SetLiftDirection(Direction::Stop);
            lift[i].OpenDoor();
            return;
        }
    bool whichLift = selectLiftToHandle(floor, direction);
    addTask(whichLift, floor, direction);
}

void LiftController::insideTask(bool whichLift, int floor)
{
    addTask(whichLift, floor, Direction::Stop);
}

void LiftController::addTask(bool whichLift, int floor, Direction direction)
{
    if (!queue[floor - 1][whichLift]) ++queueCount[whichLift];
    queue[floor - 1][whichLift] = direction;
    if (lift[whichLift].GetDirection() == Direction::Stop) lift[whichLift].SetLiftDirection(floor > lift[whichLift].GetFloor() ? Direction::Up : Direction::Down);
}


bool LiftController::selectLiftToHandle(int floor, Direction direction)
{
    int distance[2];
    for (int i = 0; i < 2; ++i) {
        if (lift[i].GetDirection() == direction || lift[i].GetDirection() == Direction::Stop) distance[0] = abs(floor - lift[i].GetFloor());
        else {
            if (direction == Direction::Up) {
                distance[i] = (lift[i].GetFloor() - 1) + (floor - 1);
            } else {
                distance[i] = (lift[i].GetMaxFloor() - lift[i].GetFloor()) + (lift[i].GetMaxFloor() - floor);
            }
        }
    }
    return distance[0] > distance[1];
}

void LiftController::handleFloor(int whichLift, int floor)
{
    if (queue[floor - 1][whichLift]) {
        if (shouldContinue(whichLift)) {
            if (queue[floor - 1][whichLift] == Direction::Stop
                    || queue[floor - 1][whichLift] == lift[whichLift].GetDirection()) {
                lift[whichLift].OpenDoor();
                queue[floor - 1][whichLift].reset();
                --queueCount[whichLift];
            }
        } else {
            lift[whichLift].OpenDoor();
            queue[floor - 1][whichLift].reset();
            --queueCount[whichLift];
            if (queueCount[whichLift] == 0) lift[whichLift].SetLiftDirection(Direction::Stop);
            else lift[whichLift].SetLiftDirection(
                lift[whichLift].GetDirection() ==  Direction::Up ? Direction::Down : Direction::Up);
        }
    }
}


bool LiftController::shouldContinue(int whichLift)
{
    const Direction direction = lift[whichLift].GetDirection();
    const int floor = lift[whichLift].GetFloor();
    if (direction == Direction::Up) {
        for (int i = floor + 1; i <= lift[whichLift].GetMaxFloor(); ++i) {
            if (queue[i - 1][whichLift]) {
                return true;
            }
        }
    } else if (direction == Direction::Down) {
        for (int i = floor - 1; i >= 1; --i) {
            if (queue[i - 1][whichLift]) {
                return true;
            }
        }
    }
    return false;
}

