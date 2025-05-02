#include <string>

void setup(){
    Serial.begin(9600);
}

enum States { Idle, Seek, Approach, Grip, Lift, MoveToDrop, Release } state;
//PWM Pin 3,5,6,9,10,11
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
// char buffer[5] = {0,0,0,0,0}
unsigned char object_angle;
int target_angle;
double arm_angle = 90.0;

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
        case Grip: /* reach to this state when we reach to the object
                      keep checking if ALL 4 pressure sensors are fully pressed down, when done send to
                      lift state aka make pressure_reached = true
                      if not reach, continue incrementing 
                      [3, 0/1, 0/1, 0/1, 0/1]
                   */
            int trueCounter = 0;
            for (unsigned i = 1; i < 5; ++i){
                if (buffer[i] == '1'){
                    pressure_reached[i-1] = true;
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
                reached_up_right = true;
                state = MoveToDrop
            }
            else{
                reached_up_right = false;
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
