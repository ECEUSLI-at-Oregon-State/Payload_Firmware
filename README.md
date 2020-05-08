# Payload_Firmware
This repository contains all code files involved in the Payload system.

Software Needed: Arduino IDE,Processing IDE 3.5.3, Teensy to Arduno Programmer

The remote control and user input is configured and processed using Processing IDE 3.5.3.
Need to download the https://processing.org/download/ Processing IDE.

1. Must download the “wireless_xbox” folder in this folder:
  The “data” folder contains the remote configuration, and must be in the  “wireless_xbox” folder.
	The wireless_xbox.pde contains the code that sends the commands from input reading

2. Upload “Base Station.ino” to Base Station teensy board

3. Upload “Payload_transmitting_unit.ino” to Rover teensy board
	All motor pins and configurations stated in the code

4. Open the “wireless_xbox.pde” in Processing IDE
5. Select the correct port number for the microcontroller in Processing IDE
6. Run the “wireless_xbox.pde”, console should print reading of xbox controller
