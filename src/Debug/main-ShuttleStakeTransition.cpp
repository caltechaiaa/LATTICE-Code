#include "RC.h"
#include "Subsystems/Shuttle.h"
#include "TaskScheduler.h"
#include "Util.h"

// Manual Mode -> Test if limit switches work to cut power and if normal control works
// Left Arm Transition
// Both Arm Transition
// Right Arm Transition

auto& shuttle = lattice::Shuttle::GetInstance();
lattice::RC controller{Serial1, 19, 2};
int testState = 0;
constexpr double takeup = 0.25;
constexpr int kPeriod = 20;
bool enable = false;
bool armTransition = false;
bool initial = false;
bool hitStake = false;
bool offStake = false;
void run() {
    shuttle.UpdateSensors();
    if (Serial.available()) {
        char input = (uint8_t)Serial.read();
        if (input == 'a') {
            shuttle.SetFrontLimitSwitch(lattice::Shuttle::kLeft);
        } else if (input == 'b') {
            shuttle.SetFrontLimitSwitch(lattice::Shuttle::kRight);
        } else if (input == 'c') {
            hitStake = true;
        } else if (input == 'd') {
            offStake = false;
        } else if (input == 's') {
            enable = true;
            Serial.println("----------------");
        } else if (input == 'e') {
            enable = false;
        }
    }

    if (enable) {
        if (!armTransition) {
            offStake = false;
            armTransition = shuttle.ConstantTakeupDrive(hitStake);
            Serial.println("2000000000");
        } else {
            hitStake = false;
            armTransition = !shuttle.StakeTransition(offStake);
            Serial.println("3000000000");
        }
    } else {
        shuttle.StopMotionMotors();
        shuttle.SetTensionArmPowers(0, 0);
        Serial.println("000000000000");
    }

    Serial.print(shuttle.GetLeftSetpoint());
    Serial.print(", ");
    Serial.print(shuttle.GetRightSetpoint());
    Serial.print(", ");
    Serial.print(shuttle.GetLeftTensionArmPos());
    Serial.print(", ");
    Serial.println(shuttle.GetRightTensionArmPos());
}

Task mainLoop(kPeriod, TASK_FOREVER, &run);
Scheduler ts;
void setup() {
    lattice::GenericSetup();
    shuttle.Setup();
    shuttle.SetTakeup(takeup);
    shuttle.SetFrontLimitSwitch(lattice::Shuttle::kLeft);
    ts.addTask(mainLoop);
    delay(500);
    mainLoop.enable();
}

void loop() {
    ts.execute();
}