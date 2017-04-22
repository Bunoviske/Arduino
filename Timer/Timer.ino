void setup() 
{

  pinMode(13, OUTPUT);    
  Serial.begin(9600);

}
void loop()
{
  // Main code loop
  // TODO: Put your regular (non-ISR) logic here
  
  unsigned long antes = millis();

  double i = 400000;
  while (i != 0 ){
    //Serial.println(i);
    i--;
  }
  unsigned long var = (millis() - antes);
  Serial.println(var);
  digitalWrite(13, digitalRead(13) ^ 1);
}

