#include <ESP8266WiFi.h>        // Wifi Module
#include <BlynkSimpleEsp8266.h> // Blynk for internet-based connection
#include <SoftwareSerial.h>     // Serial Communication package


// Serial Communication port setting
SoftwareSerial softSerial(13, 12);

// States of systems
#define AUTO 0
#define MANUAL 1

////// Virtual Ports for Blynk
//////////////////////////////////////
// V0 -> Manual/ Auto button        //
// V1 -> Open/Close of valve button //
// V2 -> Threshold of moisture      //
// V3 -> Valve state LED            //
// V4 -> Timer Display              //
// V5 -> Water level Display        //
// V6 -> Soil Moisture Display      //
//////////////////////////////////////

  
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "G3k5NqlDZoTHM0f8cFcn0VQzkQOMmQ-U";
// Your WiFi credentials.
char ssid[] = "Mobinnet";
char pass[] = "";

// defines pins numbers for ultrasonic
const int trigPin = 5;
const int echoPin = 16;

// defines variables
BlynkTimer timer;
float distance;
int state = 0;
int valve_state = 0;
float moist_thr = 500;
int soil_moisture = 0;

BLYNK_CONNECTED() {
   Blynk.notify("System turned on");
   Blynk.syncAll();
}

BLYNK_WRITE(V0)
{
    state = param.asInt(); // assigning incoming value from pin V0 to a variable
    Serial.print("Change state to: ");
    Serial.println(state);
    if (state == AUTO ) {
       Blynk.notify("System now on Auto mode!");
    } else {
       Blynk.notify("System now on Manual mode!");
    }
}

BLYNK_WRITE(V1)
{
    int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
    Serial.print("Click on close/open button: ");
    if (state == AUTO) {
        Blynk.notify("System on Auto mode, you cannot open/close valve!");
    } else {
        valve_state = pinValue;
        if (valve_state == 0) {
          Blynk.notify("Valve is closed.");
        } else {
          Blynk.notify("Valve is opened.");
        }
    }
}


BLYNK_WRITE(V2)
{
    moist_thr = param.asDouble(); // assigning incoming value from pin V2 to a variable
    Serial.print("Moisture threshold has been changed to: ");
    Serial.println(moist_thr);
}


void setup() {
    Serial.begin(9600);       // Start the serial communication
    softSerial.begin(9600);   // Start connection with Arduino UNO
    pinMode(trigPin, OUTPUT); // Set the trigPin as an Output
    pinMode(echoPin, INPUT);  // Set the echoPin as an Input
    blynk_start_connection();
}

void loop() {
    Blynk.run();
    timer.run();
    distance = (float) ((int) (cal_distance() * 10)) / 10; // Calculate distance to water
    soil_moisture = analogRead(A0);                        // read from analog pin A3
    /// Checking whether we are in auto or manual state.
    if (state == AUTO) {
        if (soil_moisture > moist_thr)  // Turning valve on, if soil_moisture is above than threshold
            valve_state = 1;
        else
            valve_state = 0;         
    }
    /// Monitoring values in terminal
    print_stats();  
    delay(500);
}


void print_stats () {
    Serial.print("soil: ");
    Serial.println(soil_moisture);
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.print("  Valve_state: ");
    Serial.println(valve_state); 
}


void blynk_start_connection () {
    Blynk.begin(auth, ssid, pass); // Connect to wifi with ssid, pass. After that, connect to client with auth.
    Blynk.connect();
    timer.setInterval(1000L, update_client); //  Here you set interval (1sec) and which function to call, We frequently update the state of client.
}


void update_client() {
    Blynk.virtualWrite(V5, distance);       // Write Water level on blynk  
    Blynk.virtualWrite(V6, soil_moisture);  // Write Soil Moisture value on blynk  
    if(valve_state == 0) {
          Blynk.virtualWrite(V3, 0);        // Turning off LCD in Blynk
          softSerial.write('0');            // Send a code to Arduino UNO board
      } else {
          Blynk.virtualWrite(V3, 255);      // Turning on LCD in Blynk
          softSerial.write('1');            // Send a code to Arduino UNO board
    }
}


float cal_distance() {
    long duration;
    //Clear the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Set the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Read the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034/2;
    return distance;
}
