#include <kSeries.h>//include kSeries Library
#include <math.h>//for log() and NAN, used in the NTC conversion below

/*!
 * @file environmental_chamber.ino
 * @brief Reads a K30 CO2 sensor (serial) and an NTC 10k thermistor (analog
 *        divider) to drive a CO2 solenoid valve and 3 chamber heaters.
 *        DHT11 has been removed from this build -- no humidity sensor present.
 */

//
//
//NTC DIVIDER CALIBRATION -- placeholder values, replace before trusting temp readings
const float R_FIXED  = 10000.0; // TODO: real value of the fixed resistor, ohms
const float NTC_R25  = 10000.0; // TODO: thermistor resistance at 25C, ohms (datasheet)
const float NTC_BETA = 3950.0;  // TODO: thermistor beta coefficient (datasheet)
//SECTION ENDS AS CALIBRATION CONSTANTS DEFINED
//
//

//
//
//defining variables

float temp; // declaring temp monitoring variable (deg C, from NTC divider)
int ntcRaw; // raw analogRead() of the NTC divider -- handy while calibrating
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
int enA = 9; // pin 9 is set for enA -- motor ctrl 1 OUT1 -> CO2 solenoid valve
// to run heating foil connected to enB
int enB = 11; // pin 11 is set for enB -- shared "teal 1" bus feeding motor ctrl 1 ENB
              // (heater 1), motor ctrl 2 ENA (heater 2) and ENB (heater 3). One PWM
              // signal, three heaters wired in parallel on the enable line.
int alarm = 7; // alarm is at pin 7 -- not currently wired to anything
int ntcPin = 8; // pin 8 -- digital supply rail for the NTC divider ("teal 2")
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

  pinMode(ntcPin, OUTPUT);
  digitalWrite(ntcPin, HIGH); // TODO: confirm this stays HIGH permanently as the
                              // divider's supply, vs. only being pulsed before a reading
  //SECTION ENDS AS PINMODES ARE SET
  //
  // 
}

// Converts a raw analogRead() of the NTC divider into deg C (beta equation).
// Node sits between R_FIXED (to D8/5V) and the NTC (to GND), so R_NTC = R_FIXED * raw/(1023-raw).
float ntcRawToCelsius(int raw){
  if (raw <= 0 || raw >= 1023) return NAN; // guards divide-by-zero at either rail
  float r_ntc = R_FIXED / (1023.0 / raw - 1.0);
  float steinhart = log(r_ntc / NTC_R25) / NTC_BETA;
  steinhart += 1.0 / (25.0 + 273.15);
  return (1.0 / steinhart) - 273.15;
}

void loop(){

  //
  //
  //WE ARE READING SENSORS HERE
  ntcRaw = analogRead(A0); // reading the NTC divider
  temp = ntcRawToCelsius(ntcRaw); // converting to deg C
  co2 = 10*K_30.getCO2('p'); // setting the co2 value from K30. // p for ppm. change p tp % to see in percentage
  //SECTION ENDS AS ALL READING DONE
  //
  //

  //
  //
  //THIS SECTION SHOWS READING TO SERIAL MONITOR
  Serial.print(temp); // fetching the temperature variable from the NTC divider
  Serial.print(",");
  Serial.print(ntcRaw); // raw ADC count -- useful while dialing in the constants above
  Serial.print(",");
  Serial.println(co2); //fetching the co2 variable from k30
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
  // WE ARE TRIGGERING HEATING SYSTEM HERE (all 3 chamber heaters, via the teal-1 bus)
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
  //temp having 0 or 255 means sensor problem //co2 < 0 means co2 sensor is not active
  //(humidity condition removed -- no humidity sensor on this build)
 // if (temp ==0 || temp == 255 || co2<0){ 
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
