#include <Arduino.h>

#include "Brake.h"
#include "HytorcSimple.h"
#include "Util.h"

lattice::Brake brakeport(A4);
// lattice::HytorcSimple testMotor(9, 5, 6);

double power = 0;
bool brakeflag = false;
// temperary until test
// Brake = false (off)
// brake = true (on)

void setup() {
    // testMotor.Setup();
    // testMotor.SetPercentOutput(power);
    lattice::GenericSetup();
    brakeport.Setup();
}

void loop() {
    // if(Serial.available()){
    //     char input = (uint8_t)Serial.read();

    //     if (input == 'a') {
    //         power = 0.25;
    //     } else if (input == 'b') {
    //         power = -0.25;
    //     } else if (input == 'c') {
    //         power = 1;
    //     } else if (input == 'd') {
    //         power = -1;
    //     } else if (input == 'f') {
    //         power = 0;
    //     }
    //     if(input == 'j'){
    //         brakeflag = true;
    //         power = 0;
    //     }
    // }
    if (Serial.available()) {
        char input = (uint8_t)Serial.read();
        if (input == 's') {
            // testMotor.SetPercentOutput(power);
            brakeport.Set(true);
            Serial.println("start");
        } else if (input == 'a') {
            brakeport.Set(false);
            Serial.println("end");
        }
    }
    // Serial.println(testMotor.GetPosition());
    // testMotor.SetPercentOutput(power);
    // brakeport.Set(brakeflag);
}