#define AIN1 7
#define AIN2 8
#define PWM 3
#define BIN1 4
#define BIN2 5


void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT); 


  analogWrite(PWM, 100);        // Speed (0–255)

  digitalWrite(AIN1,HIGH);      // Set A direction
  digitalWrite(AIN2, LOW);
  
  digitalWrite(BIN1, LOW);      // Set B direction
  digitalWrite(BIN2, HIGH);
}

void loop() {
  // Motor spins constantly

}