#include <Servo.h>

Servo finger1;
Servo finger2;
Servo finger3;
Servo wrist;
Servo vexR;
Servo vexP;

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
    vexR.attach(10);
    vexP.attach(11);
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
String bufferString;
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
		            target_angle = Serial.readString().toInt();
                state = Seek;
            }else state = Idle;
            startSignal = 0; //reset startSignal
            break;
        case Seek:
            obj_located = buffer[0];
            // target_located = buffer[1]; // not finding target now
            if (obj_located) {
              state = Approach;
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
                      [3, 0/1, 0/1, 0/1, 0/1]
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
            buffer thats being read: [4, 0/1, X, X, X] (X = not used) 
          */
            if (arm_angle == 90.0){
                reached_upright = true;
                state = MoveToDrop;
            }
            else{
                reached_upright = false;
                state = Lift;
            }
        case MoveToDrop:
            if (buffer[1] == '1'){
                target_reached = true;
                state = Release;
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
        case Idle:
            break;
        case Seek:
            
            break;
        case Approach:
            int count = 0;
            if (buffer[1] == '1'){

            }
            else{
                count++;
            }

            if (buffer[2] == '1'){

            } else{
                count++;
            }

            if (buffer[3] == '1'){
                wrist.write(85);
                delay(50);
                wrist.write(90);
            } else{
                count++;
            }

            if (buffer[4] == '1'){
                wrist.write(95);
                delay(50);
                wrist.write(90);
            } else{
                count++;
            }
            
            if (count == 4){ // the object is centered in the camera

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
            break;
        case MoveToDrop:
            break;
        case Release:
            break;
        default:
            break;
    }
  }

void loop() {
    state = Idle;
    Tick();
}
