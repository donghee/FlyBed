#include "Servo.h"
#include "mbed.h"

Servo::Servo(PinName Pin) : ServoPin(Pin) {
    initialize(); // TODO: Works?
}

void Servo::initialize() {
    // initialize ESC
    Enable(1000,20000); // low throttle 50Hz TODO: Frequency modify
}

void Servo::SetPosition(int Pos) {
    Position = Pos;
}

void Servo::StartPulse() {
    ServoPin = 1;
    PulseStop.attach_us(this, &Servo::EndPulse, Position);
}

void Servo::EndPulse() {
    // my change
    PulseStop.detach();
    // my change
    ServoPin = 0;
}

void Servo::Enable(int StartPos, int Period) {
    Position = StartPos;
    Pulse.attach_us(this, &Servo::StartPulse, Period);
}

void Servo::Disable() {
    Pulse.detach();
}

void Servo::operator=(int position) {
    SetPosition(position);
}