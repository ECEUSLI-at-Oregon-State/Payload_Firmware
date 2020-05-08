//************************
// Payload Ground Station Firmware
//************************
/*
    Connections for Xbee = Serial Port 2
 ===========
 Connect Dout to Teensy 3.6 pin 9
 Connect Din to Teensy 3.6 pin 10
 Connect VDD to 3.3-5V DC
 Connect GROUND to common ground
 */
 
#include <stdio.h>
#include <string.h>
#include <Arduino.h>


//define gloabal variables
#define xBeeSerial Serial2
#define arduinoSerial Serial4

String received = "";
String received_user = "";
String temp = "";
String temp_user = "";
char *ptr = NULL;

void setup(){
  // Initialize USB serial
  Serial.begin(9600);
  while (!Serial) {};

  //Initialize XBee UART
  xBeeSerial.begin(9600);
  while (!xBeeSerial) {};
  
//    // Initialize Arduino UART
//  arduinoSerial.begin(9600);
//  while (!arduinoSerial) {};

  Serial.println("All ports Initialized");
  pinMode(13, OUTPUT);
}

//Read Serial port to get user input from computer-Processing
void receive_user_input(){
  temp_user = "";
  while (1) {
    temp_user = Serial.read();
    if (temp_user == "!") {
      break;
    } else {
      received_user = received_user + temp_user;
    }
    delay(2);
  }
  while (Serial.available()) {
    temp_user = Serial.read();
    delay(2);
  }
  temp_user = "";
}

//Read Xbee Serial port to get sensor data from rover
void receive_data() {
  temp = "";
  while (1) {
    temp = xBeeSerial.read();
    if (temp == "!") {
      break;
    } else {
      received = received + temp;
    }
    delay(2);
  }
  while (xBeeSerial.available()) {
    temp = xBeeSerial.read();
    delay(2);
  }
  temp = "";
}

// get individual argument in user input
void tokenize_user(String received_user, String input[]) {
  int i = 0;
  char mssg[received_user.length()];
  received_user.toCharArray(mssg, received_user.length() + 1);
  ptr = strtok(mssg, ",");
  while (ptr != NULL) {
    input[i] = String(ptr);
    i++;
    ptr = strtok(NULL, "!,");
  }
}

// get individual argument in sensor data
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

//for checking data packets
String reconstruct_msg(String fields[]) {
  int i = 0;
  String message = "";
  // Rebuild message string to calculate checksum with
  for (i = 0; i < 5; i++) {
    message += fields[i];
  }
  return message;
}

//for verifying data packets
//bool verify_checksum(String msg, String fields[]) {
//  int checksum = 0;
//  for (int i = 0; i < msg.length(); i++) {
//    if (msg[i] != ',') {
//      checksum += msg[i];
//    }
//  }
//  checksum %= 256;
////  Serial.println("Calculated " + String(checksum));
////  Serial.println("Comparing with " + fields[5]);
//  if (checksum == fields[5].toInt()) {
//    return true;
//  }
//  else {
//    return false;
//  }
//}


//Execute the program
void loop(){
    String fields[50];
    String input[50];
    String direct;
    int speedvalue = 0;

    //check computer serial to get user data
    if (Serial.available()){

    //receives user command
    receive_user_input();
    tokenize_user(received_user, input);
    direct = input[0];
    speedvalue = input[1].toInt();
    
    }
    //send the user input to Rover Xbee
    xBeeSerial.println(received_user+"!");

  //check for Xbee commmunication
  if (xBeeSerial.available()){
    //receive sensor data from rover
//    receive_data();
//    arduinoSerial.println(received);
//    Serial.println("Received: ");
//    Serial.println(received);

    //get individual argument
//    tokenize(received, fields);
//    String check_checksum = reconstruct_msg(fields);
//    if (verify_checksum(check_checksum, fields)) {
//      Serial.println("ACK: " + String(fields[0]) + "!");
//    }

  }

  //call related movement function
//    if(direct == "f"){
//      digitalWrite(13, HIGH);
//      delay(100);
//    }
//    if(direct == "b"){
//      digitalWrite(13, HIGH);
//      delay(100);
//    }
//    
//  digitalWrite(13, LOW);
  received_user = "";
  received = "";
  delay(10);
}
