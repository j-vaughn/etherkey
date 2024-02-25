#include "usb-keyboard.h"

// Updated for Leonardo board with code from https://www.sjoerdlangkemper.nl/2022/11/16/running-etherkey-on-arduino-leonardo/
#ifdef ARDUINO_AVR_LEONARDO
#include "Keyboard.h"

#define KEY_UP KEY_UP_ARROW
#define KEY_DOWN KEY_DOWN_ARROW
#define KEY_RIGHT KEY_RIGHT_ARROW
#define KEY_LEFT KEY_LEFT_ARROW

#define KEYPAD_PLUS KEY_KP_PLUS
#define KEYPAD_0 KEY_KP_0

#define KEY_ENTER KEY_RETURN
#define KEY_SPACE ' '

#define MODIFIERKEY_CTRL KEY_LEFT_CTRL
#define MODIFIERKEY_ALT KEY_LEFT_ALT
#define MODIFIERKEY_SHIFT KEY_LEFT_SHIFT
#define MODIFIERKEY_GUI KEY_LEFT_GUI

#define keyboard_leds 0
#endif

char in_ascii;
char kbd_buff[KBD_BUFFSZ];
int kbd_idx = 0;
int crs_idx = 0;

int mode = 1;
int newmode = 0;
enum mode {INVALID, COMMAND, INTERACTIVE, DEBUG};
const char* mode_strings[] = {"invalid", "command", "interactive", "debug"};
const char* selectMode = "Select Inputmode: [1] Command - [2] Interactive - [3] Debug";

void setup() {
  HWSERIAL.begin(57600);
  delay(1000);
  SerialClear();
  SerialPrintfln("Switching to %s mode", mode_strings[mode]);
}

void loop() {
  if (HWSERIAL.available() > 0) {
    in_ascii = HWSERIAL.read();

    if (in_ascii<0 || in_ascii>127)
      // Ignore non-basic ascii characters
      return;

    if ((newmode = mode_select(in_ascii, mode))) {
      mode = newmode;
      return;
    }

    switch(mode) {
      case COMMAND:
        command_mode(in_ascii);
        break;

      case INTERACTIVE:
        interactive_mode(in_ascii);
        break;

      case DEBUG:
        debug_mode(in_ascii);
        break;
    }
  }
}
