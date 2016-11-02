const int led1_pin = 6;
const int led2_pin = 7;

void setup()
{
  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
}

void loop()
{
   digitalWrite(led1_pin, HIGH);
   digitalWrite(led2_pin, LOW);
   delay(500);
   digitalWrite(led1_pin, LOW);
   digitalWrite(led2_pin, HIGH);
   delay(500);
}

