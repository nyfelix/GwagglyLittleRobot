# 1 "/var/folders/ch/1lck182n1_q1thdk0zn0p6_m0000gn/T/tmpOTf2ml"
#include <Arduino.h>
# 1 "/Users/nyfelix/dev/GwagglyLittleRobot/device-embedded/src/main.ino"
# 20 "/Users/nyfelix/dev/GwagglyLittleRobot/device-embedded/src/main.ino"
#include <Arduino.h>
#include <Modular.h>
#include <Sonar.h>
#include <ChassisBiped.h>
#include <Voice.h>


#include <Config.h>
# 36 "/Users/nyfelix/dev/GwagglyLittleRobot/device-embedded/src/main.ino"
enum SystemJobState {
  AUTONOMOUS,
  REMOTE_CONTROL,
  TEACH,
  DODGE,
  RESUME_LAST
};

struct SystemState {
  ChassisState chassis;
  SonarState sonar;
  VoiceState voice;
  bool stop = true;
  SystemJobState job;
} state;


Sonar *sonar;
ChassisBiped *chassis;
Voice *voice;


SystemJobState lastJobState;
int stepCount = 0;
int teachLastIndex = 0;
int teachIndex =0;
ChassisOperations teachOperations[100];
bool teachLoop = false;

#include <Api.h>
void setup();
void loop();
void start();
void stop();
void setJobState(SystemJobState job);
void strategy();
void exeJobDodge();
void exeJobRC();
void teachEmptyStack();
void teachAddOpperation(ChassisOperations op);
void teachRestart();
void exeJobTeach();
void exeJobAuto();
#line 67 "/Users/nyfelix/dev/GwagglyLittleRobot/device-embedded/src/main.ino"
void setup() {
  Serial.begin(9600);

  #ifdef DEBUG

  while ( ! Serial ) {
      delay( 1 );
  }
  #endif
  Serial.println("System: Booting...");
  sonar = new Sonar(SONAR_TRIGGER_PIN, SONAR_ECHO_PIN, SONAR_MAX_DISTANCE);
  chassis = new ChassisBiped(SERVO_FOOT_RIGHT, SERVO_HIP_RIGHT, SERVO_FOOT_LEFT, SERVO_HIP_LEFT, SERVO_NECK);
  voice = new Voice(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, SDCARD_CS);

  setupApi(WIFI_CS, WIFI_IRQ, WIFI_RST, WIFI_EN, WIFI_LISTEN_PORT);
  Serial.println("System: System started");

  setJobState(AUTONOMOUS);
  stop();
}

void loop() {
  if(!state.stop) {
    sonar->loop(&state.sonar);
    chassis->loop(&state.chassis);
    strategy();
  } else {
    chassis->stop();
  }
  loopApi();

  voice->loop(&state.voice);
}


void start() {
    state.stop = false;
}


void stop() {
    state.stop = true;
}

void setJobState(SystemJobState job) {

  if (job == RESUME_LAST) {
    state.job = lastJobState;
  } else {
    lastJobState = state.job;
    state.job = job;
  }
  Serial.print("New Job: ");
  Serial.println (state.job);
}




void strategy() {

  if (state.job != DODGE && (state.sonar.obstacelDistance > 0 && state.sonar.obstacelDistance < MIN_OBJECT_DISTANCE_CM)) {
    setJobState(DODGE);
  }
  if (state.job == DODGE) { exeJobDodge(); }
  if (state.job == REMOTE_CONTROL) { exeJobRC(); }
  if (state.job == AUTONOMOUS) { exeJobAuto(); }
  if (state.job == TEACH) { exeJobTeach(); }
}


void exeJobDodge() {
  Serial.println(state.sonar.obstacelDistance);
  if (state.sonar.obstacelDistance == 0 || state.sonar.obstacelDistance >= MIN_OBJECT_DISTANCE_CM) {
    setJobState(RESUME_LAST);
  }

  chassis->doOperation(STEP_RIGHT);
  chassis->doOperation(STEP_RIGHT);
}



void exeJobRC() {



}



void teachEmptyStack() {
  for (int i = 0; i < 100; i++) {
    teachOperations[i] = CHASSIS_OP_NONE;
  }
  teachLastIndex = 0;
  teachIndex = 0;
}

void teachAddOpperation(ChassisOperations op) {
  if (teachLastIndex < 100) {
    teachOperations[teachLastIndex] = op;
    teachLastIndex++;
  }
}

void teachRestart() {
  teachIndex = 0;
}

void exeJobTeach() {
  chassis->stop(false);
  if (teachIndex < teachLastIndex) {
    chassis->doOperation(teachOperations[teachIndex]);
    teachIndex++;
  } else if (teachLoop) {
    teachRestart();
  }
}


void exeJobAuto() {
  chassis->stop(false);

  stepCount++;
  switch (stepCount) {
    case 0 ... 14:
      chassis->doOperation(STEP_FORE);
      break;
    case 15 ... 19:
      chassis->doOperation(STEP_RIGHT);
      break;
    case 20 ... 34:
      chassis->doOperation(STEP_FORE);
      break;
    case 35 ... 39:
      chassis->doOperation(STEP_LEFT);
      break;
    case 40:
      stepCount = 0;
      break;
  }
}