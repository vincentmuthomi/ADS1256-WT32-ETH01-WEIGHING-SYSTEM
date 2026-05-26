#include <ETH.h>
#include <WiFi.h>
#include <ModbusIP_ESP8266.h>
ModbusIP mb;
IPAddress local_IP(192, 168, 0, 88); 
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
const int REG_FLOAT = 0;
float value = 0.01;
float value2 = 1000.0;

void writeFloatToHreg(int reg, float val, float val2) {
  union {
    float f;
    uint16_t w[2];
  } data;
  
  union {
    float f;
    uint16_t w[2];
  } data2;

  data.f = val;
  data2.f = val2;
  mb.Hreg(reg + 0, data.w[1]);  
  Serial.print("HIGH W1: ");
  Serial.print(data.w[0]);
  Serial.print(" - HIGH W2 ");
  Serial.print(data2.w[0]);
  Serial.print(" -||- ");

  mb.Hreg(reg + 1, data.w[0]);
  Serial.print("LOW W1: ");
  Serial.print(data.w[1]);
  Serial.print(" - LOW W2: ");

  mb.Hreg(reg + 2, data2.w[1]);
  mb.Hreg(reg + 3, data2.w[0]);
  Serial.print(data2.w[1]);
  Serial.print(" -||- ");
  Serial.print("ACTUAL VAL1: ");
  Serial.print(val);
  Serial.print(" - VAL2: ");
  Serial.print(val2);
  Serial.println(" ");
}

void setup() {
  Serial.begin(115200);
  ETH.begin();
  if (!ETH.config(local_IP, gateway, subnet)) {
    Serial.println("Static IP configuration failed!");
  }
  while (!ETH.linkUp()) {
    delay(100);
  }

  Serial.print("Static IP: ");
  Serial.println(ETH.localIP());
  mb.server();

  mb.addHreg(REG_FLOAT, 0);
  mb.addHreg(REG_FLOAT + 1, 0);
  mb.addHreg(REG_FLOAT + 2, 0);
  mb.addHreg(REG_FLOAT + 3, 0);  
}

void loop() {
  value += 0.01;
   
  if (value > 1000.0) value = 0.01;
  value2 = 1000 - value;
  writeFloatToHreg(REG_FLOAT, value, value2);
  mb.task();
  delay(1000);
}
