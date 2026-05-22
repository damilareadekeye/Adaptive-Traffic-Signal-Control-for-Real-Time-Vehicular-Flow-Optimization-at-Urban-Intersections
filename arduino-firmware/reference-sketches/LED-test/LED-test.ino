
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(13, OUTPUT);  // - Green LED on console
  pinMode(11, OUTPUT);  // - Green - Lane 1
  pinMode(10, OUTPUT);  // - Yellow - Lane 1
  pinMode(9, OUTPUT);   // - Red - Lane 1
  pinMode(8, OUTPUT);   // - Yellow - Lane 2
  pinMode(7, OUTPUT);   // - Green - Lane 2
  pinMode(6, OUTPUT);   // - Red - Lane 2
  pinMode(5, OUTPUT);   // - Green - Lane 3
  pinMode(4, OUTPUT);   // - Green - Lane 3
  pinMode(3, OUTPUT);   // - Red - Lane 3
  pinMode(16, OUTPUT);  // - Red Lane 4
  pinMode(15, OUTPUT);  // - Green - Lane 4
  pinMode(14, OUTPUT);  // - Yellow - Lane 4
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(13, HIGH);  // turn the LED on (HIGH is the voltage level)
  digitalWrite(11, HIGH);  // - Green
  digitalWrite(10, HIGH);  // - Yellow
  digitalWrite(9, HIGH);   // - Red 
  digitalWrite(8, HIGH);   // - Yellow
  digitalWrite(7, HIGH);   // - Green
  digitalWrite(6, HIGH);   // - Red
  digitalWrite(5, HIGH);   // - Green
  digitalWrite(4, HIGH);   // - Yellow
  digitalWrite(3, HIGH);   // - Yellow
  digitalWrite(16, HIGH);  // - Red
  digitalWrite(15, HIGH);  // - Green
  digitalWrite(14, HIGH);  // - Yellow
  delay(1000);             // wait for a second
  digitalWrite(13, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(11, LOW);   // - Green
  digitalWrite(10, LOW);   // - Yellow
  digitalWrite(9, LOW);    // - Red 
  digitalWrite(8, LOW);    // - Yellow
  digitalWrite(7, LOW);    // - Green
  digitalWrite(6, LOW);    // - Red
  digitalWrite(5, LOW);    // - Green
  digitalWrite(4, LOW);    // - Yellow
  digitalWrite(3, LOW);    // - Yellow
  digitalWrite(16, LOW);   // - Red
  digitalWrite(15, LOW);   // - Green
  digitalWrite(14, LOW);   // - Yellow
  delay(1000);             // wait for a second
}
