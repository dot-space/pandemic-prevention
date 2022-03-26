#include <DS3231.h> //Library used for RTC module
#include <MFRC522.h>    //Library used for RFID sensor
#include <SPI.h> //Library used for SPI communication
#include <SD.h> //Library used for communicating with micro SD card
#include <Servo.h>  //Library used to control servo motors

DS3231  rtc(SDA, SCL);
MFRC522 mfrc522(10, 9);  // As SS and RST pins of RFID sensor are connected to digital pins 10 and 9 of the arduino
Servo servo_oxygen;
Servo servo_temperature;
Servo servo_sanitiser;
Servo servo_latch;

void setup()
{
Serial.begin(9600);     //9600 baud rate
SPI.begin();
SD.begin(4);      // As we have connected CS pin of SC card module to digital pin 4 of arduino
rtc.begin();
mfrc522.PCD_Init();
servo_oxygen.attach(5);             // Servo motors are connected to pins 5, 6 of the arduino
servo_temperature.attach(6);
servo_sanitiser.attach(2);             
servo_latch.attach(3);
servo_sanitiser.write(0);
servo_latch.write(0);
}

void loop()
{
    if ( mfrc522.PICC_IsNewCardPresent())                   // If card is present
  {
      if ( mfrc522.PICC_ReadCardSerial())                      //Read card value (RAW)
    {
        String content= "";
        byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)                       //Converting values to readable and editable format (HEX)
  {
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
     content.toUpperCase();
  }
     if (content.substring(1) == "82 24 37 A2" || content.substring(1) == "58 B3 72 A3" || content.substring(1) == "32 A6 32 20" )
        {

    servo_temperature.write(90);        //Triggers temperature check
    delay(15);
    servo_temperature.write(0);
    delay(3000);                                   // Waits for 3 seconds
    servo_oxygen.write(120);               //Opens oximeter
    delay(5000);
    servo_oxygen.write(0);                     // Servo retracts so as the oximeter can start measuring oxygen
    servo_sanitiser.write(120);
    delay(2000);
    servo_latch.write(180);

    File dataFile = SD.open("Covid_scan_times.txt", FILE_WRITE);    //Opens file
    dataFile.print(rtc.getDateStr());             // Prints date
    dataFile.print(",");
    dataFile.print(rtc.getTimeStr());           //Prints time
    dataFile.print(",");
   dataFile.print(content.substring(1));        //Prints RFID Value
    dataFile.println();     // Prints new line
    dataFile.close();       //Closes file
        }
    }
  }
}
