#define TAG_NUMBER 6
uint8_t my_number = 5;

void setup() {
  // put your setup code here, to run once:
  pinMode(4, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  /*tone(8, 40000);
  delay(5);
  noTone(8);
  delay(20);*/
  while(!digitalRead(4)){}
  send_frame();
}

void send_frame()
{
  tone(8, 40000);
  delay(15);
  for(uint8_t i = 0; i < TAG_NUMBER; i++)
  {
    if(i == my_number) tone(8, 40000);
    else noTone(8);
    delay(5);
  }
  noTone(8);
  delay(10);
}
