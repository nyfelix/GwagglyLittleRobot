#ifndef sonar_h
#define sonar_h

#include <Arduino.h>
#include <Modular.h>
#include <NewPing.h>

#define INFINITE_DISTANCE 9999;

struct SonarState {
  int obstacelDistance = INFINITE_DISTANCE;
};

class Sonar : public Component
{
  public:
    Sonar(uint8_t triggerPin, uint8_t echoPin, unsigned int maxDistance);
    void loop(SonarState *state);

  protected:
    NewPing *sonar;
};

#endif
