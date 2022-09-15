#pragma once
#include <AccelStepper.h>

namespace lattice {
    class StepperHandOff {
        public:
        /**
         * Sets up to control the handoff motor off the given pins
         * @param pin1, pin2, pin3, pin4 The four pins connected to the motor
         */
        StepperHandOff(const int NumPin, const int pin1, const int pin2,
            const int pin3, const int pin4);

        void Setup(int MaxSpeed);
        /**
         * Sets the speed of the handoff motor
         * @param speed Speed in rpm
         * @return True if successful, false if error
         */
        bool SetSpeed(long speed);

        /**
         * Instructs the handoff motor to advance the given
         * number of steps
         * @param steps The steps to move: positive for forwards, negative
         *      for reverse
         * @return True on successful movement, false otherwise
         */
        bool Move(int steps);

        void run();

        /**
         * Sets the brake to the given value, overriding any movement
         * @param brake True to stop, false to go
         * @return True on success, false otherwise
         */
        bool SetBrake(bool brake);

        private:
        static const int STEPS_PER_REV = 200;
        AccelStepper mMotor;
        bool brake;
    };
}