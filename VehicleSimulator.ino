    #include <SPI.h>
    #include <mcp_can.h>
  
    #define CAN_INT 2 // Set INT to pin 2
    MCP_CAN CAN(10); // Set CS to pin 10



    INT32U canId = 0x000;

    //2024   4   65   12   18   248   185   147
     
long timelast;

char rndCoolantTemp = random(0x0, 0xFF);  
char rndRPM1 = random(0x0, 0xFF);  
char rndRPM2 = random(0x0, 0xFF); 
char rndSpeed = random(0x0, 0xFF); 
char rndIAT = random(0x0, 0xFF);  
char rndMAF1 = random(0x0, 0xFF);  
char rndMAF2 = random(0x0, 0xFF);  
char rndAmbientAirTemp = random(0x0, 0xFF);  
char rndEngineOilTemp = random(0x00, 0xFF);  
char rndEngineTorque1 = random(0x0, 0xFF); 
char rndEngineTorque2 = random(0x0, 0xFF);  
char rndFuelTank = random(0x0, 0xFF);  
char rndThrottlePos = random(0x0, 0xFF);  


    long unsigned int rxID;
    unsigned char len = 0;
    unsigned char buf[8];
    char str[20];

    String BuildMessage="";
    int MSGIdentifier=0;

    void setup()
    {
        Serial.begin(9600);

   if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");

  CAN.setMode(MCP_NORMAL); // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN_INT, INPUT); // Configuring pin for /INT input

  Serial.println("MCP2515 Library Receive Example...");

    }


    void loop()
    {
       
 
 timelast=millis(); 
 rndCoolantTemp = random(0x0, 0xFF);
 rndRPM1 = random(0x0, 0xFF);
 rndRPM2 = random(0x0, 0xFF);
 rndSpeed = random(0x0, 0xFF);
 rndIAT = random(0x0, 0xFF);
 rndMAF1 = random(0x0, 0xFF);
 rndMAF2 = random(0x0, 0xFF);
 rndAmbientAirTemp = random(0x0, 0xFF);
 rndEngineOilTemp = random(0x00, 0xFF);
 rndEngineTorque1 = random(0x0, 0xFF);
 rndEngineTorque2 = random(0x0, 0xFF); 
 rndFuelTank = random(0x0, 0xFF);
rndThrottlePos = random(0x0, 0xFF); 
 
  // SENSORS
unsigned char CoolantTemp[8] =        {0x03, 0x41, 0x05, rndCoolantTemp, 0xAA, 0xAA, 0xAA, 0xAA}; //Pid 5
unsigned char rpm[8] =                {0x04, 0x41, 0x0C, rndRPM1, rndRPM2, 0xAA, 0xAA, 0xAA};         //Pid 12
unsigned char vspeed[8] =             {0x03, 0x41, 0x0D, rndSpeed, 0xAA, 0xAA, 0xAA, 0xAA};       //Pid 13
unsigned char IATSensor[8] =          {0x03, 0x41, 0x0F, rndIAT, 0xAA, 0xAA, 0xAA, 0xAA};         //Pid 15
unsigned char MAFSensor[8] =          {0x04, 0x41, 0x10, rndMAF1, rndMAF2, 0xAA, 0xAA, 0xAA};         //Pid 10
unsigned char AmbientAirTemp[8] =     {0x03, 0x41, 0x46, rndAmbientAirTemp, 0xAA, 0xAA, 0xAA, 0xAA}; //Pid 70
unsigned char EngineOilTemp[8] =      {0x03, 0x41, 0x5C, rndEngineOilTemp, 0xAA, 0xAA, 0xAA, 0xAA};  //Pid 92
unsigned char EngineTorque[8] =       {0x04, 0x41, 0x63, rndEngineTorque1, rndEngineTorque2, 0xAA, 0xAA, 0xAA};    //Pid 99
unsigned char FuelTank[8] =           {0x03, 0x41, 0x2F, rndFuelTank, 0xAA, 0xAA, 0xAA, 0xAA};            //Pid 47
unsigned char ThrottlePos[8]=         {0x03, 0x41, 0x11, rndThrottlePos, 0xAA, 0xAA, 0xAA, 0xAA}; //Pid 17


        if(!digitalRead(CAN_INT)) 
        {
          CAN.readMsgBuf(&rxID, &len, buf);
            canId = rxID;
            Serial.print("<");Serial.print(canId);Serial.print(",");
            for(int i = 0; i<len; i++)
            { 
              BuildMessage = BuildMessage + buf[i] + ",";

            }
            Serial.println(BuildMessage);
            if (BuildMessage == "2,1,5,170,170,170,170,170,") {CAN.sendMsgBuf(0x7E8, 0, 8, CoolantTemp);}
            if (BuildMessage == "2,1,12,170,170,170,170,170,") {CAN.sendMsgBuf(0x7E8, 0, 8, rpm);}
            if (BuildMessage == "2,1,13,170,170,170,170,170,") {CAN.sendMsgBuf(0x7E8, 0, 8, vspeed);}
            if (BuildMessage == "2,1,15,170,170,170,170,170,") {CAN.sendMsgBuf(0x7E8, 0, 8, IATSensor);}
            if (BuildMessage == "2,1,16,170,170,170,170,170,") {CAN.sendMsgBuf(0x7E8, 0, 8, MAFSensor);}
            if (BuildMessage == "2,1,70,170,170,170,170,170,") {CAN.sendMsgBuf(0x7E8, 0, 8, AmbientAirTemp);}
            if (BuildMessage == "2,1,92,170,170,170,170,170,") {CAN.sendMsgBuf(0x7E8, 0, 8, EngineOilTemp);}
            if (BuildMessage == "2,1,99,170,170,170,170,170,") {CAN.sendMsgBuf(0x7E8, 0, 8, EngineTorque);}
            if (BuildMessage == "2,1,47,170,170,170,170,170,") {CAN.sendMsgBuf(0x7E8, 0, 8, FuelTank);}
            if (BuildMessage == "2,1,17,170,170,170,170,170,") {CAN.sendMsgBuf(0x7E8, 0, 8, ThrottlePos);}
            BuildMessage="";

           

        }
       

    }


