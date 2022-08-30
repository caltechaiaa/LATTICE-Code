#include "Driver.h"

#include <tuple>

#include "FeedforwardUtil.h"
#include "Util.h"

using namespace lattice;

Driver::Driver()
    : elevator(ElevatorConstants::kMotorPin, ElevatorConstants::kEncoderFwdPin, ElevatorConstants::kEncoderBckPin),
      /* actuator(), */
      handoff(HandoffConstants::kMotorPin1, HandoffConstants::kMotorPin2, HandoffConstants::kMotorPin3, HandoffConstants::kMotorPin4),
      /* rc_input(), rc_output(), */
      firstStake(HandoffConstants::kLimitSwitch1Pin),
      secondStake(HandoffConstants::kLimitSwitch2Pin),
      thirdStake(HandoffConstants::kLimitSwitch3Pin),
      elevatorZero(ElevatorConstants::kTopLimitSwitchPin),
      elevatorEnd(ElevatorConstants::kBottomLimitSwitchPin),
      actuatorTemp(DriverConstants::kHytorcThermistorPin),
      actuatorCurrent(DriverConstants::kHytorcCurrentPin),
      elevatorCurrent(ElevatorConstants::kCurrentPin),
      elevatorController(ElevatorConstants::kP, ElevatorConstants::kI, ElevatorConstants::kD,
                         GetElevatorFeedforward(ElevatorConstants::kS, ElevatorConstants::kV, ElevatorConstants::kA, ElevatorConstants::kG, 0, 0)),
      actuatorController(DriverConstants::kP, DriverConstants::kI, DriverConstants::kD,
                         GetSimpleFeedforward(DriverConstants::kS, DriverConstants::kV, DriverConstants::kA, 0, 0)),
      state(State::Idle) {}

void Driver::Setup() {
    elevator.Setup();

    firstStake.Setup();
    secondStake.Setup();
    thirdStake.Setup();
    elevatorZero.Setup();
    elevatorEnd.Setup();
}

void Driver::UpdateSensors() {
    firstStake.Update();
    secondStake.Update();
    thirdStake.Update();
    elevatorZero.Update();
    elevatorEnd.Update();
}

void Driver::Run() {
    UpdateSensors();
    // TODO: state machine stuff
}

void Driver::EStop() {
    elevator.SetBrake(true);
    // actuator.SetBrake(true);
    handoff.SetBrake(true);
}

bool Driver::RunElevatorOneTick(double setpoint) {
    double feedback = elevatorCurrent.Get();
    double input;
    bool success;
    std::tie(input, success) = actuatorController.Run(feedback, setpoint);

    if (success) {
        if (input < kElevatorMinVoltage) {  // voltage drop
            // TODO: throw a warning that we might be stuck
        }
        if (!elevator.Run(input)) {
            // TODO: throw a warning for elevator input out of bounds
        }

        delay(kElevatorLoopDelay);
        if (elevatorEnd.Pushed()) {
            return true;
        }
        // TODO: calculate velocity, if it's zero, throw a warning that we might be stuck
    } else {
        // TODO: throw a warning for elevator controller not evaluating?
    }

    return false;
}

bool Driver::ZeroElevator() {
    if (state == State::ZeroElevator) {
        if (elevatorZero.Get()) {
            elevator.Run(0);  // stop
            state = State::Idle;
            return true;
        }
    } else if (state == State::Idle) {
        if (elevatorZero.Get()) {
            // TODO: throw warning of trying to zero elevator when already zeroed
            return true;
        }
        state = State::ZeroElevator;
        elevator.Run(-kElevatorZeroSpeed);
    } else {
        // TODO: throw warning that we can't zero the elevator right now
        // add way to override it?
    }
    return false;
}