//#include <107-Arduino-MCP2515.h>
#include <mcp_can.h>
#include <SPI.h>
#include <Wire.h>



long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128]; // Array to store serial string

#define CAN0_INT 2 // Set INT to pin 2
MCP_CAN CAN0(10); // Set CS to pin 10

#define SLAVE_ADDR 9


// Define 8-byte data buses
byte CoolantTemp[] = {0x02, 0x01, 0x05, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
byte rpm[] = {0x02, 0x01, 0x0C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
byte vspeed[] = {0x02, 0x01, 0x0D, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
byte IATSensor[] = {0x02, 0x01, 0x0F, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
byte MAFSensor[] = {0x02, 0x01, 0x10, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
byte AmbientAirTemp[] = {0x02, 0x01, 0x46, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
byte EngineOilTemp[] = {0x02, 0x01, 0x5C, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
byte EngineTorque[] = {0x02, 0x01, 0x63, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
byte FuelTank[] = {0x02, 0x01, 0x2F, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
byte ThrottlePos[] = {0x02, 0x01, 0x11, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

void setup() {
  Wire.begin();
  Serial.begin(9600);
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL); // Set operation mode to normal so the MCP2515 sends acks to received data.
  pinMode(CAN0_INT, INPUT); // Configuring pin for /INT input


}

const unsigned long transmissionInterval = 1;
unsigned long lastTransmissionTime = 0;

void loop() {
  // Check if it's time to send data
  if (millis() - lastTransmissionTime >= transmissionInterval) {
    // Define the sequence of data buses to send
    byte* dataBuses[] = {rpm, CoolantTemp, vspeed, IATSensor, MAFSensor, AmbientAirTemp, EngineOilTemp, EngineTorque, FuelTank, ThrottlePos};
    //byte* dataBuses[] = {rpm, EngineOilTemp, FuelTank};

    for (int i = 0; i < 10; i++) {
      // Send the selected data bus
      CAN0.sendMsgBuf(0x7DF, 0, 8, dataBuses[i]);
      updateCanBus(dataBuses[i], i); // Pass the data bus and index to the updateCanBus function
    }

    // Update the last transmission time
    lastTransmissionTime = millis();
  }

  // Other code in the loop can continue running...
}

void updateCanBus(byte* dataBus, int index) {
  if (!digitalRead(CAN0_INT)) // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf); // Read data: len = data length, buf = data byte(s)
   
    if ((rxId & 0x80000000) == 0x80000000) // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
    Serial.print(msgString);

    if ((rxId & 0x40000000) == 0x40000000) { // Determine if the message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
      for (byte i = 0; i < len; i++) {
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(msgString);
      }
    }
    processCanBusData(rxBuf, dataBus, index); // Pass the data bus and index to the processCanBusData function
  }
}

void processCanBusData(unsigned char CanResponse[8], byte* dataBus, int i) {
  byte PIDVALUE[] = {CanResponse[2], CanResponse[3]};

  //delay(1000);
  if (CanResponse[2] == 0x0C) {
    long RPM_Calculated = (CanResponse[3]*256L + CanResponse[4])/4;
    Serial.print(" -Engine Speed: ");
    Serial.println(RPM_Calculated);
    byte PIDVALUE_RPM[] = {CanResponse[2], RPM_Calculated};
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(PIDVALUE_RPM, 2);
    Wire.endTransmission();
    delay(20);
  
   }
  if (CanResponse[2] == 0x10) {
    long MAF_Calculated = (CanResponse[3]*256L + CanResponse[4])/100;
    Serial.print(" -Mass Air Flow: ");
    Serial.println(MAF_Calculated);
    byte PIDVALUE_MAF[] = {CanResponse[2], MAF_Calculated};
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(PIDVALUE_MAF, 2);
    Wire.endTransmission();
    delay(20);
     }
  if (CanResponse[2] == 0x63) {
    long TORQUE_Calculated = (CanResponse[3]*256L + CanResponse[4]);
    Serial.print(" -Engine Torque: ");
    Serial.println(TORQUE_Calculated);
    byte PIDVALUE_TORQUE[] = {CanResponse[2], TORQUE_Calculated};
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(PIDVALUE_TORQUE, 2);
    Wire.endTransmission();
    delay(20);
  }
  if (CanResponse[2] == 0x0D) {
    Serial.print(" -Vehicle speed: ");
    Serial.println(CanResponse[3]);
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(PIDVALUE, 2);
    Wire.endTransmission();
    delay(20);
  }
  if (CanResponse[2] == 0x05) {
    Serial.print(" -Coolant Temperature: ");
    Serial.println(CanResponse[3]);
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(PIDVALUE, 2);
    Wire.endTransmission();
    delay(20);
  }

  if (CanResponse[2] == 0x0F) {
    Serial.print(" -Intake Air Temperature: ");
    Serial.println(CanResponse[3]);
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(PIDVALUE, 2);
    Wire.endTransmission();
    delay(20);
    }

  if (CanResponse[2] == 0x46) {
    Serial.print(" -Ambient Air Temperature: ");
    Serial.println(CanResponse[3]);
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(PIDVALUE, 2);
    Wire.endTransmission();
    delay(20);
    }
  if (CanResponse[2] == 0x11) {
    long ThrottlePos_Calculated = (CanResponse[3]*100/255);
    Serial.print(" -Throttle Position: ");
    Serial.println(ThrottlePos_Calculated);
    byte PIDVALUE_ThrottlePos[] = {CanResponse[2], ThrottlePos_Calculated};
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(PIDVALUE_ThrottlePos, 2);
    Wire.endTransmission();
    delay(20);
  }

  if (CanResponse[2] == 0x2F) {
    Serial.print(" -Fuel Tank Level: ");
    Serial.println(CanResponse[3]);
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(PIDVALUE, 2);
    Wire.endTransmission();
    delay(20);
  }
  if (CanResponse[2] == 0x5C) {
    Serial.print(" -Engine Oil Temperature: ");
    Serial.println(CanResponse[3]);
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(PIDVALUE, 2);
    Wire.endTransmission();
    delay(20);
  }


}