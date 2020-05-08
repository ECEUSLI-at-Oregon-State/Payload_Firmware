//************************
// Payload Rover Firmware
//************************
/*
      Connections for Xbee = Serial Port 2
   ===========
   Connect Dout to Teensy 3.6 pin 9 
   Connect Din to Teensy 3.6 pin 10
   Connect VDD to 3.3-5V DC
   Connect GROUND to common ground

         Connections for GPS = Serial Port 3
   ===========
   Connect Dout to Teensy 3.6 pin 7
   Connect Din to Teensy 3.6 pin 8
   Connect VDD to 3.3-5V DC
   Connect GROUND to common ground
   
      Connections for IMU 
   ===========
   Connect SCL to Teensy 3.6 pin 19 SCL0
   Connect SDA to Teensy 3.6 pin 18 SDA0
   Connect VDD to 3.3-5V DC
   Connect GROUND to common ground
   
*/
//include required libraries
#include <stdio.h>
#include <string.h>
#include <AccelStepper.h>
#include <TinyGPS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

//define gloabal variables
#define xBeeSerial Serial2
#define gpsPort Serial3
TinyGPS gps;         
#define GPSECHO  true
bool usingInterrupt = false;
String message = "";
String temp = "";
String received = "";
char buf[32];
int enumerate = 0;
char *fields[20];
char *ptr = NULL;


// Define motor connections:
#define motor1_pwm 2
#define motor1_dir 7
#define motor2_pwm 6
#define motor2_dir 4

#define collectionM1 16
#define collectionM2 33
#define CollectionTRQ 20


void setup() {
  //serial port begins
  Serial.begin(9600);
  //set Xbeeport baud rate
  xBeeSerial.begin(9600);
  //initialize gps port
  gpsPort.begin(9600);
  //initialize sensor

  // Set all the motor control pins to outputs
  pinMode(collectionM1, OUTPUT);
  pinMode(collectionM2, OUTPUT);
  pinMode(motor1_pwm, OUTPUT);
  pinMode(motor1_dir, OUTPUT);
  pinMode(motor2_pwm, OUTPUT);
  pinMode(motor2_dir, OUTPUT);

  // Declare sensor pins as input:
//  pinMode(24, INPUT);
//  pinMode(25, INPUT);
//  pinMode(26, INPUT);
//  pinMode(27, INPUT);

  Serial.print("All initialized!");
  delay(100);
}

// This function will run both motors in forward direction at given speed
void forward(int speed){
// Turn on motor A
digitalWrite(motor1_dir, LOW);
analogWrite(motor1_pwm, speed);// Set speed 
// Turn on motor B
digitalWrite(motor2_dir, LOW);
analogWrite(motor2_pwm, speed);// Set speed in range 0~255
}

// This function will run both motors in backward direction at given speed
void backward(int speed){
// Turn on motor A
digitalWrite(motor1_dir, HIGH);
analogWrite(motor1_pwm, speed);// Set speed 
// Turn on motor B
digitalWrite(motor2_dir, HIGH);
analogWrite(motor2_pwm, speed);// Set speed in range 0~255
}

// This function will stop motors
void bothstop(){
// Turn off motor A
digitalWrite(motor1_dir, HIGH);
analogWrite(motor1_pwm, 0);// Set speed 
// Turn off motor B
digitalWrite(motor2_dir, HIGH);
analogWrite(motor2_pwm, 0);// Set speed in range 0~255
}

// This function will run both motors in spinleft direction at given speed
void spinleft(int speed){
// Turn on motor A
digitalWrite(motor1_dir, LOW);
analogWrite(motor1_pwm, speed);// Set speed 
// Turn on motor B
digitalWrite(motor2_dir, HIGH);
analogWrite(motor2_pwm, speed);// Set speed in range 0~255
}

// This function will run both motors in spinright direction at given speed
void spinright(int speed){
// Turn on motor A
digitalWrite(motor1_dir, HIGH);
analogWrite(motor1_pwm, speed);// Set speed 
// Turn on motor B
digitalWrite(motor2_dir, LOW);
analogWrite(motor2_pwm, speed);// Set speed in range 0~255
}

// This function will run both motors in leftforward direction at a fixed speed
void leftforward(int speed){
// Turn on motor A
digitalWrite(motor1_dir, HIGH);
analogWrite(motor1_pwm, int(speed/2));// Set speed 
// Turn on motor B
digitalWrite(motor2_dir, HIGH);
analogWrite(motor2_pwm, speed);// Set speed in range 0~255
}

// This function will run both motors in rightforward direction at a fixed speed
void rightforward(int speed){
// Turn on motor A
digitalWrite(motor1_dir, HIGH);
analogWrite(motor1_pwm, speed);// Set speed 
// Turn on motor B
digitalWrite(motor2_dir, HIGH);
analogWrite(motor2_pwm, int(speed/2));// Set speed in range 0~255
}


// This function controls collection up
void collection_up(int speed){
//int speed = 255;
// Turn on collection Motor
analogWrite(collectionM1, speed);
analogWrite(collectionM2, 0);// Set speed 
digitalWrite(CollectionTRQ, 0);// Set speed in range 0~255
}
// This function controls collection down
void collection_down(int speed){
//int speed = 255;
// Turn on collection Motor
analogWrite(collectionM1, 0);
analogWrite(collectionM2, speed);// Set speed 
digitalWrite(CollectionTRQ, 0);// Set speed in range 0~255
}


/*function to get IMU data*/
String getEvent_to_string(sensors_event_t* event){
/*gyroscope: rad/s
 *accelerometer: m/s^2
 *linearacceleration: m/s^2 
 *magnetometer:
 */
//  Serial.println();
//  Serial.print(event->type);
   String data = "";
  //initialize x,y,z with dumb values to spot problem easily
  double x = -1000000, y = -1000000 , z = -1000000; 
  //check the type of event detected by the sensor
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }
  else if (event->type == SENSOR_TYPE_ORIENTATION) {
    x = event->orientation.x;
    y = event->orientation.y;
    z = event->orientation.z;
  }
  else if (event->type == SENSOR_TYPE_MAGNETIC_FIELD) {
    x = event->magnetic.x;
    y = event->magnetic.y;
    z = event->magnetic.z;
  }
  else if ((event->type == SENSOR_TYPE_GYROSCOPE) || (event->type == SENSOR_TYPE_ROTATION_VECTOR)) {
    x = event->gyro.x;
    y = event->gyro.y;
    z = event->gyro.z;
  }

  data = String("x= " + String(x) + " | y= " + String(y) + " | z= " + String(z));
//  Serial.print(String("x= " + String(x) + " | y= " + String(y) + " | z= " + String(z)));

  return data;
}

void send_sensor_data(){
  enumerate += 1;
  message = String(enumerate);
  String accelData = "";

  //create sensors_event_t variables for accelerometer data wanted
//  sensors_event_t orientationData , angVelocityData, linearAccelData;
//  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
//   
//  //get accelerometer data
//  accelData = getEvent_to_string(&linearAccelData);

  accelData = "dummies";

  //add to message
  message = String(message + ",");
  message = message + String(accelData);

  bool newData = false;
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;){
    while (gpsPort.available()) {
      char c = gpsPort.read();
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  int year;
  uint8_t month, day, hour, minutes, second, hundredths;
  unsigned long age;
  float flat, flon;

  gps.f_get_position(&flat, &flon, &age);
  gps.crack_datetime(&year, &month, &day, &hour, &minutes, &second, &hundredths, &age);

  //  message = String(message + ",Time: ");
  message = String(message + ",");

  temp = String(year);
  temp = String(temp + "/");
  message = String(message + temp);

  temp = String(month);
  temp = String(temp + "/");
  message = String(message + temp);

  temp = String(day);
  temp = String(temp + " ");
  message = String(message + temp);

  temp = String(hour);
  temp = String(temp + ":");
  message = String(message + temp);

  temp = String(minutes);
  temp = String(temp + ":");
  message = String(message + temp);

  temp = String(second);
  temp = String(temp + ".");
  message = String(message + temp);

  temp = String(hundredths);
  message = String(message + temp);

  //  message = String(message + ",Lat: ");
  message = String(message + ",");
  temp = String(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);//Latitude
  temp = String("44.000000");//Lat of Oregon
  message = String(message + temp);

  //  message = String(message + ",Lon: ");
  message = String(message + ",");
  temp = String(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);//Longitude
  temp = String("-120.500000");//Lon of Oregon
  message = String(message + temp);

  message = String(message + ",");
  temp = String(calc_checksum(message));
  message = String(message + temp);
  message = String(message + "!\n");

//  message = "Testing with No Sensor data!";
  Serial.println("Transmitteed: ");
  Serial.println(message);
  xBeeSerial.println(message);
}


void receive_data(){
  temp = "";
  while (1){
    temp = xBeeSerial.read();
//    Serial.println(temp);
    if (temp == "!") {
      break;
    } else {
      received = received + temp;
    }
    delay(2);
  }
  while (xBeeSerial.available()){
    temp = xBeeSerial.read();
    delay(2);
  }
  temp = "";
}

void tokenize(String received, String fields[]) {
  int i = 0;
  char mssg[received.length()];
  received.toCharArray(mssg, received.length() + 1);
  ptr = strtok(mssg, ",");
  while (ptr != NULL) {
    fields[i] = String(ptr);
    i++;
    ptr = strtok(NULL, "!,");
  }
}


//calculate checksum
int calc_checksum(String msg) {
  int checksum = 0;
  for (int i = 0; i < msg.length(); i++) {
    if (msg[i] != ',') {
      checksum += msg[i];
    }
  }
  return (checksum %= 256);
}

//execute the program
void loop(){
  String fields[50];
  String direct;
  int speedvalue = 0;

  //print to debug
//  Serial.println(xBeeSerial.available());
  xBeeSerial.flush();
  //receive data for direction
  if (xBeeSerial.available()){
//    Serial.println("here!");
    //receive user input
//    received = xBeeSerial.readString();
//    Serial.println(temp);
    receive_data();
    Serial.println("Received: ");
    Serial.println(received);
    if(received.indexOf(",") >= 0){
    tokenize(received, fields);
    direct = fields[0];
    speedvalue = fields[1].toInt();
    received = "";
    }
 

  //call related movement function
    if(direct == "f"){
      Serial.println("calling forward");
      forward(speedvalue);
    }
    if(direct == "b"){
      Serial.println("calling backward");
      backward(speedvalue);
    }
    if(direct == "r"){
      Serial.println("calling spinright");
      spinright(speedvalue);
    }
    if(direct == "l"){
      Serial.println("calling spinleft");
      spinleft(speedvalue);
    }
//    if(direct == "rf"){
//      rightforward(speedvalue);
//    }
//    if(direct == "lf"){
//      leftforward(speedvalue);
//    }
    if(direct == "stop"){
      Serial.println("calling stop");
      bothstop();
    }
    
    //collection system motor
    if(direct == "up"){
      Serial.println("calling up");
      collection_up(speedvalue);
    }
    if(direct == "low"){
      Serial.println("calling down");
      collection_down(speedvalue);
    }
    
  }
    //send sensor data 
//    send_sensor_data();
delay(10);
}
