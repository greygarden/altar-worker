#include <EEPROM.h>

String stringToWrite = "45CF96F8-990F-44DD-8FD9-D5D1FB23A762";
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
