#include <Arduino.h>
#include <Credentials.h>
#include <Wire.h>
#include <Adafruit_BNO08x.h>
#ifdef ATWINC
#include <WiFi101.h>
#elif ESP32
#include <WiFi.h>
#endif
#include <PubSubClient.h>
#include <ArduinoJson.h>


const char* MQTT_ID = "ESP32PCB";
const int myID = 1;
const char* DATATOPICNAME = "data/accel1";

// #ifdef ATWINC
// const char* MQTT_ID = "ATWINC";
// const int myID = 1;
// const char* DATATOPICNAME = "data/accel1";
// #elif ESP32
// const char* MQTT_ID = "ESP32";
// const int myID = 2;
// const char* DATATOPICNAME = "data/accel2";
// #endif

// ********************
// **** SET UP BNO ****
// ********************
// for SPI mode, we need a CS pin
#define BNO08X_CS 10
#define BNO08X_INT 9

// For SPI mode, we also need a RESET 
//#define BNO08X_RESET 5
// but not for I2C or UART
#define BNO08X_RESET -1

Adafruit_BNO08x  bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;


// ****************************************
// ********** SETUP WI-FI *****************
// Replace the next variables with your SSID/Password combination
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Add your MQTT Broker IP address, example:
const char* mqtt_server = MQTT_SERVER;

WiFiClient espClient;

// *************************************
// ********* SETUP MQTT ****************
// *************************************
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const int message_delay = 10;

StaticJsonDocument<220> doc_data;
StaticJsonDocument<220> doc_md;
char JSONmessageBuffer[220];

byte rangeCode = 0;

const int CS = 12;
String data = "";

bool inCallback = false;


// ************************************
// ******  HELPER FUNCTIONS   *********
// ************************************
void sendMetaData();
void setRange(int numRange);

//Wifi based
void reconnect();
void setup_wifi();
void callback(char* topic, byte* message, unsigned int length);

void setReports();

void setup()
{

  // Configure pins for Adafruit ATWINC1500 Feather
  #ifdef ATWINC
  WiFi.setPins(8, 7, 4, 2);
  #endif
  Serial.begin(9600);

  setup_wifi();
  delay(100);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Try to initialize!
  if (!bno08x.begin_I2C()) {
  //if (!bno08x.begin_UART(&Serial1)) {  // Requires a device with > 300 byte UART buffer!
  //if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
    Serial.println("Failed to find BNO08x chip");
    while (1) { delay(10); }
  }
  Serial.println("BNO08x Found!");

  setReports();

  Serial.print("Sensor is connected properly.");
  Serial.println();
  Serial.print("topic name is ");
  Serial.println(DATATOPICNAME);
}

void setup_wifi(){
    delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  #ifdef ATWINC
  WiFi.setPins(8, 7, 4, 2);
  #endif

  WiFi.begin(ssid, password);
  //WiFi.getTxPower();
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
      if(i > 20){
          i = 0;
          WiFi.begin(ssid,password);
      }
    delay(500);
    Serial.print(".");
    i++;
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
    inCallback = true;
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String newMessage;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    newMessage += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "queryDevice") {
      Serial.println("got query item, sending meta data");
     sendMetaData(); 
  } else if(String(topic) == "adxl313/range"){
      setRange(newMessage[0]);
  }
  inCallback = false;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // check wifi status first
    if(WiFi.status() != WL_CONNECTED){
        Serial.println("Wifi is not connected, attempting WiFi connection");
        setup_wifi();
    }
    if (client.connect(MQTT_ID)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("adxl313/range");
      client.subscribe("queryDevice");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop()
{
    if(!client.connected()){
        Serial.println("trying to connect");
        reconnect();
    }

    if(bno08x.wasReset()){
        Serial.print("sensor was reset");
        setReports();
    }

    unsigned long now = millis();
    if(now - lastMsg > message_delay){
        lastMsg = now;
        // Serial.println("reading accel");
        if(bno08x.getSensorEvent(&sensorValue)) // check data ready interrupt, note, this clears all other int bits in INT_SOURCE reg
        {
            // Serial.print("x: ");
            // Serial.print(myAdxl.x);
            // Serial.print("\ty: ");
            // Serial.print(myAdxl.y);
            // Serial.print("\tz: ");
            // Serial.print(myAdxl.z);
            // Serial.println();
            doc_data["accelID"] = myID;
            doc_data["time"] = now;
            doc_data["xval"] = sensorValue.un.linearAcceleration.x;
            doc_data["yval"] = sensorValue.un.linearAcceleration.y;
            doc_data["zval"] = sensorValue.un.linearAcceleration.z;
            doc_data["rssi"] = WiFi.RSSI();
            serializeJson(doc_data, JSONmessageBuffer);
            client.publish(DATATOPICNAME,JSONmessageBuffer);
            client.loop();
        }
        else
        {
            Serial.println("Waiting for dataReady.");
        }  

    }
//   delay(50);
}
void sendMetaData(){
    // doc_md["output_data_rate"] = myAdxl.getRate();
    // doc_md["clock_speed"] = spiClk;
    // doc_md["device_id"] = readRegister(0x00);
    // doc_md["range"] = myAdxl.getRange();
    // Serial.print("range: ");
    // Serial.println(myAdxl.getRange());
    // myAdxl.printAllRegister();
    // doc_md["measure_mode"]=readRegister(REGISTER_MEASURE_MODE);
    doc_md["sample_time"]= message_delay;
    #ifdef ATWINC
        doc_md["device_name"] = "ATWINC";
    #elif ESP32
        doc_md["device_name"] = "ESP32";
    #endif
    doc_md["accelID"] = myID;

    serializeJson(doc_md, JSONmessageBuffer);
    client.publish("accel/metadata",JSONmessageBuffer);
}
void setRange(int numRange){
    // sets range to +/- 4g
    Serial.println(numRange);
    switch(numRange){
        // case 48: myAdxl.setRange(ADXL313_RANGE_05_G); break;
        // case 49: myAdxl.setRange(ADXL313_RANGE_1_G); break;
        // case 50: myAdxl.setRange(ADXL313_RANGE_2_G); break;
        // case 52: myAdxl.setRange(ADXL313_RANGE_4_G); break;
        default: return;
    }
}

// Here is where you define the sensor outputs you want to receive
void setReports(void) {
  Serial.println("Setting desired reports");
  if (! bno08x.enableReport(SH2_LINEAR_ACCELERATION)) {
    Serial.println("Could not enable linear acceleration");
  }
}