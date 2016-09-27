#include <EEPROM.h>

String stringToWrite = "A9560361-CC9F-4C7A-9965-C31A9E344EA8";
int currentCharPosition = 0;

void setup() {
  /** Empty setup. **/
}

void loop() {

  while (currentCharPosition < stringToWrite.length()) {
    /***
    Write the value to the appropriate byte of the EEPROM.
      these values will remain there when the board is
      turned off.
    ***/
    
    EEPROM.write(currentCharPosition, stringToWrite.charAt(currentCharPosition));

    /***
    As the EEPROM sizes are powers of two, wrapping (preventing overflow) of an
    EEPROM address is also doable by a bitwise and of the length - 1.
    ***/
    currentCharPosition++;
  }
}
