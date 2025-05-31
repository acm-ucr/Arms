#include <Servo.h>

Servo finger1;
Servo finger2;
Servo finger3;
Servo wrist;
Servo vexR;
Servo vexP;

// vex motor definitions i think
#define AIN1 11 // left bit for the pitch motor 10
#define AIN2 12 // right bit for the pitch motor 01
#define PWM 10
#define BIN1 7 // left bit for the rotation motor 10
#define BIN2 8 // right bit for the rotation motor 01

// 00 for slow-stop, 01 for clockwise, 10 for counter-clockwise, 11 for hardstop

void setup(){ // commented parts are there due to uncertainty (its scary)
    Serial.begin(9600);
    pinMode(3, INPUT);
    pinMode(5, INPUT); 

    //vex pinmodes
    pinMode(AIN1, INPUT);
    pinMode(AIN2, INPUT);
    pinMode(BIN1, INPUT);
    pinMode(BIN2, INPUT);

    pinMode(6, INPUT);
    pinMode(9, INPUT);
    pinMode(10, INPUT); // same for both a and b (i think)
    //pinMode(11, INPUT);
    finger1.attach(3);
    finger2.attach(5);
    finger3.attach(6);
    wrist.attach(9);
    vexR.attach(10);
    //vexP.attach(11);
    vexP.attach(10);

    pinMode(A4, INPUT); // Palm pressure sensor

    analogWrite(PWM, 50); //making sure the speed is always nothing when starting
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW); // same idea for the bits
    state = Idle;
    int averagePalmSensor = 0;
    int tickCounter = 0;
}

enum States { Idle, Seek, Approach, Grip, Lift, MoveToDrop, Release } state;
//PWM Pin 3,5,6,9,10,11
//For the vex motor, the arduino conects to the driver
//Pin D11 goes to PWMA and Pin D10 goes to PWMB
//D13 and D12 go to 
//STBY needs to be connected to high at all times.
// flags
unsigned char startSignal; // used
unsigned char obj_located;
unsigned char target_located;
bool object_reached;
bool pressure_reached[4];
bool reached_upright;
char buffer[5] = {0,0,0,0,0};
String bufferString;
int vexPTickCount = 0; // for the release state
int vexRTickCount = 0; // ^^^ clockwise increases the count while counter clockwise decreases the count
// flags: 0x00

unsigned char object_angle;
int target_angle;
double arm_angle = 90.0;
bool target_reached;
int curr_finger1_angle;
int curr_finger2_angle;
int curr_finger3_angle;

void Tick(){
    if(Serial.available() > 0){
          bufferString = Serial.readString(); //reads from raspberry pi
        }
    for(int i = 0; i < 5; i++){
        buffer[i] = bufferString[i];    //convert to char array
    }
    switch(state){  //state transitions
        case Idle:
            startSignal = buffer[0];    //get the first bit from buffer
            if(startSignal){                //if true, move to seek state
                state = Seek;
            }else state = Idle;
            startSignal = 0; //reset startSignal
            break;
        case Seek:
            obj_located = buffer[0];
            // target_located = buffer[1]; // not finding target now
            if (obj_located) {
              state = Approach;
              digitalWrite(BIN1, LOW);
            } else state = Seek;
            obj_located = 0;
            break;
        case Approach:
            object_reached = buffer[0];
            if (object_reached) {
                state = Grip;    
            } else state = Approach;
            object_reached = 0;
            break;
        case Grip: /* reach to this state when we reach to the object
                      keep checking if ALL 4 pressure sensors are fully pressed down, when done send to
                      lift state aka make pressure_reached = true
                      if not reach, continue incrementing 
                      [0/1, 0/1, 0/1, 0/1, X]
                   */
            int trueCounter = 0;
            for (unsigned i = 0; i < 4; ++i){
                if (buffer[i] == '1'){
                    pressure_reached[i] = true;
                    trueCounter++;
                }
            }
            if (trueCounter == 4){
                state = Lift;
            }
            
            break;
        case Lift: /* reach to this state when recieved pressure reached from all 4 pressure sensors
            keep checking the reached_up_right until we read that we are at 90
            when done, send the flag that we reached up right to MoveToDrop state
            buffer thats being read: [0/1, X, X, X, X] (X = not used) 
          */
            if (arm_angle == 90.0){
                reached_upright = true;
                state = MoveToDrop;
                digitalWrite(AIN1, LOW);
            }
            else{
                reached_upright = false;
                state = Lift;
            }
        case MoveToDrop: // should discuss if we are told if its cw or ccw so it can move faster
            if (buffer[1] == '1'){
                target_reached = true;
                state = Release;
                digitalWrite(AIN1, LOW);
                digitalWrite(AIN2, LOW);
            }
            else{
                target_reached = false;
            }
            break;
  // MAKE SURE TO NOT CLEAR THE BUFFER HERE WE NEED TO KEEP THE 0/1 FOR THE LIFT STATE
        case Release:
            if(buffer[0] == '1' && buffer[1] == 0 && buffer[2] == '0' && buffer[3] == '0' && buffer[4] == '0'){ //if we get a 1 from the serial, we start the 
                state = Idle; //move to idle state
            }
            else state = Release;
            break;
        default:
            break;
    }
    switch(state){  //state actions
        case Idle: // should do nothing, discuss this further if needed
            break;
        case Seek:
            digitalWrite(BIN1, HIGH); // rotating as told until we're done and stuff
            digitalWrite(BIN2, LOW);
            delay(25);
            vexRTickCount++;
            break;
        case Approach:
            int count = 0;
            if (buffer[1] == '1'){ // move the wrist left (with vex) -- rotation motor
                digitalWrite(BIN1, LOW); // counter-clockwise (edit if needed)
                digitalWrite(BIN2, HIGH);
                delay(25);
                digitalWrite(BIN2, LOW);
                vexPTickCount++;
            }
            else{
                count++;
            }

            if (buffer[2] == '1'){ // move the wrist right (with vex) -- rotation motor
                digitalWrite(BIN1, HIGH); // clockwise (edit if needed)
                digitalWrite(BIN2, LOW);
                delay(25);
                digitalWrite(BIN1, LOW);
                vexPTickCount++;
            } else{
                count++;
            }

            if (buffer[3] == '1'){ // move the wrist up (with servo)
                wrist.write(85);
                delay(25);
                wrist.write(90);
            } else{
                count++;
            }

            if (buffer[4] == '1'){ // move the wrist down (with servo)
                wrist.write(95);
                delay(25);
                wrist.write(90);
            } else{
                count++;
            }
            
            if (count == 4){ // the object is centered in the camera  -- pitch motor
                digitalWrite(AIN1, LOW); // counter-clockwise, HEAVILY CHECK THIS
                digitalWrite(AIN2, HIGH); 
                delay(25);
                digitalWrite(AIN2, LOW);
                vexRTickCount--;
            }

            if (tickCounter != 5){
                averagePalmSensor += analogRead();
                tickCounter++;
            }
            else {
                averagePalmSensor = averagePalmSensor / tickCounter;
                Serial.println(averagePalmSensor);
            }

            break;
        case Grip:
            curr_finger1_angle = finger1.read();
            curr_finger2_angle = finger2.read();
            curr_finger3_angle = finger3.read();
            // might have to change direction later (+/-)
            finger1.write(curr_finger1_angle + 1);
            finger2.write(curr_finger2_angle + 1);
            finger3.write(curr_finger3_angle + 1);
            break;
        case Lift:
            digitalWrite(AIN1, HIGH); // clockwise, keep opposite of approach (205-210 as i write this) -- pitch motor
            digitalWrite(AIN1, LOW);
            vexRTickCount++;
            break;
        case MoveToDrop:
            //potential code based off of if we are told the direction
            if (direction == 1){ // clockwise -- rotation motor
                digitalWrite(BIN1, HIGH);
                digitalWrite(BIN2, LOW);
                delay(25);
                vexRTickCount++;
            }
            else{
                digitalWrite(BIN1, LOW); // counter-clockwise -- rotation motor
                digitalWrite(BIN2, HIGH);
                delay(25);
                vexRTickCount--;
            }
            // code that will 100% work with what logic we already have
            digitalWrite(BIN1, HIGH); // clockwise -- rotation motor
            digitalWrite(BIN2, LOW);
            delay(25);
            vexRTickCount++;
            break;
        case Release: // first release the fingers back to a certain degree (probably starting), then change the rotation and pitch of the original position dictated by the count variables
                      // then fix the wrist orientation
                      // original idea is that it iterates the action multiple times, probably adding more 
                if (fixFingers()){
                    if (fixVexMotors()){
                        if (fixWrist()){

                        }
                    }
                }
            break;
        default:
            break;
    }
  }

void loop() {
    Tick();
}

bool fixFingers(){
    return false;
}

bool fixVexMotors(){
    if (vexPTickCount != 0){
        if (vexPTickCount > 0){ // going counter-clockwise to go to original position
            digitalWrite(AIN1, LOW);
            digitalWrite(AIN2, HIGH);
            delay(25);
            vexPTickCount--;
        }
        else{ // going clockwise to go to original position
            digitalWrite(AIN1, HIGH);
            digitalWrite(AIN2, LOW);
            delay(25);
            vexPTickCount++;
        }            
    }

    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);

    if (vexRtickount != 0){
        if (vexRTickCount > 0){ // going counter-clockwise to go to original position
            digitalWrite(BIN1, LOW);
            digitalWrite(BIN2, HIGH);
            delay(25);
            vexRTickCount--;
        }
        else{ // going clockwise to go to original position
            digitalWrite(BIN1, HIGH);
            digitalWrite(BIN2, LOW);
            delay(25);
            vexRTickCount++;
        }
    }

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);

    if  (vexRTickCount == 0 && vexRTickCount == 0){
        return true;
    }
    return false;
}

bool fixWrist(){
    return false;
}