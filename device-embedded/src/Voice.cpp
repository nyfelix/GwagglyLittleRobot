#include <Voice.h>

Voice::Voice(uint8_t vsResetPin, uint8_t vsCSPin, uint8_t vsDCSPin, uint8_t vsDREQPin, uint8_t scCardCSPin) { //Vision(uint8_t pinSS);
  Serial.print("Init Soundsystem ...");
  musicPlayer = new Adafruit_VS1053_FilePlayer(vsResetPin, vsCSPin, vsDCSPin, vsDREQPin, scCardCSPin);
  if (! musicPlayer->begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
  }
  if (!SD.begin(scCardCSPin)) {
    Serial.println(F("SD failed, or not present"));
  }
  musicPlayer->setVolume(1,1);
  musicPlayer->useInterrupt(VS1053_FILEPLAYER_PIN_INT);

  Serial.println("[OK]");

}

void Voice::play(const char *fileName) {
  nextTrackName = fileName;
}

void Voice::loop(VoiceState *state) {
  if (musicPlayer->stopped()) {
    if (nextTrackName != "") {
      debugLn("Voice: Start Playing");
      musicPlayer->playFullFile(nextTrackName);
      nextTrackName = "";
    }
  }
}
