void setup() {
}

void loop() {
  PORTB = 0xff;
  delay(10);
  PORTB = 0x00;
  delay(10);
}
