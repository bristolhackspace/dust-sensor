#include <EEPROM.h>

void setupEEPROM()
{
    EEPROM.begin(512);
}

//This function will write a 2 byte (16bit) unsigned int to the eeprom at
//the specified address to address + 1.
void EEPROMWriteInt(int address, unsigned int value)
{
  //Decomposition from an int to 2 bytes by using bitshift.
  //One = Most significant -> two = Least significant byte
  byte two = (value & 0xFF);
  byte one = ((value >> 8) & 0xFF);

  //Write the 2 bytes into the eeprom memory.
  EEPROM.write(address, two);
  EEPROM.write(address + 1, one);
  EEPROM.commit();
}

unsigned int EEPROMReadInt(int address)
{
  //Read the 2 bytes from the eeprom memory.
  long two = EEPROM.read(address);
  long one = EEPROM.read(address + 1);

  //Return the recomposed int by using bitshift.
  return ((two << 0) & 0xFF) + ((one << 8) & 0xFFFF); 
}

void update_lcd(unsigned long lowpulseoccupancy)
{
    Serial.println("updating lcd");
    lcd.clear();
    lcd.setCursor(0, 0);
    switch(state)
    {
        case NOT_CONNECTED:
            lcd.print("not connected");
            break;
        case SAMPLING:
            lcd.print("sampling");
            break;
        case CHECK_WIFI:
            lcd.print("checking wifi");
            break;
        case POSTING:
            lcd.print("posting");
            break;
    }
    yield();

    lcd.setCursor(0, 2);
    lcd.print("LPO:");
    lcd.print(lowpulseoccupancy);
    yield();

    lcd.setCursor(0, 3);
    lcd.print("time:");
    lcd.print(millis()/1000);
    yield();

    lcd.setCursor(0, 4);
    lcd.print("reboots:");
    lcd.print(EEPROMReadInt(EEP_REBOOTS));
    yield();

    lcd.setCursor(0, 5);
    lcd.print("reconns:");
    lcd.print(EEPROMReadInt(EEP_WIFI_CONN));
    yield();
}
