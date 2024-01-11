#include <Wire.h>
#include <ArduinoBLE.h>

#define SLAVE_ADDR 9
#define MAX_ARRAY_SIZE 2

int initialValue = 0;

byte receivedBytes[MAX_ARRAY_SIZE];
byte dataToSend2[2];
int byteCount = 0;



BLEService OBD2Service("12345678-90AB-CDEF-0000-0123456789AB");
BLECharCharacteristic CoolantTempChar("39BB4243-7397-4CA7-82B5-DF28B670E005", BLERead | BLENotify);
BLECharCharacteristic rpmChar("39BB4243-7397-4CA7-82B5-DF28B670E012", BLERead | BLENotify);
BLECharCharacteristic vspeedChar("39BB4243-7397-4CA7-82B5-DF28B670E013", BLERead | BLENotify);
BLECharCharacteristic IATSensorChar("39BB4243-7397-4CA7-82B5-DF28B670E015", BLERead | BLENotify);
BLECharCharacteristic MAFSensorChar("9BB4243-7397-4CA7-82B5-DF28B670E080", BLERead | BLENotify);
BLECharCharacteristic AmbientAirTempChar("39BB4243-7397-4CA7-82B5-DF28B670E070", BLERead | BLENotify);
BLECharCharacteristic EngineOilTempChar("39BB4243-7397-4CA7-82B5-DF28B670E092", BLERead | BLENotify);
BLECharCharacteristic EngineTorqueChar("39BB4243-7397-4CA7-82B5-DF28B670E099", BLERead | BLENotify);
BLECharCharacteristic FuelTankChar("39BB4243-7397-4CA7-82B5-DF28B670E047", BLERead | BLENotify);
BLECharCharacteristic ThrottlePosChar("39BB4243-7397-4CA7-82B5-DF28B670E017", BLERead | BLENotify);

void setup() {
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("OBD2");
  BLE.setAdvertisedService(OBD2Service);
  OBD2Service.addCharacteristic(CoolantTempChar);
  OBD2Service.addCharacteristic(rpmChar);
  OBD2Service.addCharacteristic(vspeedChar);
  OBD2Service.addCharacteristic(IATSensorChar);
  OBD2Service.addCharacteristic(MAFSensorChar);
  OBD2Service.addCharacteristic(AmbientAirTempChar);
  OBD2Service.addCharacteristic(EngineOilTempChar);
  OBD2Service.addCharacteristic(EngineTorqueChar);
  OBD2Service.addCharacteristic(FuelTankChar);
  OBD2Service.addCharacteristic(ThrottlePosChar);
  BLE.addService(OBD2Service);



  CoolantTempChar.writeValue(initialValue);
  rpmChar.writeValue(initialValue);
  vspeedChar.writeValue(initialValue);
  IATSensorChar.writeValue(initialValue);
  MAFSensorChar.writeValue(initialValue);
  AmbientAirTempChar.writeValue(initialValue);
  EngineOilTempChar.writeValue(initialValue);
  EngineTorqueChar.writeValue(initialValue);
  FuelTankChar.writeValue(initialValue);
  ThrottlePosChar.writeValue(initialValue);


  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");

  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    unsigned long previousMillis = 0;
    const long interval = 1;  // Interval in milliseconds (1 second)

    while (central.connected()) {
      // Check if it's time to send data
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // Continuously send I2C data over BLE
        sendI2CDataOverBLE();
      }
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}


void receiveEvent(int howMany) {
  byteCount = 0;
  while (Wire.available() > 0 && byteCount < MAX_ARRAY_SIZE) {
    receivedBytes[byteCount] = Wire.read();
    byteCount++;
  }
}

void sendI2CDataOverBLE() {
  // Check if new I2C data is available
  if (receivedBytes[0] == 0x05) {
        CoolantTempChar.writeValue(receivedBytes[1]);
        Serial.print("Coolant Temperature: ");
        Serial.println(receivedBytes[1]);
  }
  if (receivedBytes[0] == 0x0D) {
        vspeedChar.writeValue(receivedBytes[1]);
        Serial.print("Vehicle speed: ");
        Serial.println(receivedBytes[1]);
  }
  
      if (receivedBytes[0] == 0x0C) {
        rpmChar.writeValue(receivedBytes[1]);
        Serial.print("RPM: ");
        Serial.println(receivedBytes[1]);
      }

      if (receivedBytes[0] == 0x0F) {
        IATSensorChar.writeValue(receivedBytes[1]);
        Serial.print("Intake Air Temperature: ");
        Serial.println(receivedBytes[1]);
      }

      if (receivedBytes[0] == 0x10) {
        MAFSensorChar.writeValue(receivedBytes[1]);
        Serial.print("Mass Air Flow: ");
        Serial.println(receivedBytes[1]);
      }

      if (receivedBytes[0] == 0x46) {
        AmbientAirTempChar.writeValue(receivedBytes[1]);
        Serial.print("Ambient Air Temperature: ");
        Serial.println(receivedBytes[1]);
      }

    if (receivedBytes[0] == 0x5C) {
        EngineOilTempChar.writeValue(receivedBytes[1]);
        Serial.print("Engine Oil Temperature: ");
        Serial.println(receivedBytes[1]);
    }
    if (receivedBytes[0] == 0x63) {
        EngineTorqueChar.writeValue(receivedBytes[1]);
        Serial.print("Engine Torque: ");
        Serial.println(receivedBytes[1]);
    }
    if (receivedBytes[0] == 0x2F) {
        FuelTankChar.writeValue(receivedBytes[1]);
        Serial.print("Fuel Tank Level: ");
        Serial.println(receivedBytes[1]);
    }
    if (receivedBytes[0] == 0x11) {
      ThrottlePosChar.writeValue(receivedBytes[1]);
      Serial.print("Throttle Position: ");
      Serial.println(receivedBytes[1]);
    }

    // Reset byteCount for the next set of data
    byteCount = 0;
  }




