#include <Servo.h>
#include <IRremote.h> 

// Define each button for the remote 
#define Decrease 0xFF22DD // Rewind button
#define Increase 0xFFC23D // Fast forward button

// Set pins and declare variables for the motion sensor 
const int trigPin = 9;
const int echoPin = 10;
long duration;
int distance;

// Set up pin number for the red LED indication bulb
const int BULB = 11;

// Set up for the rotating motor
Servo servo;
int angle = 0;      // Starting angle of the servo motor
int motorSpeed = 20; // Delay between the motor's angle change (smaller value = faster rotation)

// Set up the remote and receiver
const int receiver = 13; // Initialize pin 13 as recevier pin.
uint32_t Previous;       // Stores the last button pressed, useful for when the button is held down
IRrecv irrecv(receiver); // Instance of receiver
decode_results results;


void setup() {

  Serial.begin(9600);
  
  // Initializing the ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initializing the red bulb
  pinMode(BULB, OUTPUT);

  // Initializing the rotating motor
  servo.attach(8);
  servo.write(angle);

  // Intializing the receiver for the remote
  irrecv.enableIRIn(); 

}

void loop() {
  
  // The motor is rotating and looking for objects, and it will stop when an object is detected
    
  // Scan from 10 to 180 degrees
  for(angle = 10; angle < 180; angle++){
    
    Serial.print("scanning\n");
    distance = DistanceCalculation(trigPin, echoPin); // Calculate distance of nearest object
    if (distance <= 15){
      // An Object was detected
      while(distance <= 15){
        // This loop will continue as long as the object does NOT move
        distance = DistanceCalculation(trigPin, echoPin); 
        BlinkBulb(BULB); // Blink the bulb
        }
      }
    
    servo.write(angle);       // Sets the motor at a new angle
    SignalCheck(&motorSpeed); // Changes the speed of the motor if if a specific button is pressed
    delay(motorSpeed);        // Applies the speed of the motor
    }

  // Scan back from 180 to 10 degrees
  for(angle = 180; angle > 10; angle--){
        
    distance = DistanceCalculation(trigPin, echoPin); // Calculate distance of nearest object
    Serial.print("scanning\n");
    // An Object was detected
    if (distance <= 15){
        // This loop will continue as long as the object does NOT move
        while(distance <= 15){
          distance = DistanceCalculation(trigPin, echoPin); 
          BlinkBulb(BULB); // Blink the bulb
          }
       }
   
    servo.write(angle);       // Sets the motor at a new angle
    SignalCheck(&motorSpeed); // Changes the speed of the motor if if a specific button is pressed
    delay(motorSpeed);        // Applies the speed of the motor
    }

   // End of the loop, the motor is back where it started at and has completed a whole cycle
}



// Functions used inside of the program

int DistanceCalculation(int trigpin, int echopin){
  // This function is used to calculate the distance of the nearest object when it is called

  // I believe that these next two lines are not needed
  //digitalWrite(trigPin,LOW);
  //delayMicroseconds(2);

  // Sending an ultrasonic wave
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Calculate distance and return the value
  long duration;
  duration = pulseIn(echoPin, HIGH);
  return duration*0.034/2;
  
}

void BlinkBulb(int BULB){
  // Simply used to blink the red bulb and it is called when an object is detected

  Serial.print("DETECTION!\n");
  digitalWrite(BULB, HIGH);
  delay(100);
  digitalWrite(BULB, LOW);
  delay(100);
}

void SignalCheck(int *motorSpeed){
  // This function checks to see if a button was pressed and will change the speed of the motor if needed
  
  if (irrecv.decode(&results)) { 
    // IR signal was received
    
    if (results.value==0xFFFFFFFF) { 
      // This means that the signal is repeating
        results.value=Previous;
        }
    // This is where we change the rotation speed if a specific button is clicked 
    switch(results.value) {
           case Decrease : *motorSpeed = *motorSpeed + 2;  break;
           case Increase : *motorSpeed = *motorSpeed - 2;  break;
           
      } 
 
 //Serial.println (*motorSpeed); //display HEX results 
 irrecv.resume(); //next value
    }
 Previous=results.value; // Value stored for future reference
 }
