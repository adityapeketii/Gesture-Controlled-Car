//These are needed for MPU


#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

//These are need for RF handlling
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define PRINT_DEBUG   //Uncomment this line if you want to print the MPU6050 initialization information on serial monitor

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// MPU control/status vars
MPU6050 mpu;
bool dmpReady = false;  // set true if DMP init was successful
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint8_t fifoBuffer[64]; // FIFO storage buffer
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// RF control
const uint64_t pipeOut = 0xF9E8F0F0E1LL;   //IMPORTANT: The same as in the receiver 0xF9E8F0F0E1LL
RF24 radio(8, 9); // select CE,CSN pin

struct PacketData 
{
  byte xAxisValue;
  byte yAxisValue;
} data;

void setupRadioTransmitter()
{
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  radio.stopListening(); //start the radio comunication for Transmitter  

  data.xAxisValue = 127; // Center
  data.yAxisValue = 127; // Center 
}


void setupMPU()
{
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
      Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif

  #ifdef PRINT_DEBUG
    // initialize serial communication
    Serial.begin(115200);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately
    // initialize device
    Serial.println(F("Initializing I2C devices..."));
  #endif
  
  mpu.initialize();

  #ifdef PRINT_DEBUG  
    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
    // wait for ready
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again
    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
  #endif
  
  devStatus = mpu.dmpInitialize();
  
  // make sure it worked (returns 0 if so)
  if (devStatus == 0) 
  {
      // Calibration Time: generate offsets and calibrate our MPU6050
      mpu.CalibrateAccel(6);
      mpu.CalibrateGyro(6);
      
      #ifdef PRINT_DEBUG      
        mpu.PrintActiveOffsets();
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
      #endif
      mpu.setDMPEnabled(true);
  
      // set our DMP Ready flag so the main loop() function knows it's okay to use it
      #ifdef PRINT_DEBUG      
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
      #endif
      dmpReady = true;
  
      // get expected DMP packet size for later comparison
      packetSize = mpu.dmpGetFIFOPacketSize();
  } 
  else 
  {
      // ERROR!
      // 1 = initial memory load failed
      // 2 = DMP configuration updates failed
      // (if it's going to break, usually the code will be 1)
      #ifdef PRINT_DEBUG       
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
      #endif
  }
}


void setup()
{
  //This is to set up radio transmitter for rf
  setupRadioTransmitter();   
  //This is to set up MPU6050 sensor
  setupMPU();
}

void loop() 
{
  // if programming failed, don't try to do anything
  if (!dmpReady) return;
  // read a packet from FIFO. Get the Latest packet
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) 
  {  
    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    int xAxisValue = constrain(ypr[2] * 180/M_PI, -90, 90);
    int yAxisValue = constrain(ypr[1] * 180/M_PI, -90, 90);
    data.xAxisValue = map(xAxisValue, -90, 90, 0, 254); 
    data.yAxisValue = map(yAxisValue, -90, 90, 254, 0);

    radio.write(&data, sizeof(PacketData));

    #ifdef PRINT_DEBUG  
      Serial.println(xAxisValue);  
      Serial.println(yAxisValue);        
    #endif
  }
}