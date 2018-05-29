
#include <ChassisBiped.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

ChassisBiped::ChassisBiped(uint8_t servoNumRightLeg, uint8_t servoNumRightHip, uint8_t servoNumLeftLeg,  uint8_t servoNumLeftHip, uint8_t servoNeck) {
  Serial.print("Init Chassis... ");
  pwm = Adafruit_PWMServoDriver(0x40);
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  legs[0] = servoNumRightLeg;
  legs[1] = servoNumLeftLeg;
  hips[0] = servoNumRightHip;
  hips[1] = servoNumLeftHip;
  this->servoNeck = servoNeck;
  currentSteer = STEER_STRAIGHT;
  reset();
  Serial.println("[OK]");
}

int ChassisBiped::getMiddlePos() {
  return (int)SERVOMIN + (SERVOMAX - SERVOMIN)*1.0/2 + TRIM_SERVOMID;
}

void ChassisBiped::reset() {
  int middlePos = getMiddlePos();
  for (int s = 0; s < 2; s++) {
    curFootPos[s] = middlePos;
    curHipPos[s] = middlePos;
    pwm.setPWM(legs[s], 0, middlePos);
    pwm.setPWM(hips[s], 0, middlePos);
  }
  pwm.setPWM(servoNeck, 0, middlePos);
}

void ChassisBiped::forward() {
  currentState.moving = true;
}

void ChassisBiped::forward(int speed) {
  currentState.speed = speed;
  forward();
}
void ChassisBiped::backward() {

  //currentState.moving = true;
}
void ChassisBiped::backward(int speed) {
  currentState.speed = speed;
  backward();
}

void ChassisBiped::steer(SteerDirection direction) {
  if (currentSteer != direction) {
    currentSteer = direction;
  }
}

void ChassisBiped::stop(bool doReset) {
  //ToDo: set all legs to postion of step 0
  if (currentState.moving) {
    if (doReset) {
      reset();
    }
  }
  currentState.moving = false;
}

void ChassisBiped::doOperation(ChassisOperations op) {
  switch (op) {
    case STEP_FORE:
      stepForeward();
      break;
    case STEP_RIGHT:
      stepRight();
      break;
    case STEP_LEFT:
      stepLeft();
      break;
  }
}

// Standard Waliking Pattern
void ChassisBiped::stepForeward() {
  int middlePos = getMiddlePos();
  //Feet to the left
  moveParallel(legs[0], legs[1], middlePos - bounceF, middlePos + bounceF, false, false);
  //Hip right side to front
  moveParallel(hips[0], hips[1], middlePos + bounceH, middlePos - bounceH, false, true);
  //Feet to the right
  moveParallel(legs[0], legs[1], middlePos + bounceF, middlePos - bounceF, false, false);
  //Hip left side to front
  moveParallel(hips[0], hips[1], middlePos - bounceH, middlePos + bounceH, false, true);
}

void ChassisBiped::stepRight() {
  reset();
  int middlePos = getMiddlePos();
  //Tilt legt foot
  int d = 2;
  moveServo(legs[1],middlePos, middlePos + 2*bounceF, d);
  moveServo(legs[0],middlePos, middlePos + bounceF, d);

  moveServo(hips[0],middlePos, middlePos - bounceH, d);
  moveServo(legs[1],middlePos + 2*bounceF, middlePos - bounceF, d);
  moveServo(legs[0],middlePos + bounceF, middlePos - 2*bounceF, d);

  moveServo(hips[0],middlePos - bounceH, middlePos, d);
  moveServo(legs[1],middlePos - bounceF, middlePos, d);
  moveServo(legs[0],middlePos - 2*bounceF, middlePos, d);
}

void ChassisBiped::stepLeft() {
  reset();
  int middlePos = getMiddlePos();
  //Tilt legt foot
  int d = 2;
  moveServo(legs[0],middlePos, middlePos - 2*bounceF, d);
  moveServo(legs[1],middlePos, middlePos - bounceF, d);

  moveServo(hips[1],middlePos, middlePos + bounceH, d);
  moveServo(legs[0],middlePos - 2*bounceF, middlePos + bounceF, d);
  moveServo(legs[1],middlePos - bounceF, middlePos + 2*bounceF, d);

  moveServo(hips[1],middlePos + bounceH, middlePos, d);
  moveServo(legs[0],middlePos + bounceF, middlePos, d);
  moveServo(legs[1],middlePos + 2*bounceF, middlePos, d);
}

//ToDo: Generic Walking Pattern

void ChassisBiped::loop(ChassisState *state) {
  // Drive each servo one at a time
  state->moving = currentState.moving;

  if (currentState.moving) {

    switch (currentSteer) {
      case STEER_STRAIGHT:
        stepForeward();
        break;
      case STEER_RIGHT:
        stepRight();
        break;
      case STEER_LEFT:
        stepLeft();
        break;
    }
    //Chekc if next frame is reache

  }
}

void ChassisBiped::moveServo(int servo, int from, int to) {
  moveServo(servo, from, to, speed);
}

void ChassisBiped::moveServo(int servo, int from, int to, int s) {
  if (to > from) {
    for (uint16_t pulselen = from; pulselen < to; pulselen++) {
      pwm.setPWM(servo, 0, pulselen);
      delay(s);
    }
  } else {
     for (uint16_t pulselen = from; pulselen > to; pulselen--) {
      pwm.setPWM(servo, 0, pulselen);
      delay(s);
    }
  }
}

void ChassisBiped::moveParallelStep(int servo1, int servo2, int pulselen, int plcontra, bool contra) {
  pwm.setPWM(servo1, 0, pulselen);
  if (contra) {
    pwm.setPWM(servo2, 0, plcontra);
  } else {
    pwm.setPWM(servo2, 0, pulselen);
  }
}

void ChassisBiped::moveParallel(int servo1, int servo2, int from, int to, bool contra, bool head, int s) {
  int rdc = 1;
  int middlePos = getMiddlePos();
  if (to > from) {
    for (uint16_t pulselen = from; pulselen < to; pulselen++) {
      int plcontra = 2*from-pulselen;
      if (head) {
        int pos = middlePos + 1.0*(middlePos-pulselen)/rdc;
        pwm.setPWM(servoNeck, 0, pos);
        Serial.println(pos);
      }
      moveParallelStep(servo1, servo2, pulselen, plcontra, contra);
      delay(s);
    }
  } else {
    for (uint16_t pulselen = from; pulselen > to; pulselen--) {
      int plcontra = 2*from-pulselen;
      if (head) {
        pwm.setPWM(servoNeck, 0, middlePos + 1.0*(middlePos-pulselen)/rdc);
      }
      moveParallelStep(servo1, servo2, pulselen, plcontra, contra);
      delay(s);
    }
  }
}

void ChassisBiped::moveParallel(int servo1, int servo2, int from, int to, bool contra, bool head) {
  moveParallel( servo1, servo2, from,  to, contra, head, speed);
}
