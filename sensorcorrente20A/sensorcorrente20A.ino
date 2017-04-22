

int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
int RawValue= 0;
int ACSoffset = 2500; 
double Voltage = 0;
double Amps = 0;


void setup(){ 
   Serial.begin(9600);
}

void loop(){
   
   RawValue = analogRead(A0);
   Serial.println(RawValue);
   Voltage = (RawValue / 1023.0) * 5000; // Gets you mV
   Amps = ((Voltage - ACSoffset) / mVperAmp);
   float pot = 220*Amps; 
   Serial.print("\t mV = "); // shows the voltage measured 
   Serial.print(pot,3); // the '3' after voltage allows you to display 3 digits after decimal point
   Serial.print("\t Amps = "); // shows the voltage measured 
   Serial.println(Amps,3); // the '3' after voltage allows you to display 3 digits after decimal point
   //digitalWrite(rele,1);
   //delay(5000);
   //digitalWrite(rele,0);
   delay(5000);
 
}
