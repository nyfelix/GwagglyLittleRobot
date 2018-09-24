
////////////////////////////////////////////////////////////////////
// Define the Pins connected to the arduineo
//

#define DEBUG

// Setup for the Feather
// Chassis Pins
#define SERVO_FOOT_RIGHT 0
#define SERVO_FOOT_LEFT 4
#define SERVO_HIP_RIGHT 1
#define SERVO_HIP_LEFT 5
#define SERVO_NECK 6

// Sonar Pins
#define SONAR_TRIGGER_PIN A1 //  yellow
#define SONAR_ECHO_PIN A2 // orange
#define SONAR_MAX_DISTANCE 500

// WiFi Pins
#define WIFI_CS 8
#define WIFI_IRQ 7
#define WIFI_RST 4
#define WIFI_EN 2
#define WIFI_LISTEN_PORT 80

// MusikMaker Pins
#define VS1053_RESET   -1     // VS1053 reset pin (not used!)
#define VS1053_CS       6     // VS1053 chip select pin (output)
#define VS1053_DCS     10     // VS1053 Data/command select pin (output)
#define SDCARD_CS       5     // Card chip select pin
// DREQ should be an Int pin *if possible* (not possible on 32u4)
#define VS1053_DREQ     9     // VS1053 Data request, ideally an Interrupt pin

////////////////////////////////////////////////////////////////////
// General Device specific Configuraiton (Strategy)
//

// Minimum distnace of object to start evasive maneuver
#define MIN_OBJECT_DISTANCE_CM 15

// Root directory for static web content on the SD Card
#define WWW_ROOT "/www-root"
