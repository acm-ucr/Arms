#include <string>
#include <Servo.h>

Servo finger1;
Servo finger2;
Servo finger3;
Servo wrist;
Servo vex1;
Servo vex2;

void setup(){
    Serial.begin(9600);
    pinMode(3, INPUT);
    pinMode(5, INPUT);
    pinMode(6, INPUT);
    pinMode(9, INPUT);
    pinMode(10, INPUT);
    pinMode(11, INPUT);
    finger1.attach(3);
    finger2.attach(5);
    finger3.attach(6);
    wrist.attach(9);
    vex1.attach(10);
    vex2.attach(11);
}

enum States { Idle, Seek, Approach, Grip, Lift, MoveToDrop, Release } state;
//PWM Pin 3,5,6,9,10,11
//For the vex motor, the arduino conects to the driver
//Pin D11 goes to PWMA and Pin D10 goes to PWMB
//D13 and D12 go to 
//STBY needs to be connected to high at all times.
// flags
unsigned char startSignal;
unsigned char obj_located;
unsigned char target_located;
bool object_reached;
bool pressure_reached[4];
bool reached_upright;
char buffer[5] = {0,0,0,0,0};
std::string bufferString;
// flags: 0x00

unsigned char object_angle;
int target_angle;

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
		target_angle = toInt(Serial.readString());
                state = Seek;
            }else state = Idle;
            startSignal = 0; //reset startSignal
            break;
        case Seek:
            obj_located = buffer[0];
            target_located = buffer[1];            
            break;
        case Approach:
            break;
        case Grip:
            break;
        case Lift: /* reach to this state when recieved pressure reached from all 4 pressure sensors
            keep checking the reached_up_right until we read that we are at 90
            when done, send the flag that we reached up right to MoveToDrop state
            buffer thats being read: [4, 0/1, X, X, X] (X = not used) 
          */
            if (buffer[1] == '1'){
                reached_up_right = true;
            }
            else{
                reached_up_right = false;
            }
            if (reached_up_right){
                state = Mo
            }
        case MoveToDrop:
            break;
        case Release:
            break;
        default:
            break;
    }
    switch(state){  //state actions
        case Idle:
            break;
        case Seek:
            break;
        case Approach:
            break;
        case Grip:
            break;
        case Lift:
            break;
        case MoveToDrop:
            break;
        case Release:
            break;
        default:
            break;
    }
    buffer = 0;
  }

void loop() {
    state = Idle;
    Tick();
}
