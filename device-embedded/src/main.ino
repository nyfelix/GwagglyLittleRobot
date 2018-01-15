/*
  This is the sourcecode of IgelBot Project.
  But also, it is an approach to design a robotic system inspired be the React Flux Pattern
  For now, this pattern ins not properly embedded into a framework, but rather developed
  following a set of concentions / rules:
  - A system consists of number of components
  - Components are reusabel mechatronic units
  - A component has
    - a state: struct {ComponentName}State
    - a number of Actions, that initiaite a behavoir in physical or logical operation (e.g. a movement, read a sensor signal)
    - a loop method, that executes the next step. A Component loop must never use a timefunciton such as delay()
      The Loop is ment have a very low porcessing time in order to have short reaction time from signals of other component.
  - Only a Component is allowed to update its state (through actions or loop).
  - The main loop works like flux:
    1. Read Input form the Supersystem
    2. Set Actions, based on the current State (you could call this step the "Strategy" step)
    3. Loop each component
*/

#include <Arduino.h>
#include <Modular.h>
#include <Sonar.h>
#include <ChassisBiped.h>

// Configuration
#include <Config.h>

// System States

// The Robot has 3 States:
// The first three are modes of operation (default is AUTONOMOUS)
// REMOTE_CONTROL and TEACH (Blockly JR) are controlled by the app.
// From any mode, if an obstacle is detected, the system will go to DODGE
// mode. RESUME_LAST is a dynamic state that resumes the last state.
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
  bool stop = true;
  SystemJobState job;
} state;

// Components used by the system
Sonar *sonar;
ChassisBiped *chassis;
// Voice *voice; // To be developed

// Variables to operate the full system
SystemJobState lastJobState;
int stepCount = 0;
int teachLastIndex = 0;
int teachIndex =0;
ChassisOperations teachOperations[100];
bool teachLoop = false;
// Plugins
#include <Api.h>

void setup() {
  Serial.begin(9600);
  // Wait for serail on feather
  /*while ( ! Serial ) {
      delay( 1 );
  }*/
  Serial.println("System: Booting...");
  sonar = new Sonar(SONAR_TRIGGER_PIN, SONAR_ECHO_PIN, SONAR_MAX_DISTANCE);
  chassis = new ChassisBiped(SERVO_FOOT_RIGHT, SERVO_HIP_RIGHT, SERVO_FOOT_LEFT, SERVO_HIP_LEFT, SERVO_NECK);

  // Initialze web Api
  setupApi(WIFI_CS, WIFI_IRQ, WIFI_RST, WIFI_EN, WIFI_LISTEN_PORT);
  Serial.println("System: System started");

  setJobState(AUTONOMOUS);
  start();
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
}

/* main Action Start */
void start() {
    state.stop = false;
}

/* main Action Start */
void stop() {
    state.stop = true;
}

void setJobState(SystemJobState job) {
  //ToDo: Only allow valid state transitions
  if (job == RESUME_LAST) {
    state.job = lastJobState;
  } else {
    lastJobState = state.job;
    state.job = job;
  }
  Serial.print("New Job: ");
  Serial.println (state.job);
}

/* Run Component actions based on the current state */
/* Strategy consts */

void strategy() {
  // To be discussed: When should Sonar be activated (in which sates)
  if (state.job != DODGE && (state.sonar.obstacelDistance > 0 && state.sonar.obstacelDistance < MIN_OBJECT_DISTANCE_CM)) {
    setJobState(DODGE);
  }
  if (state.job == DODGE) { exeJobDodge(); }
  if (state.job == REMOTE_CONTROL) { exeJobRC(); }
  if (state.job == AUTONOMOUS)     { exeJobAuto();  }
  if (state.job == TEACH)     { exeJobTeach();  }
}

// Turn right until distance is ok
void exeJobDodge() {
  if (state.sonar.obstacelDistance == 0 || state.sonar.obstacelDistance >= MIN_OBJECT_DISTANCE_CM) {
    setJobState(RESUME_LAST);
  }
  // Allways do two steps
  chassis->doOperation(STEP_RIGHT);
  chassis->doOperation(STEP_RIGHT);
}


// Methods of RC operation
void exeJobRC() {
  // Movement Commands are set by the API, the strategy is only to inverfere only when the robo is in danger
  // To implement this properly, the API must not directely set chassis actions, rather it should update the API state and
  // the final decision must be done here.
}

// Methods for teaching robot with Blockly JR

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

//Move until an obstacel is detected, then turn right until the way is free
void exeJobAuto() {
  chassis->stop(false);
  //Serial.println(state.sonar.obstacelDistance);
  stepCount++;
  switch (stepCount) {
    case 0 ... 4:
      chassis->doOperation(STEP_FORE);
      break;
    case 5 ... 9:
      chassis->doOperation(STEP_RIGHT);
      break;
    case 10 ... 14:
      chassis->doOperation(STEP_FORE);
      break;
    case 15 ... 19:
      chassis->doOperation(STEP_LEFT);
      break;
    case 20:
      stepCount = 0;
      break;
  }

  Serial.println(stepCount);
}
