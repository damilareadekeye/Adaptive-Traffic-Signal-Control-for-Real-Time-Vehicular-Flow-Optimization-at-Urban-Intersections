#include <HCSR04.h>

UltraSonicDistanceSensor distanceSensor1(23, 22);  // Lane 1, Ultrasonic I
UltraSonicDistanceSensor distanceSensor2(24, 25);  // Lane 1, Ultrasonic II
UltraSonicDistanceSensor distanceSensor3(26, 27);  // Lane 3, Ultrasonic II
UltraSonicDistanceSensor distanceSensor4(28, 29);  // Lane 4, Ultrasonic I
UltraSonicDistanceSensor distanceSensor5(30, 31);  // Lane 4, Ultrasonic II
UltraSonicDistanceSensor distanceSensor6(32, 33);  // Lane 3, Ultrasonic I
UltraSonicDistanceSensor distanceSensor7(34, 35);  // Lane 2, Ultrasonic II
UltraSonicDistanceSensor distanceSensor8(36, 37);  // Lane 2, Ultrasonic I


void setup() {
  Serial.begin(115200);  // We initialize serial connection so that we could print values from sensor.
}

void loop() {
  // Every 500 miliseconds, do a measurement using the sensor and print the distance in centimeters.
  Serial.print("Lane 1, Ultrasonic I: ");
  Serial.println(distanceSensor1.measureDistanceCm());  // Lane 1, Ultrasonic I
  Serial.print("Lane 1, Ultrasonic II: ");
  Serial.println(distanceSensor2.measureDistanceCm());  // Lane 1, Ultrasonic II
  Serial.print("Lane 2, Ultrasonic I: ");
  Serial.println(distanceSensor8.measureDistanceCm());  // Lane 2, Ultrasonic I
  Serial.print("Lane 2, Ultrasonic II: ");
  Serial.println(distanceSensor7.measureDistanceCm());  // Lane 2, Ultrasonic II
  Serial.print("Lane 3, Ultrasonic I: ");
  Serial.println(distanceSensor6.measureDistanceCm());  // Lane 3, Ultrasonic I
  Serial.print("Lane 3, Ultrasonic II: ");
  Serial.println(distanceSensor3.measureDistanceCm());  // Lane 3, Ultrasonic II
  Serial.print("Lane 4, Ultrasonic I: ");
  Serial.println(distanceSensor4.measureDistanceCm());  // Lane 4, Ultrasonic I
  Serial.print("Lane 4, Ultrasonic II: ");
  Serial.println(distanceSensor5.measureDistanceCm());  // Lane 4, Ultrasonic II

  Serial.println("---");

  delay(500);
}
