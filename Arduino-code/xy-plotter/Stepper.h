#ifndef STEPPER_H
#define STEPPER_H
#include <Arduino.h>
class Stepper {
  public:
    void attachStepperMotor(byte stepPin, byte dirPin, byte enablePin);
    void attachEndstop(byte pin);
    void setMotorSpeed(int stepperspeed);
    void setStepsPerMm(int stepsPerMm);
    void setMotorDirection(bool direction);
    void move(int steps);
    void MoveDoubleX(int steps1, Stepper stepper2);
    void oneStep(bool dir);
    void home(bool dir);
    void home(bool dir,Stepper stepper);
  private:
    byte _stepPin;
    byte _dirPin;
    byte _enablePin;
    int _speed;
    int _stepsPerMm;
    byte _endStop;
};


#endif
