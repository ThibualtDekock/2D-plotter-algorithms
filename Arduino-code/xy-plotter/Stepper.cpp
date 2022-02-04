#include "Stepper.h"


/*
    The stepper I'm using uses 3200 pulses on the STEP pin.
*/
//#define singleRotation 3200

void Stepper::attachStepperMotor(byte stepPin, byte dirPin, byte enablePin) {
  // Assign outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  // Set level output
  digitalWrite(enablePin, LOW); // Active when LOW

  // Store values
  _stepPin = stepPin;
  _dirPin = dirPin;
  _enablePin = enablePin;
  _speed = 100; // Random starting speed
}

void Stepper::move(int steps) {
  byte dir = steps > 0 ? 1 : 0;
  for (int i = 0; i < abs(steps) ; i++) {
    oneStep(dir);
  }
}

/*
   Since the speed is controlled with the microseconds delay,
   this value should be mapped to a value between 500 (slow) and
   25 (very fast)
*/
void Stepper::setMotorSpeed(int stepperspeed) {
  _speed = map(stepperspeed, 0, 10000, 500, 25);
}

void Stepper::setStepsPerMm(int steps) {
  _stepsPerMm = steps;
}

void Stepper::oneStep(bool dir) {
  if (dir) {
    setMotorDirection(true);
  } else {
    setMotorDirection(false);
  }
  digitalWrite(_stepPin, HIGH);
  delayMicroseconds(25);
  digitalWrite(_stepPin, LOW);
  delayMicroseconds(_speed);
}

void Stepper::MoveDoubleX(int steps, Stepper stepper2) {
  int stepsDone = 0;
  byte dir = steps > 0 ? 1 : 0;
  do {
    oneStep(dir);
    stepper2.oneStep(dir);
    stepsDone++;
  } while (stepsDone < steps);
}

void Stepper::setMotorDirection(bool direction) {
  if (direction) {
    digitalWrite(_dirPin, LOW);
  } else {
    digitalWrite(_dirPin, HIGH);
  }
}

void Stepper::attachEndstop(byte pin) {
  _endStop = pin;
  pinMode(pin, INPUT_PULLUP);
}

void Stepper::home(bool dir) {
  while (!digitalRead(_endStop)) {
    oneStep(dir);
  }
}
void Stepper::home(bool dir, Stepper stepper) {
  while (!digitalRead(_endStop)) {
    oneStep(dir);
    stepper.oneStep(dir);
  }
}
