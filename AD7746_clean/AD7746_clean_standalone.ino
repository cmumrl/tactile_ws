#include <Wire.h>

//ROS Libraries
//#include <ros.h>
//#include <std_msgs/Float64.h>

//AD7746 definitions
#define I2C_ADDRESS  0x48 //0x90 shift one to the rigth

#define WRITE_ADDRESS 0x90
#define READ_ADDRESS 0x91
#define RESET_ADDRESS 0xBF

#define REGISTER_STATUS 0x00
#define REGISTER_CAP_DATA 0x01
#define REGISTER_VT_DATA 0x04
#define REGISTER_CAP_SETUP 0x07
#define REGISTER_VT_SETUP 0x08
#define REGISTER_EXC_SETUP 0x09
#define REGISTER_CONFIGURATION 0x0A
#define REGISTER_CAP_DAC_A 0x0B
#define REGISTER_CAP_DAC_B 0x0B
#define REGISTER_CAP_OFFSET 0x0D
#define REGISTER_CAP_GAIN 0x0F
#define REGISTER_VOLTAGE_GAIN 0x11

#define RESET_ADDRESS 0xBF

#define VALUE_UPPER_BOUND 16000000L 
#define VALUE_LOWER_BOUND 0xFL
#define MAX_OUT_OF_RANGE_COUNT 3
#define CALIBRATION_INCREASE 1

//setup parameters
byte calibration;
byte outOfRangeCount = 0;

unsigned long offset = 0;
unsigned long offsetting;

//initialize the ROS node
//ros::NodeHandle nh;
float x;
//std_msgs::Float64  cap_msg;
//ros::Publisher chatter("chatter", &cap_msg);


void setup()
{

  Wire.begin(); // sets up i2c for operation
  Serial.begin(9600); // set up baud rate for serial

  //Serial.println("Initializing");

  Wire.beginTransmission(I2C_ADDRESS); // start i2c cycle
  Wire.write(RESET_ADDRESS); // reset the device
  Wire.endTransmission(); // ends i2c cycle

  //wait a tad for reboot
  delay(1);

  writeRegister(REGISTER_EXC_SETUP, _BV(3) | _BV(1) | _BV(0)); // EXC source A

  writeRegister(REGISTER_CAP_SETUP,_BV(7)| _BV(5)); // cap setup reg - cap enabled

  //Serial.println("Getting offset");
  offset = ((unsigned long)readInteger(REGISTER_CAP_OFFSET)) << 8;  
  //Serial.print("Factory offset: ");
  //Serial.println(offset);

  writeRegister(0x0A, _BV(7) | _BV(6) | _BV(5) | _BV(4) | _BV(3) | _BV(2) | _BV(0));  // set configuration to calib. mode, slow sample

  //wait for calibration
  delay(10);

  displayStatus();
  //Serial.print("Calibrated offset: ");
  offset = ((unsigned long)readInteger(REGISTER_CAP_OFFSET)) << 8;  
  //Serial.println(offset);

  writeRegister(REGISTER_CAP_SETUP,_BV(7)| _BV(5)); // cap setup reg - cap enabled

  writeRegister(REGISTER_EXC_SETUP, _BV(3)); // EXC source A

  writeRegister(REGISTER_CONFIGURATION, _BV(7) | _BV(6) | _BV(5) | _BV(4) | _BV(3) | _BV(0)); // continuous mode

  displayStatus();
  calibrate();


  //Serial.println("done");
  
  //initialization of ROS Publisher
 // nh.getHardware()->setBaud(9600);
 // nh.initNode();
 // nh.advertise(chatter);
  
}

void loop() // main program begins
{

  // if we recieve date we print out the status
  if (Serial.available() > 0) {
    // read the incoming byte:
    Serial.read();
    displayStatus();  

  }

  long value = readValue();
  unsigned long code;
  code = (value-calibration)* 2.44e-07-((int)calibration)*0.164;

 if ((value<VALUE_LOWER_BOUND) or (value>VALUE_UPPER_BOUND)) {
    outOfRangeCount++;
  }
  if (outOfRangeCount>MAX_OUT_OF_RANGE_COUNT) {
    if (value < VALUE_LOWER_BOUND) {
      calibrate(-CALIBRATION_INCREASE);
    } 
    else {
      calibrate(CALIBRATION_INCREASE);
    }
    outOfRangeCount=0;
  }
  x=value;
  Serial.println(value);
  //ros publisher on capacitance
 // cap_msg.data = x;
 // chatter.publish( &cap_msg );
 // nh.spinOnce();
 // delay(1);
}

void calibrate (byte direction) {
  calibration += direction;
  //assure that calibration is in 7 bit range
  calibration &=0x7f;
  writeRegister(REGISTER_CAP_DAC_A, _BV(7) | calibration);
}

void calibrate() {
  calibration = 0;

  long value = readValue();

  while (value>VALUE_UPPER_BOUND && calibration < 128) {
    calibration++;
    writeRegister(REGISTER_CAP_DAC_A, _BV(7) | calibration);
    value = readValue();
  }
}

long readValue() {
  long ret = 0;
  uint8_t data[3];

  char status = 0;
  //wait until a conversion is done
  while (!(status & (_BV(0) | _BV(2)))) {
    //wait for the next conversion
    status= readRegister(REGISTER_STATUS);
  }

  unsigned long value =  readLong(REGISTER_CAP_DATA);

  value >>=8;
  ret =value;
  return ret;
}
