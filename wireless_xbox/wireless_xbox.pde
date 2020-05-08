//This is the program from remote control
//import required libraries
import processing.serial.*;
import net.java.games.input.*;
import org.gamecontrolplus.*;
import org.gamecontrolplus.gui.*;
import cc.arduino.*;
import org.firmata.*;

//declare global variables
ControlDevice cont;
ControlIO control;
float fb_thumb;
float lr_thumb;
float ud_thumb;
Serial port;

void setup() {
  size(200, 200);
  
  //get remote controller device
  control = ControlIO.getInstance(this);
  cont = control.getMatchedDevice("xbox");
  if (cont == null) {
    println("Not controller found"); // write better exit statements than me
    System.exit(-1);
  }
  
  //set up port
  println(Serial.list());
  String XbeePort = Serial.list()[0];
  port = new Serial(this, XbeePort, 9600);
  
}

//get input from Xbox joystick
public void getUserInput() {
  fb_thumb = map(cont.getSlider("FBdirection").getValue(), -1, 1, -255, 255);
  lr_thumb = map(cont.getSlider("LRdirection").getValue(), -1, 1, -255, 255);
  ud_thumb = map(cont.getSlider("UDdirection").getValue(), -1, 1, -255, 255);
}

//loop to run the program
void draw() {
  //updates the thumb value and uses it to change background to give a visual indicator of the change in value
  getUserInput();
  //background(fb_thumb, 100, 255);
  
  println("fb: "+fb_thumb);
  println("lr: "+lr_thumb);
  println("ud: "+ud_thumb);  
  println(" ");
  
  //turn reading of joystick into speed
  int fbspeed = int(abs(fb_thumb));
  int lrspeed = int(abs(lr_thumb));
  int udspeed = int(abs(ud_thumb));

  //rover going forward
  if((fb_thumb < -127) && (abs(lr_thumb)<=100)){
    port.write("f"+","+ str(fbspeed)+"!");
    //port.write("f");
    println("f"+","+ str(fbspeed)+"!");
  }
  //rover going backward
  if((fb_thumb > 127) && (abs(lr_thumb)<=100)){
    port.write("b"+","+ str(fbspeed)+"!");
    println("b"+","+ str(fbspeed)+"!");
  }
  //rover turning right
  if((lr_thumb > 127) && (abs(fb_thumb)<=100)){
    port.write("r"+","+ str(lrspeed)+"!");
    println("r"+","+ str(lrspeed)+"!");
  }
  //rover turning left
  if((lr_thumb < -127) && (abs(fb_thumb)<=100)){
    port.write("l"+","+ str(lrspeed)+"!");
    println("l"+","+ str(lrspeed)+"!");
  }
  
  ////rover turning rightforward
  //if((fb_thumb < -10) && (fb_thumb >= -127) && (lr_thumb > 10)){
  //  port.write("rf"+","+ str(fbspeed)+"!");
  //  println("rf"+","+ str(fbspeed)+"!");
  //}
  ////rover turning leftforward
  //if((fb_thumb < -10) && (fb_thumb >= -127) && (lr_thumb < -10)){
  //  port.write("lf"+","+ str(fbspeed)+"!");
  //  println("lf"+","+ str(fbspeed)+"!");
  //}
  
  //rover stopping
  if((abs(fb_thumb)<=10) && (abs(lr_thumb)<=10) && (abs(ud_thumb)<=10)){
    //bothstop();
    port.write("stop"+","+str(0)+"!");
    println("stop"+","+str(0)+"!");
  }
  //collection going up 
  if((ud_thumb < -127) && (abs(fb_thumb)<=10)&& (abs(lr_thumb)<=10)){
    port.write("up"+","+str(udspeed)+"!");
    println("up"+","+str(udspeed)+"!");
  }
   //collection going down
  if((ud_thumb > 127) && (abs(fb_thumb)<=10) && (abs(lr_thumb)<=10)){
    port.write("low"+","+str(udspeed)+"!");
    println("low"+","+str(udspeed)+"!");
  }
  
  delay(10);
}
