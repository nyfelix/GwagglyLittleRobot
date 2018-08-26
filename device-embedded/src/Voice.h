#ifndef voice_h
#define voice_h

#include <Modular.h>
#include <Debug.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>

struct VoiceState {
  bool playing;
};

class Voice : public Component
{
  public:
    Voice(uint8_t vsResetPin, uint8_t vsCSPin, uint8_t vsDCSPin, uint8_t vsDREQPin, uint8_t scCardCSPin);
    void loop(VoiceState *state);
    void play(const char *fileName);

  protected:
    Adafruit_VS1053_FilePlayer *musicPlayer;
    VoiceState currentState;
    const char *nextTrackName = "";
};

#endif
