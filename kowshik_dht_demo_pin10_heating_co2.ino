#include <kSeries.h>//include kSeries Library

/*!
 * @file readDHT11.ino
 * @brief DHT11 is used to read the temperature and humidity of the current environment. 
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Wuxiao](xiao.wu@dfrobot.com)
 * @version  V1.0
 * @date  2018-09-14
 * @url https://github.com/DFRobot/DFRobot_DHT11
 */

#include <DFRobot_DHT11.h>  

DFRobot_DHT11 DHT; // library for operating DHT sensor
#define DHT11_PIN 10 // setting the digital pin 10 for DHT

//
//
//defining variables

int temp; // declaring temp monitoring variable
int hum; // declaring humidity monitoring variable
double co2; //declaring co2 monitoring variable

//SECTION ENDS AS VARIABLES DEFINED
//
//

//
//
//PIN SET UP FOR CERTAIN OPERATIONS
//In motor driver IC, we can control 2 devices independently. They are controlled by their enA or enB. 
//enA or enB takes value between 0 to 255 which basicallly controls the speed of the motor
//in1 or in2 are the directionality of the motor rotation which can be set by logic in code or from external source
// to run solinoid valve connected to enA
int enA = 9; // pin 9 is set for enA //to be set to 255 PWM; (in a range of 0-255; this controls the output voltage of the motor driver)
// to run heating foil connected to enB
int enB = 11; // pin 11 is set for enB // to be set to 255 PWM; (in a range of 0-255; this controls the output voltage of the motor driver)
int alarm = 7; // alarm is at pin 7
// We are setting TX-RX Here
kSeries K_30(12,13); //Initialize a kSeries Sensor with pin 12 as Rx and 13 as Tx
//SECTION ENDS AS ALL PINS HAVE BEEN SET
//
//

//
//
//MAIN RUN
void setup(){
  Serial.begin(115200); // establishing contact with PC. (will not be required while doing remote work)

  //
  //
  //WE SHALL SET PINS AS INPUT OR OUTPUT
  pinMode(alarm, OUTPUT); //buzzer alarm pin is set as output
  //keeping alarm off initially
  digitalWrite(alarm, LOW); //alarm has been set at a digital key. So, digital write
  
  // defining outout for the 3 motor driver pins
  pinMode(enA, OUTPUT);
  //keeping solenoid valve off initially
  analogWrite(enA, 0); // pins can give pwm to analog write

  pinMode(enB, OUTPUT);
  //keeping heating system off initially
  analogWrite(enB, 0); // pins can give pwm to analog write
  //SECTION ENDS AS PINMODES ARE SET
  //
  // 
}



void loop(){

  //
  //
  //WE ARE READING SENSORS HERE
  DHT.read(DHT11_PIN); // reading through DHT sensor
  temp = DHT.temperature; // setting the temperature value to previously declared temp variable
  hum = DHT.humidity; // setting the humidity value to previously declared hum variable
  co2 = 10*K_30.getCO2('p'); // setting the humidity value to previously declared hum variable. // p fpr ppm. change p tp % to see in percentage
  //SECTION ENDS AS ALL READING DONE
  //
  //

  //
  //
  //THIS SECTION SHOWS READING TO SERIAL MONITOR
  Serial.print(temp); // fetching the temperature variable from DHT
  Serial.print(",");
  Serial.print(hum); // fetching the humidity variable from DHT
  Serial.print(",");
  Serial.println(co2); //fetching the humidity variable from k30
  // SECTION ENDS AS READING IS DONE
  //
  //

  //
  //
  // WE ARE TRIGGERING SOLENOID VALVE HERE
  if (co2 > 5000 ){ //setting the CO2 level we need to maintain
    // turning on solenoid valve
    analogWrite(enA, 255);// in1 and in2 are high and low respectively from circuit. So, I am not using logic
    delay(2000);// running valve for 2 seconds
  }
  if (co2 <5000){
    // otherwise keeping it off
    analogWrite(enA, 0); 
  }
  //SECTION ENDS HERE FOR SOLENOID VALVE CONTROL
  //
  //

  //
  // WE ARE TRIGGERING HEATING SYSTEM HERE
  if (temp < 37 ){ // setting the TEMPERATURE we need to maintain
  // turning on heating foil
  analogWrite(enB, 255); // in1 and in2 are high and low respectively from circuit. So, I am not using logic
  }
  // otherwise keeping it off
  if (temp >37){
  analogWrite(enB, 0);
  }
  //SECTION ENDS HERE FOR HEATING SYSTEM CONTROL
  //
  //

  //
  //
  //WE ARE TRIGGERING ALARM HERE FOR ANY OF THE FOLLOWING CONDITIONS
  //hum below 85 means water level low //temp or hum having 0 or 255 means sensor problem //co2 < 0 means co2 sensor is not active
 // if (hum < 85 || temp ==0 || temp == 255 || co2<0){ 
   // Serial.println("Stop Immediately");
    //digitalWrite(alarm, HIGH);
     //delay(2000); //BUZZER IS ON FOR 2 SECONDS 
    //digitalWrite(alarm, LOW);
   // delay(1000); //BUZZER IS OFF FOR 8 SECONDS 
 // }
  //SECTION ENDS HERE FOR ALARM SYSTEM
  //
  //

  delay(1000);
}

//THE END
