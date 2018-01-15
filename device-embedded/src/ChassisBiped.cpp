
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
  servoNeck = servoNeck;
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
  moveParallel(legs[0], legs[1], middlePos - bounceF, middlePos + bounceF, false);
  //Hip right side to front
  moveParallel(hips[0], hips[1], middlePos + bounceH, middlePos - bounceH, false);
  //Feet to the right
  moveParallel(legs[0], legs[1], middlePos + bounceF, middlePos - bounceF, false);
  //Hip left side to front
  moveParallel(hips[0], hips[1], middlePos - bounceH, middlePos + bounceH, false);
}

void ChassisBiped::stepRight() {
  int middlePos = getMiddlePos();
  //Feet to the left
  moveParallel(legs[0], legs[1], middlePos, middlePos - 2*bounceF, false);
  //Hip right side to front
  moveParallel(hips[0], hips[1], middlePos, middlePos + 20, true);
  //Feet to the right
  moveParallel(legs[0], legs[1], middlePos - 2*bounceF, middlePos, false);
  //Hip left side to front
  moveParallel(hips[0], hips[1], middlePos + 20, middlePos, true);
}

void ChassisBiped::stepLeft() {
  int middlePos = getMiddlePos();
  //Feet to the left
  moveParallel(legs[0], legs[1], middlePos, middlePos - 2*bounceF, false);
  //Hip right side to front
  moveParallel(hips[0], hips[1], middlePos, middlePos - 20, true);
  //Feet to the right
  moveParallel(legs[0], legs[1], middlePos - 2*bounceF, middlePos, false);
  //Hip left side to front
  moveParallel(hips[0], hips[1], middlePos - 20, middlePos, true);
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
  if (to > from) {
    for (uint16_t pulselen = from; pulselen < to; pulselen++) {
      pwm.setPWM(servo, 0, pulselen);
      delay(speed);
    }
  } else {
     for (uint16_t pulselen = from; pulselen > to; pulselen--) {
      pwm.setPWM(servo, 0, pulselen);
      delay(speed);
    }
  }
}

void ChassisBiped::moveParallel(int servo1, int servo2, int from, int to, bool contra) {
  if (to > from) {
    for (uint16_t pulselen = from; pulselen < to; pulselen++) {
      pwm.setPWM(servo1, 0, pulselen);
      int pl = pulselen;
      if (contra) {
        pl = 2*from-pulselen;
      }
      pwm.setPWM(servo2, 0, pl);
      delay(speed);
    }
  } else {
     for (uint16_t pulselen = from; pulselen > to; pulselen--) {
      pwm.setPWM(servo1, 0, pulselen);
      int pl = pulselen;
      if (contra) {
        pl = 2*from-pulselen;
      }
      pwm.setPWM(servo2, 0, pl);
      delay(speed);
    }
  }
}
