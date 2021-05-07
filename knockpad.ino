#include<Servo.h>
#include<LiquidCrystal.h>
#include "SparkFun_Qwiic_Keypad_Arduino_Library.h"

//time taken for servo to position itself at 90 degrees (value was 225 at first, but then I realized that it wouldnt turn completely, which is why i increased it a little)
#define TURN 230

//Servo and Keypad declarations 
Servo servo1;
Servo servo2;
KEYPAD keypad;

//pin initiation
const int green = 9;
const int red = 8;
const int blue = 7;
const int firstServo = 22;
const int secondServo = 23;
const int RS = 11, EN = 12, D4 = 2, D5 = 3, D6 = 4, D7 = 5;

//LCD declaration
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

//value initiation and variable initiation
int threshold = 100;
int sensorReading = 0;
int ledState = LOW;
int counter = 0;
int keypadCounter;
boolean knockDone;
String password;
String realPass;
int r;

void setup() {
  r = random(3);
  keypadCounter = 0;
  servo1.attach(firstServo);
  servo2.attach(secondServo);
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(blue,OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Knock on door!");
  // https://github.com/sparkfun/SparkFun_Qwiic_Keypad_Arduino_Library/blob/master/examples/Example1_ReadButton/Example1_ReadButton.ino
  // Using begin() like this will use default I2C address, 0x4B. 
  // You can pass begin() a different address like so: keypad1.begin(Wire, 0x4A).
  if (keypad.begin() == false)   
  {
    lcd.print("Keypad disconnected");
    while (1);
  }
  redOn();
}

void loop() {
  if (counter == -1) {
    //counter is incremented here
    resetAfterSuccess();
  }
  //read the sensor if we tap or not
  sensorReading = analogRead(A7);
  if (sensorReading <= threshold) {
    ledState = !ledState;
    toggleblue();
    counter++;
  }
  //this program is designed to detect 6 knocks as a pattern and then put in the password. 
  if (counter == 6) {
    //counter is incremented here
    knockSuccessful();
  }
  if (counter == 7) {
    //counter is incremented here
    lcdSetup();
  }
  if (counter == 8) {
    //UPDATING FIFO so that it recognizes the input button when we press it, otherwise it wont be able to register any input
    keypad.updateFIFO();
    char button = keypad.getButton();
    realPass = randomPassword(r);
    //If we press a button, we want to print it
    if (button){
      lcd.print(button);
      if(button == '*') {
        clearPassword();
      } 
      else if(button == '#') {
        if(password == realPass) {
          passwordSuccess();
        } 
        else {
          passwordFail();
          keypadCounter++;
          if (keypadCounter == 3) {
            systemReset();
          }
        }
         
        password = ""; // clear input password
      } else {
        password += button; // append new character to input password string
      }
    }
  }

  //I delayed a bit so that i dont tie up the I2C bus of the Keypad
  delay(25); //25 is good, more is better
  delay(100); //i did more just in case
}

/**
 * This method turns the red LED on
 */
void redOn() {
  digitalWrite(red, HIGH);
}

/**
 * This method turns the green LED on
 */
void greenOn() {
  digitalWrite(green, HIGH);
}

/**
 * This method turns the red LED off
 */
void redOff() {
  digitalWrite(red, LOW);
}

/**
 * turns the green LED off
 */
void greenOff() {
  digitalWrite(green, LOW);
}

/**
 * toggles blue LED to show that a knock has been registered
 */
void toggleblue() {
  digitalWrite(blue, ledState);
}

/**
 * This method returns the random password through the random number generated through the random class.
 */
String randomPassword(int randomNumber) {
  if (randomNumber == 0) return "1234";
  if (randomNumber == 1) return "4295";
  if (randomNumber == 2) return "7410";
}

/**
 * This method resets the state-of-the-art lock system after a person has been granted access.
 */
void resetAfterSuccess() {
  delay(10000);
  servo1.write(180);
  delay(TURN);
  servo1.write(90);
  servo2.write(180);
  delay(TURN);
  servo2.write(90);
  lcd.clear();
  lcd.print("Knock on door!");
  counter++;
  redOn();
  greenOff();
}

/**
 * This method unlocks a lock once the knocks have been registered.
 */
void knockSuccessful() {
  lcd.clear();
  lcd.print("Knock Accepted!");
  servo2.write(0);
  delay(TURN);
  servo2.write(90);
  counter++;
  delay(1500);
}

/**
 * This method sets up the LCD for the user to enter a password through the keypad
 */
void lcdSetup() {
  lcd.clear();
  lcd.print("Enter:");
  lcd.setCursor(0, 1);  
  counter++;
}

/**
 * This method unlocks a lock, and confirms that the door has been unlocked. It also turns the red LED off and the green LED on.
 */
void passwordSuccess() {
  lcd.clear();
  lcd.print("Correct Password");
  lcd.setCursor(0, 1);
  lcd.print("Door Unlocked!");
  delay(500);
  lcd.setCursor(0, 0);
  servo1.write(0);
  delay(TURN);
  servo1.write(90);
  r = random(3);
  counter = -1;
  keypadCounter = 0;
  redOff();
  greenOn();
}

/**
 * This method displays a message on the LCD if the password is incorrect.
 */
void passwordFail() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Incorrect!");
  lcd.setCursor(0, 1);
}

/**
 * This method resets the system if the password inputted by the user is wrong three times in a row for state-of-the-art security purposes.
 */
void systemReset() {
  servo2.write(180);
  delay(TURN);
  servo2.write(90);
  counter = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Knock on door!");
}

/**
 * This method clears the password inputted if the "*" button was pressed.
 */
void clearPassword() {
  password = ""; 
  lcd.clear();
  if (keypadCounter == 0) {
    lcd.print("Enter:");
    lcd.setCursor(0, 1);
  }
  else {
    lcd.print("Incorrect!");
    lcd.setCursor(0, 1);
  }
}
