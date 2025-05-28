#define AIN1 11
#define AIN2 12
#define PWM 10
#define BIN1 7
#define BIN2 8


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

  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  delay(1000);

  digitalWrite(AIN1,LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, HIGH);

  delay(1000);

}