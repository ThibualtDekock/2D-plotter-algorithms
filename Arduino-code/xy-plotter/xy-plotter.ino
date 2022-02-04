#include "Stepper.h"
#include <Servo.h>

#define steps_per_mm 100.00000
#define S1 18 // Motor endstops
#define S2 19

// Number of segmensts
float SEGMENTS_PER_CIRCLE = 2.0;



// Pin assignments servo
const int servoPin = 27;

// Declare servo
Servo ZAXIS;

// max pos servo
int MAXZ = 135;

// Measuring method
bool IsAbsolutePositioning = true;

// Initializing overall delay
int OverallDelay = 0;

// Initializing speed
float StepperSpeed = 7500;

Stepper stepper_X1;
Stepper stepper_X2;
Stepper stepper_Y;

float px = 0;
float py = 0;
float savedPx = 0;
float savedPy = 0;
bool pz = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(74880);
  delay(25);
  Serial.setTimeout(20);
  Serial.println("Initializing");

  ZAXIS.attach(servoPin);
  ZAXIS.write(MAXZ);

  stepper_X1.attachStepperMotor(15, 21, 14); // Set STEP, DIR, ENABLE pins
  stepper_X1.attachEndstop(S1); // Attach an endstop
  stepper_X2.attachStepperMotor(3, 2, 26); // Set STEP, DIR, ENABLE pins
  stepper_X2.attachEndstop(S1);
  stepper_Y.attachStepperMotor(22, 23, 14); // Set STEP, DIR, ENABLE pins
  stepper_Y.attachEndstop(S2);

  setAllMotorSpeeds(StepperSpeed); // Set speeds



  Serial.println("Initialized");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String zin = Serial.readString();
    Serial.print("Input: ");
    Serial.println(zin);
    // Get command
    String command = zin.substring(0, zin.indexOf(" "));

    // Get code part of input
    zin = zin.substring(zin.indexOf(" ") + 1, zin.length());

    executeAction(command, zin);
  }
}
/*
   Supported commands:
   G1 X. Y.         Linear movement
   G1 F.            Setting the speed (max 10.000)
   G1 Z.            Activate/ Deactivate the pen
   G2 X. Y. I. J.   Clockwise arc
   G3 X. Y. I. J.   Counter clockwise arc
   G4 P.            Set delay
   G21              Auto home
   G90              Set to absolute positioning
   G91              Set to relative positioning
*/
void executeAction(String command, String input) {
  //Serial.println(command);
  switch (command.charAt(0)) {
    case 'G':
      switch (command.charAt(1)) {
        case '1':
          if (input.indexOf('Z') != -1) {
            //Serial.print("Raized ZAXIS to pos: ");
            pz = input.substring(1, 2).toInt();
            if (pz) {
              ZAXIS.write(0);
              delay(50);
            } else {
              ZAXIS.write(MAXZ);
              delay(250);
            }
            //Serial.println(pz);
          } else if (input.indexOf('F') != -1) {
            //Serial.print("Changed speed to : ");
            StepperSpeed = input.substring(input.indexOf('F') + 1, input.length()).toInt();
            setAllMotorSpeeds(StepperSpeed);
            //Serial.println(StepperSpeed);
          } else {
            //Serial.println("Starting linear movement now.");
            // Get X part input
            float X_Val = input.substring(input.indexOf("X") + 1, input.indexOf(" ")).toFloat();
            //Serial.println(X_Val, 4);

            // Remove X part
            input = input.substring(input.indexOf(" ") + 1, input.length());

            // Get Y part input
            float Y_Val = input.substring(input.indexOf("Y") + 1, input.indexOf(" ")).toFloat();
            //Serial.println(Y_Val, 4);
            Serial.print("Line => X: ");
            Serial.print(X_Val, 5);
            Serial.print(" Y: ");
            Serial.println(Y_Val, 5);

            line(X_Val * steps_per_mm, Y_Val * steps_per_mm);
            savedPx = X_Val;
            savedPy = Y_Val;
            Serial.print("New pos => X:");
            Serial.print(savedPx, 5);
            Serial.print(" Y:");
            Serial.println(savedPy, 5);
          }
          break;
        case '2':
          if (command.charAt(2) == '1') {
            //Serial.println("Start homing sequence");
            home();
          } else {
            //Serial.println("Start arc sequence");

            float X_Val = input.substring(input.indexOf("X") + 1, input.indexOf(" ")).toFloat();
            //Serial.println(X_Val, 4);

            // Remove X part
            input = input.substring(input.indexOf(" ") + 1, input.length());

            // Get Y part input
            float Y_Val = input.substring(input.indexOf("Y") + 1, input.indexOf(" ")).toFloat();
            //Serial.println(Y_Val, 4);

            // Remove Y part
            input = input.substring(input.indexOf(" ") + 1, input.length());

            // Get I part input
            float I_Val = input.substring(input.indexOf("I") + 1, input.indexOf(" ")).toFloat();
            //Serial.println(I_Val, 4);

            // Remove I part
            input = input.substring(input.indexOf(" ") + 1, input.length());

            // Get J part input
            float J_Val = input.substring(input.indexOf("J") + 1, input.length()).toFloat();
            //Serial.println(J_Val, 4);
            Serial.print("Arc 2 => X: ");
            Serial.print(X_Val, 5);
            Serial.print(" Y: ");
            Serial.print(Y_Val, 5);
            Serial.print(" I: ");
            Serial.print(I_Val, 5);
            Serial.print(" J: ");
            Serial.println(J_Val, 5);
            superArc(I_Val, J_Val, X_Val, Y_Val, false);
            savedPx = X_Val;
            savedPy = Y_Val;
            Serial.print("New pos => X:");
            Serial.print(savedPx, 5);
            Serial.print(" Y:");
            Serial.println(savedPy, 5);
          }
          break;
        case '3': // Clockwise arc
          float X_Val = input.substring(input.indexOf("X") + 1, input.indexOf(" ")).toFloat();
          //Serial.println(X_Val, 4);

          // Remove X part
          input = input.substring(input.indexOf(" ") + 1, input.length());

          // Get Y part input
          float Y_Val = input.substring(input.indexOf("Y") + 1, input.indexOf(" ")).toFloat();
          //Serial.println(Y_Val, 4);

          // Remove Y part
          input = input.substring(input.indexOf(" ") + 1, input.length());

          // Get I part input
          float I_Val = input.substring(input.indexOf("I") + 1, input.indexOf(" ")).toFloat();
          //Serial.println(I_Val, 4);

          // Remove I part
          input = input.substring(input.indexOf(" ") + 1, input.length());

          // Get J part input
          float J_Val = input.substring(input.indexOf("J") + 1, input.length()).toFloat();
          //Serial.println(J_Val, 4);

          Serial.print("Arc 3 => X: ");
          Serial.print(X_Val, 5);
          Serial.print(" Y: ");
          Serial.print(Y_Val, 5);
          Serial.print(" I: ");
          Serial.print(I_Val, 5);
          Serial.print(" J: ");
          Serial.println(J_Val, 5);
          superArc(I_Val, J_Val, X_Val, Y_Val, true);
          savedPx = X_Val;
          savedPy = Y_Val;
          Serial.print("New pos => X:");
          Serial.print(savedPx, 5);
          Serial.print(" Y:");
          Serial.println(savedPy, 5);
          break;
        case '4':
          if (input.indexOf('P') != -1) {
            //Serial.println(input.substring(input.indexOf('P') + 1, input.length()));
            OverallDelay = input.substring(input.indexOf('P') + 1, input.length()).toInt();
            //Serial.print("Overall delay time adjusted to: ");
            //Serial.print(OverallDelay);
          }
          break;

        case '9':
          if (command.charAt(2) == '0') {
            IsAbsolutePositioning = true;
            //Serial.println("Absolute positioning");
          }
          if (command.charAt(2) == '1') {
            IsAbsolutePositioning = false;
            //Serial.println("Incremental positioning");
          }
          break;
      }


      break;
  }
  Serial.println("true");
}

/*
   This funciton will set all motors to the required speed
*/
void setAllMotorSpeeds(int speed) {
  stepper_X1.setMotorSpeed(speed);
  stepper_X2.setMotorSpeed(speed);
  stepper_Y.setMotorSpeed(speed);
}


/*
   This funciton will raise the Z axis
   After that, it will run the Y axis motor until his endstop is pressed.
   Then it will do the same for the two X axis motors
*/
void home() {
  //Serial.println("Raising Z");
  ZAXIS.write(MAXZ); // off
  delay(500);
  setAllMotorSpeeds(9000); // Set speeds on max for quick homing

  stepper_Y.home(1);
  delay(10);
  stepper_Y.move(-270 * steps_per_mm); // Steps times 10 because Steps_per_mm is / 10 (due to line method)
  delay(10);

  stepper_X1.home(0, stepper_X2);
  delay(10);

  px = 0;
  py = 0;
  pz = 0;
  savedPx = 0;
  savedPy = 0;
  setAllMotorSpeeds(StepperSpeed); // Set speeds
}


/*
   This function will take the printhead to a new position
   It calculates which stepper to move to become a straight line
   This works at all angles
   The steppers will stop at the same time
*/
/*
   Voor 0,1 => input van 100, 100
   Na 0,1 => input van 10, 10
   198,12345
   19,812345


   130
   13000

*/
void line(float newx, float newy) {
  float i;
  float over = 0;
  float dx  = newx - px;
  float dy  = newy - py;

  bool dirx = dx > 0 ? 1 : 0;
  bool diry = dy > 0 ? 1 : 0; // because the motors are mounted in opposite directions
  dx = abs(dx);
  dy = abs(dy);

  if (dx > dy) {
    over = dx / 2;
    for (i = 0; i < (long) dx; i++) {
      stepper_X1.MoveDoubleX(dirx, stepper_X2);
      over += dy;
      if (over >= dx) {
        over -= dx;
        stepper_Y.oneStep(diry);
      }
      //delay(step_delay);
    }
  } else {
    over = dy / 2;
    for (i = 0; i < (long) dy; i++) {
      stepper_Y.oneStep(diry);
      over += dx;
      if (over >= dy) {
        over -= dy;
        stepper_X1.MoveDoubleX(dirx, stepper_X2);;
      }
      //pause(step_delay);
    }
  }

  px = newx;
  py = newy;
}

/*
   This method assumes that the arc is less than 180 degrees
   The method is very complecated.
   It will make an arc, hopefully
*/
void superArc(float cx, float cy, float x, float y, bool dir) {

  Serial.print("Used X: ");
  Serial.println(savedPx, 5);
  Serial.print("Used Y: ");
  Serial.println(savedPy, 5);

  // Calculate middlepoint
  float dx = savedPx + cx;
  float dy = savedPy + cy;

  // Calculate the radius of the circle
  float ddx = abs(cx);
  float ddy = abs(cy);
  float radius = sqrt(ddx * ddx + ddy * ddy);


  // Calculate distance between start and endpoint
  float tx = abs(savedPx - abs(x));
  float ty = abs(savedPy - abs(y));
  float birdTravel = sqrt(tx * tx + ty * ty);

  /*
   * Calculating the number of segments
   * This allows the position to be more persistent  
   * When you set this to a default value, the smaller arcs will loose accuracy
   */
  if(birdTravel < 3) SEGMENTS_PER_CIRCLE = 1.0;
  else if (birdTravel < 6) SEGMENTS_PER_CIRCLE = 2.0;
  else if (birdTravel < 9) SEGMENTS_PER_CIRCLE = 3.0;
  else if (birdTravel < 12) SEGMENTS_PER_CIRCLE = 4.0;
  else if (birdTravel < 15) SEGMENTS_PER_CIRCLE = 5.0;
  else if (birdTravel < 18) SEGMENTS_PER_CIRCLE = 6.0;
  else if (birdTravel < 21) SEGMENTS_PER_CIRCLE = 7.0;
  else if (birdTravel < 24) SEGMENTS_PER_CIRCLE = 8.0;
  else if (birdTravel < 27) SEGMENTS_PER_CIRCLE = 9.0;
  else if (birdTravel < 30) SEGMENTS_PER_CIRCLE = 10.0;
  else SEGMENTS_PER_CIRCLE = 10.0;
  
  // Calculate angle between the start and end point 
  float angle = 2.0 * (asin((birdTravel / 2.0) / radius) * 57296.0 / 1000.0);

  
  float angleSegments = angle / SEGMENTS_PER_CIRCLE;

  // Calculate angle between start and absolute 0
  float mx = abs(savedPx - (dx + radius));
  float my = abs(savedPy - dy);
  float birdTravel2 = sqrt(mx * mx + my * my);

  // Calculating in which kwadrant the circle is
  float angleKwadrant;
  if (dir) {
    if ((abs(dy - savedPy) < 0.005) && (dx > savedPx)) {
      angleKwadrant = 180;
    } else if ((abs(dy - savedPy) < 0.005) && (dx < savedPx)) {
      angleKwadrant = 0;
    } else if (dy < savedPy) {
      angleKwadrant = (2.0 * (asin((birdTravel2 / 2.0) / radius) * 57296.0 / 1000.0)) ;
    } else {
      angleKwadrant = 360 - (2.0 * (asin((birdTravel2 / 2.0) / radius) * 57296.0 / 1000.0)) ;
    }
  } else {
    if ((abs(dy - savedPy) < 0.005) && (dx > savedPx)) {
      angleKwadrant = 180;
    } else if ((abs(dy - savedPy) < 0.005) && (dx < savedPx)) {
      angleKwadrant = 0;
    } else if (dy > savedPy) {
      angleKwadrant = 360 - (2.0 * (asin((birdTravel2 / 2.0) / radius) * 57296.0 / 1000.0));
    } else {
      angleKwadrant = (2.0 * (asin((birdTravel2 / 2.0) / radius) * 57296.0 / 1000.0));
    }
  }

  // Calculating & executing the moves
  float nx, ny, angleCircle;
  for (float i = 1; i < SEGMENTS_PER_CIRCLE; i++) {
    if (dir) {
      angleCircle = angleSegments * i + angleKwadrant;
    } else {
      angleCircle = angleKwadrant - angleSegments * i;
    }
    nx = dx + radius * cos(angleCircle * PI / 180.0);
    ny = dy + radius * sin(angleCircle * PI / 180.0);
    line(nx * steps_per_mm, ny * steps_per_mm);
  }

  // Backup line to final destination
  line(x * steps_per_mm, y * steps_per_mm);
}
