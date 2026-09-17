#include <PID_v1.h>

// Pin Definitions
#define PWM_PIN           9   
#define HALL_SENSOR_PIN   4   

#define HALL_PPR        70.0  
const unsigned long LOOP_DELAY_MS = 50; 

// PID Parameters
double Kp = 0.1; // proportinal gain
double Ki = 0.1; // integral gain
double Kd = 0.00; // Not using Kd, hall sensor is too noisy and so Kd jumps alot

// PID Variables
double currentRPM = 0.0; 
double Output = 0.0;
double targetRPM = 800.0; 

// Initialize PID
PID myPID(&currentRPM, &Output, &targetRPM, Kp, Ki, Kd, DIRECT);

// RPM measurement variables
volatile long pulseCount = 0;
unsigned long prevTime = 0; 
unsigned long lastPIDTime = 0;

void pulseCounter() {
  pulseCount++;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("--- Stirring Started ---");
  delay(100);

  pinMode(PWM_PIN, OUTPUT);
  pinMode(HALL_SENSOR_PIN, INPUT_PULLUP); // Input pullup to prevent floating signals
  
  attachInterrupt(digitalPinToInterrupt(HALL_SENSOR_PIN), pulseCounter, RISING);

  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 255); 
  myPID.SetSampleTime(LOOP_DELAY_MS); // Tell PID library how often we run
  
  prevTime = millis(); 
}

void loop()
{
  unsigned long currentTime = millis();

  // Calculate RPM
  if (currentTime - prevTime >= LOOP_DELAY_MS) {
    noInterrupts();
    long pulses = pulseCount;
    pulseCount = 0;
    interrupts();

    // RPM math
    currentRPM = ((double)pulses / HALL_PPR) * (60000.0 / (double)(currentTime - prevTime));

    prevTime = currentTime; // Reset calculation timer
    
    // Feed into PID library 
    myPID.Compute(); 

    // Send PWM signal to motor 
    analogWrite(PWM_PIN, (int)Output);

    // Debugging
    Serial.print("Target: "); Serial.print(targetRPM, 0);
    Serial.print(" | Actual: "); Serial.print(currentRPM, 0);
    Serial.print(" | PWM: "); Serial.println((int)Output);
  }
}