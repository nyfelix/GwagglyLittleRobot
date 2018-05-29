#ifndef chassis_h
#define chassis_h

#include <Arduino.h>
#include <Modular.h>
#include <Adafruit_PWMServoDriver.h>
#include <ArduinoJson.h>

#define SERVOMIN  150  // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600  // this is the 'maximum' pulse length count (out of 4096)
#define TRIM_SERVOMID -10 // 90° is not exactl mid(Min, Max)... here's the trim
struct ChassisState {
  int speed = 1;
  bool moving = false;
};

enum SteerDirection {
  STEER_LEFT,
  STEER_RIGHT,
  STEER_STRAIGHT
};

enum ChassisOperations {
  STEP_FORE,
  STEP_RIGHT,
  STEP_LEFT,
  HEAD_STRAIGHT,
  HEAD_RIGHT,
  HEAD_LEFT,
  CHASSIS_OP_NONE
};

class ChassisBiped : public Component
{
  public:
    ChassisBiped(uint8_t servoNumRightLeg, uint8_t servoNumRightHip, uint8_t servoNumLeftLeg,  uint8_t servoNumLeftHip, uint8_t servoNeck);

    ChassisState currentState;
    void loop(ChassisState *state);
    void reset();
    void forward();
    void forward(int speed);
    void backward();
    void backward(int speed);
    void steer(SteerDirection direction);
    void stop(bool doReset = true);
    void doOperation(ChassisOperations op);

  protected:
    void moveServo(int servo, int from, int to);
    void moveServo(int servo, int from, int to, int s);
    void moveParallelStep(int servo1, int servo2, int pulselen, int plcontra, bool contra);
    void moveParallel(int servo1, int servo2, int from, int to, bool contra, bool head);
    void moveParallel(int servo1, int servo2, int from, int to, bool contra, bool head, int s);
    void stepForeward();
    void stepRight();
    void stepLeft();
    Adafruit_PWMServoDriver pwm;
    SteerDirection currentSteer;
    int getMiddlePos();
    uint8_t legs[2] = {};
    uint8_t hips[2] = {};
    uint8_t servoNeck;
    int bounceF = 35;
    int bounceH = 90;
    int curFootPos[2];
    int curHipPos[2];
    int speed=1;
};

#endif
