#include <string>

void setup(){
    Serial.begin(9600);
}

enum States { Idle, Seek, Approach, Grip, Lift, MoveToDrop, Release } state;

// flags
unsigned char startSignal;
unsigned char obj_located;
unsigned char target_located;
bool object_reached;
bool pressure_reached[4];
bool reached_up_right;
char buffer[5] = {0,0,0,0,0};
std::string bufferString;
// flags: 0x00
// char buffer[5] = {0,0,0,0,0}
int object_angle;
int target_angle;

detected_object all_detected_objects[2];  

struct detected_object{
    int angle_to_rotate;
    string obj_name;
};

void Tick(){
    if(Serial.available() > 0){
        bufferString = Serial.readString(); //reads from raspberry pi
    }
    for(int i = 0; i < 5; i++){
        buffer[i] = bufferString[i];    //convert to char array
    }
    switch(state){  //state transitions
        case Idle:
            if(buffer[0] == '1') //if we get a 1 from the serial, we start the 
                state = Seek; //move to seek state
            startSignal = buffer[0];    //get the first bit from buffer
            if(startSignal){                //if true, move to seek state
                state = Seek;
            }
            else state = Idle;
            startSignal = 0; //reset startSignal
            break;
        case Seek:
            obj_located = buffer[0];
            target_located = buffer[1];    
            while(!obj_located || !target_located){ //while we have not found the object or target
                if(obj_located){
                    detected_object obj = {/* whatever the object name is from serial */, buffer[2]}; //get the object name and angle from buffer
                    all_detected_objects[0] = obj; //store the object in the array
                } 
                if(targetfound){
                    detected_object target = {"target", buffer[3]}; //get the target name and angle from buffer
                    all_detected_objects[1] = target; //store the target in the array
                }
            }        
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
            // MAKE SURE TO NOT CLEAR THE BUFFER HERE WE NEED TO KEEP THE 0/1 FOR THE LIFT STATE
            break;
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
