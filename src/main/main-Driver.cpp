#include <Arduino.h>
#include "RC.h"
#include "Subsystems/Clifford.h"
#include "Subsystems/Shuttle.h"
#include "Subsystems/Driver.h"
#include "Util.h"

#define RPMSCALE 5700
#define ks 1
#define kv 1
#define t_desired 100 // Nm
#define ka 1
#define THETA 1
#define I_s 1
#define max_voltage 100

template <typename T> int sgn(T num) {
    return (T(0) < num) - (num < T(0));
}

#define DesiredVoltage ks*sgn(THETA) + kv + ka*(t_desired/I_s)
#define Battery 100 // TODO

// TODO: If limit switches are hit on elevator that we can’t move it anymore
// TODO: Autonomous torque and downforce application and Stake handoff
// - https://www.overleaf.com/read/kghkvywmkyfj
// - Use Latex file's math to calculate

lattice::RC controller(Serial1, 19, 2, 13); // TODO: Use correct wiring when ATVR is fixed
lattice::Clifford &clifford = lattice::Clifford::clifford();
lattice::Driver &driver = lattice::Driver::driver();
lattice::HytorcSimple hytorcSimple{9, 5, 6}; // TODO: Temp Ports

// Runs at init
void setup() {
    lattice::GenericSetup();
    controller.Setup();
    driver.Setup();
}

// Vertical: Move Drive Train Up/Down
// Horizontal: Move Drive Train Left/Right
bool updateClifford(double x, double y) {
    // clifford.Move(x, y); // TODO: Implement clifford.Move()
    return true;
}

// Vertical: Move Elevator
// Horizontal: Switch Stake
bool updateElevator(double y, double drill) {
    driver.SetElevatorPower(y);
    hytorcSimple.SetPercentOutput(drill);
    return true;
}

bool autoDrill() {
    hytorcSimple.SetVoltage(DesiredVoltage, Battery);
    return true;
}

// Horizontal: Drive Left/Right
// bool updateShuttle(double x) {
//     shuttle.SetMotionMotors(x * RPMSCALE);
//     return true;
// }

// Runs continously
void loop() {
    controller.Update();

    // Translational Motion, Right side
    double x = -1 * controller.GetAileron();
    double y = controller.GetElevator();
    double drill = (controller.GetRudder() - 0.5) * -2;
    int stake = controller.GetAux1();

    // Set stake number with Switch A, assume there's 3 for now
    switch (stake) {
        case 0:
            driver.SetStake(lattice::Driver::StakeNumber::kOne);
            break;
        case 1:
            driver.SetStake(lattice::Driver::StakeNumber::kTwo);
            break;
        case 2:
            driver.SetStake(lattice::Driver::StakeNumber::kThree);
            break;
    }

    // Serial.println(drill);
    // Serial.print(stake);
    // Serial.print(" ");

    // F Switch Finite State Machine
    bool success;
    switch (controller.GetGear()) {
        case 0:
            success = updateClifford(x, y);
            break;
        case 1:
            success = updateElevator(y, drill);
            break;
        case 2:
        //     success = updateShuttle(x);
            break;
        default:
            success = false;
            Serial.println("Unexpected return val for controller.GetGear()");
    };

    if (!success) {
        Serial.println("Failure in Finite State Machine for RC");
    }

    // Killswitch
    if (controller.GetAux2() == 1) {
        driver.EStop();
        Serial.println("Aux 2: Killed shuttle");
    };
}