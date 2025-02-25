#include "Wire.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#define ONE_WIRE_BUS_1 13
OneWire ourWire1(ONE_WIRE_BUS_1);
DallasTemperature sensor1(&ourWire1);

float RawValue =0;
void setup(){

delay(1000);
  Serial.begin(115200);
  sensor1.begin();
  sensor1.setResolution(11);

}
void loop(){
  sensor1.requestTemperatures();
  float RawValue = sensor1.getTempCByIndex(0);
  Serial.print("Sens. 1 ");
  Serial.println(RawValue, 1);
}