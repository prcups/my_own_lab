#include "lift.h"
#include "ui_lift.h"

Lift::Lift(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::Lift)
{
    m_ui->setupUi(this);
    connect(&controller, &LiftController::floorChanged, this, &Lift::handleFloor);
    connect(&controller, &LiftController::doorChanged, this, &Lift::handleDoor);
    connect(&controller, &LiftController::directionChanged, this, &Lift::handleDirection);
    connect(m_ui->OutsideDo, &QPushButton::released, this, &Lift::handleOutsideWork);
    connect(m_ui->InsideDo, &QPushButton::released, this, &Lift::handleInsideWork);
}

Lift::~Lift() = default;

void Lift::handleFloor(bool whichLift, int floor)
{
    auto liftFloor = whichLift ? m_ui->LiftFloor2 : m_ui->LiftFloor1;
    liftFloor->display(floor);
}

void Lift::handleDoor(bool whichLift, bool door)
{
    auto liftDoor = whichLift ? m_ui->LiftDoor2 : m_ui->LiftDoor1;
    liftDoor->setText(door ? "开门" : "关门");
}

void Lift::handleDirection(bool whichLift, Direction direction)
{
    auto liftDirection = whichLift ? m_ui->LiftDirection2 : m_ui->LiftDirection1;
    liftDirection->setText([&, direction]() -> auto {
        switch(direction) {
            case Direction::Up: {
                return "\u2191";
            }
            case Direction::Down: {
                return "\u2193";
            }
            case Direction::Stop: {
                return " ";
            }
        }
    }());
}

void Lift::handleOutsideWork() {
    controller.outsideTask(m_ui->OutsideFloor->value(), m_ui->OutsideDirection->currentIndex() ? Direction::Down : Direction::Up);
}

void Lift::handleInsideWork() {
    controller.insideTask(m_ui->InsideLift->value() - 1, m_ui->InsideFloor->value());
}
