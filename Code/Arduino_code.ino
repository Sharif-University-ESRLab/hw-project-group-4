#include <SoftwareSerial.h> 


// Off and On equivalent code from sender.
#define OFF 48
#define ON 49

/// set pins for serial communication
SoftwareSerial softSerial(A4, A5);

// Variables
int val = 0; // Temporary variable for reading input
const int relay_port =  3;

void setup() {
   pinMode(A0, INPUT);
   pinMode(relay_port, OUTPUT);
   Serial.begin(9600);
   softSerial.begin(9600); //Set Soft Serial communication rate
}

void loop()
{    
    if (softSerial.available())  { // If signal from main board avaiable
         val = softSerial.read();
         if(val == OFF) { 
            digitalWrite(relay_port, LOW);    // Turn off relay
         } else if (val == ON) {
            digitalWrite(relay_port, HIGH);   // Turn on relay
         }
    }
    delay(200);
}  
